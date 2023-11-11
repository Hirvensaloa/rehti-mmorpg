#include "RehtiGui.hpp"

#include "stb_image.h"
#include <iostream>

// Helper function to find Vulkan memory type bits. See ImGui_ImplVulkan_MemoryType() in imgui_impl_vulkan.cpp
uint32_t RehtiGui::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(physDeviceM, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    return 0xFFFFFFFF; // Unable to find memoryType
}

// Helper function to load an image with common settings and return a MyTextureData with a VkDescriptorSet as a sort of
// Vulkan pointer
bool RehtiGui::LoadTextureFromFile(const char *filename, MyTextureData *tex_data)
{
    // Specifying 4 channels forces stb to load the image in RGBA which is an easy format for Vulkan
    tex_data->Channels = 4;
    unsigned char *image_data = stbi_load(filename, &tex_data->Width, &tex_data->Height, 0, tex_data->Channels);

    if (image_data == NULL)
        return false;

    // Calculate allocation size (in number of bytes)
    size_t image_size = tex_data->Width * tex_data->Height * tex_data->Channels;

    VkResult err;

    // Create the Vulkan image.
    {
        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.format = VK_FORMAT_R8G8B8A8_UNORM;
        info.extent.width = tex_data->Width;
        info.extent.height = tex_data->Height;
        info.extent.depth = 1;
        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        err = vkCreateImage(logDeviceM, &info, nullptr, &tex_data->Image);
        // check_vk_result(err);
        VkMemoryRequirements req;
        vkGetImageMemoryRequirements(logDeviceM, tex_data->Image, &req);
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = req.size;
        alloc_info.memoryTypeIndex = findMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        err = vkAllocateMemory(logDeviceM, &alloc_info, nullptr, &tex_data->ImageMemory);
        // check_vk_result(err);
        err = vkBindImageMemory(logDeviceM, tex_data->Image, tex_data->ImageMemory, 0);
        // check_vk_result(err);
    }

    // Create the Image View
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = tex_data->Image;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = VK_FORMAT_R8G8B8A8_UNORM;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.layerCount = 1;
        err = vkCreateImageView(logDeviceM, &info, nullptr, &tex_data->ImageView);
        // check_vk_result(err);
    }

    // Create Sampler
    {
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // outside image bounds just use border color
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.minLod = -1000;
        sampler_info.maxLod = 1000;
        sampler_info.maxAnisotropy = 1.0f;
        err = vkCreateSampler(logDeviceM, &sampler_info, nullptr, &tex_data->Sampler);
        // check_vk_result(err);
    }

    // Create Descriptor Set using ImGUI's implementation
    tex_data->DS =
        ImGui_ImplVulkan_AddTexture(tex_data->Sampler, tex_data->ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Create Upload Buffer
    {
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = image_size;
        buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        err = vkCreateBuffer(logDeviceM, &buffer_info, nullptr, &tex_data->UploadBuffer);
        // check_vk_result(err);
        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(logDeviceM, tex_data->UploadBuffer, &req);
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = req.size;
        alloc_info.memoryTypeIndex = findMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        err = vkAllocateMemory(logDeviceM, &alloc_info, nullptr, &tex_data->UploadBufferMemory);
        // check_vk_result(err);
        err = vkBindBufferMemory(logDeviceM, tex_data->UploadBuffer, tex_data->UploadBufferMemory, 0);
        // check_vk_result(err);
    }

    // Upload to Buffer:
    {
        void *map = NULL;
        err = vkMapMemory(logDeviceM, tex_data->UploadBufferMemory, 0, image_size, 0, &map);
        // check_vk_result(err);
        memcpy(map, image_data, image_size);
        VkMappedMemoryRange range[1] = {};
        range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range[0].memory = tex_data->UploadBufferMemory;
        range[0].size = image_size;
        err = vkFlushMappedMemoryRanges(logDeviceM, 1, range);
        // check_vk_result(err);
        vkUnmapMemory(logDeviceM, tex_data->UploadBufferMemory);
    }

    // Release image memory using stb
    stbi_image_free(image_data);

    // Create a command buffer that will perform following steps when hit in the command queue.
    // TODO: this works in the example, but may need input if this is an acceptable way to access the pool/create the
    // command buffer. VkCommandPool command_pool = g_MainWindowData.Frames[g_MainWindowData.FrameIndex].CommandPool;
    VkCommandBuffer command_buffer = commandBuffersM[0];
    // {
    // 	VkCommandBufferAllocateInfo alloc_info{};
    // 	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    // 	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    // 	alloc_info.commandPool = command_pool;
    // 	alloc_info.commandBufferCount = 1;

    // 	err = vkAllocateCommandBuffers(logDeviceM, &alloc_info, &command_buffer);
    // // 	check_vk_result(err);

    // 	VkCommandBufferBeginInfo begin_info = {};
    // 	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // 	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    // 	err = vkBeginCommandBuffer(command_buffer, &begin_info);
    // // 	check_vk_result(err);
    // }

    // Copy to Image
    {
        VkImageMemoryBarrier copy_barrier[1] = {};
        copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier[0].image = tex_data->Image;
        copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_barrier[0].subresourceRange.levelCount = 1;
        copy_barrier[0].subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0,
                             NULL, 1, copy_barrier);

        VkBufferImageCopy region = {};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = tex_data->Width;
        region.imageExtent.height = tex_data->Height;
        region.imageExtent.depth = 1;
        vkCmdCopyBufferToImage(command_buffer, tex_data->UploadBuffer, tex_data->Image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier use_barrier[1] = {};
        use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier[0].image = tex_data->Image;
        use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        use_barrier[0].subresourceRange.levelCount = 1;
        use_barrier[0].subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, NULL, 0, NULL, 1, use_barrier);
    }

    // End command buffer
    {
        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        err = vkEndCommandBuffer(command_buffer);
        // check_vk_result(err);
        err = vkQueueSubmit(graphicsQueueM, 1, &end_info, VK_NULL_HANDLE);
        // check_vk_result(err);
        err = vkDeviceWaitIdle(logDeviceM);
        // check_vk_result(err);
    }

    return true;
}

// Helper function to cleanup an image loaded with LoadTextureFromFile
void RehtiGui::RemoveTexture(MyTextureData *tex_data)
{
    vkFreeMemory(logDeviceM, tex_data->UploadBufferMemory, nullptr);
    vkDestroyBuffer(logDeviceM, tex_data->UploadBuffer, nullptr);
    vkDestroySampler(logDeviceM, tex_data->Sampler, nullptr);
    vkDestroyImageView(logDeviceM, tex_data->ImageView, nullptr);
    vkDestroyImage(logDeviceM, tex_data->Image, nullptr);
    vkFreeMemory(logDeviceM, tex_data->ImageMemory, nullptr);
    ImGui_ImplVulkan_RemoveTexture(tex_data->DS);
}

RehtiGui::RehtiGui(VkInstance instance, VkDevice logDevice, VkPhysicalDevice gpu, GLFWwindow *pWindow,
                   VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass,
                   std::vector<VkCommandBuffer> commandBuffers)
    : logDeviceM(logDevice), descPoolM(descPool), physDeviceM(gpu), commandBuffersM(commandBuffers),
      graphicsQueueM(graphicsQueue)
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(pWindow, true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = instance;
    initInfo.Device = logDevice;
    initInfo.PhysicalDevice = gpu;
    initInfo.Queue = graphicsQueue;
    initInfo.MinImageCount = imageCount;
    initInfo.ImageCount = imageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.DescriptorPool = descPool;

    ImGui_ImplVulkan_Init(&initInfo, renderPass);
    bool ret = LoadTextureFromFile("H:/rehti/rehti-mmorpg/assets/item/lobster.png", &testTextureM);
    IM_ASSERT(ret);
}

RehtiGui::~RehtiGui()
{
    RemoveTexture(&testTextureM);
    vkDestroyDescriptorPool(logDeviceM, descPoolM, nullptr);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void RehtiGui::uploadFonts(VkCommandBuffer cmdBuffer)
{
    ImGui_ImplVulkan_CreateFontsTexture(cmdBuffer);
}

void RehtiGui::uploadEnded()
{
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void RehtiGui::newFrame()
{

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Begin("Rehti GUI");

    // if (ImGui::BeginMenuBar())
    // {
    //     if (ImGui::BeginMenu("Inventory"))
    //     {
    //         inventoryOpenM = true;
    //         equipmentOpenM = false;
    //         skillsOpenM = false;
    //         ImGui::EndMenu();
    //     }
    //     ImGui::EndMenuBar();
    // }

    if (ImGui::BeginTabBar("GUITabBar", 0))
    {
        if (ImGui::BeginTabItem("Inventory"))
        {
            inventoryOpenM = true;
            equipmentOpenM = false;
            skillsOpenM = false;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Equipment"))
        {
            inventoryOpenM = false;
            equipmentOpenM = true;
            skillsOpenM = false;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skills"))
        {
            inventoryOpenM = false;
            equipmentOpenM = false;
            skillsOpenM = true;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (inventoryOpenM)
    {

        for (int i = 0; i < inventoryM.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
            // ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
            // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
            if (ImGui::ImageButton((ImTextureID)testTextureM.DS, ImVec2(32, 32)))
            {
                std::cout << inventoryM[i].name << std::endl;
                inventoryItemClickCallbackM(inventoryM[i].instanceId);
            }
            ImGui::PopStyleColor(1);

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::BeginListBox("##listbox1", ImVec2(90, 60)))
                {
                    if (ImGui::Selectable(("Use " + inventoryM[i].name).c_str()))
                    {
                        std::cout << inventoryM[i].name << std::endl;
                        inventoryItemClickCallbackM(inventoryM[i].instanceId);
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable(("Drop " + inventoryM[i].name).c_str()))
                    {
                        ImGui::CloseCurrentPopup(); // TODO: Send drop item request
                    }
                    if (ImGui::Selectable("Cancel"))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndListBox();
                }
                // if (ImGui::Button(("Use " + inventoryM[i].name).c_str()))
                // {
                //     std::cout << inventoryM[i].name << std::endl;
                //     inventoryItemClickCallbackM(inventoryM[i].instanceId);
                //     ImGui::CloseCurrentPopup();
                // }
                // if (ImGui::Button(("Drop " + inventoryM[i].name).c_str()))
                // {
                //     ImGui::CloseCurrentPopup(); // TODO: Send drop item request
                // }
                // if (ImGui::Button("Cancel"))
                // {
                //     ImGui::CloseCurrentPopup();
                // }
                ImGui::EndPopup();
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text(("Use " + inventoryM[i].name).c_str());
                ImGui::EndTooltip();
            }

            ImGui::PopID();

            if ((i + 1) % 4 != 0)
            {
                ImGui::SameLine();
            }
        }
    }
    ImGui::End();
}

void RehtiGui::recordGuiData(VkCommandBuffer cmdBuff)
{
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuff);
}

void RehtiGui::startRender()
{
    ImGui::Render();
}

void RehtiGui::addInventoryItemClickCallback(std::function<void(const int id)> callback)
{
    inventoryItemClickCallbackM = callback;
}

void RehtiGui::setInventory(std::vector<GameItem> inventory)
{
    inventoryM = inventory;
}