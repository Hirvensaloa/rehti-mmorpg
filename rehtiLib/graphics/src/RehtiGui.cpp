#include "RehtiGui.hpp"
#include "GraphicsObjectManager.hpp"

#include "stb_image.h"
#include <iostream>

// Helper function to load an image with common settings and return a MyTextureData with a VkDescriptorSet as a sort of
// Vulkan pointer
bool RehtiGui::LoadTextureFromFile(const char* filename, const int id)
{
    int width = 0, height = 0;
    // Specifying 4 channels forces stb to load the image in RGBA which is an easy format for Vulkan
    unsigned char* image_data = stbi_load(filename, &width, &height, 0, 4);
    ImageData imgData{};
    imgData.pixels = image_data;
    imgData.width = width;
    imgData.height = height;
    if (image_data == NULL)
        return false;

    CombinedImage img = pGraphicsObjectManagerM->createCombinedImage(static_cast<uint32_t>(width), static_cast<uint32_t>(height), VK_FORMAT_R8G8B8A8_UNORM);

    // Create Descriptor Set using ImGUI's implementation
    VkDescriptorSet imageSet = ImGui_ImplVulkan_AddTexture(samplerM, img.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // copy to gpu and transition the image layout
    pGraphicsObjectManagerM->copyImage(img.imageAllocation, imgData);
    // set icon to id
    guiIconsM[id] = imageSet;
    // Release image memory using stb
    stbi_image_free(image_data);

    return true;
}

RehtiGui::RehtiGui(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, GLFWwindow* pWindow, VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass, std::shared_ptr<GraphicsObjectManager> pGfxManager)
    : logDeviceM(logDevice), descPoolM(descPool), pGraphicsObjectManagerM(pGfxManager)
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

    // Create Sampler
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
    if (vkCreateSampler(logDeviceM, &sampler_info, nullptr, &samplerM) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler for gui system.");
    }
    // check_vk_result(err);
    // TODO USE PROPER ID's, OR LOAD THE IMAGE IN ANOTHER PLACE
    bool ret = LoadTextureFromFile("./assets/item/lobster.png", 10);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/item/iron_sword.png", 1);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/headslot.png", -1);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/neckslot.png", -2);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/topslot.png", -3);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/mainhandslot.png", -4);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/offhandslot.png", -5);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/bottomslot.png", -6);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/bootslot.png", -7);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/gloveslot.png", -8);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/ringslot.png", -9);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/no-icon.png", -10);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/woodcutting.png", -11);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/mining.png", -12);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/fishing.png", -13);
    IM_ASSERT(ret);
    ret = LoadTextureFromFile("./assets/gui/cooking.png", -14);
    IM_ASSERT(ret);
}

RehtiGui::~RehtiGui()
{
    vkDestroyDescriptorPool(logDeviceM, descPoolM, nullptr); // destroys the set in the map as well.
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

    ImGui::SetWindowSize(ImVec2(windowWidthM, windowHeightM));

    if (ImGui::BeginTabBar("GUITabBar", 0))
    {
        if (ImGui::BeginTabItem("Inventory"))
        {
            openTabM = UiTab::Inventory;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Equipment"))
        {
            openTabM = UiTab::Equipment;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skills"))
        {
            openTabM = UiTab::Skills;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (openTabM == UiTab::Inventory)
    {
        drawInventory();
    }

    if (openTabM == UiTab::Equipment)
    {
        drawEquipment();
    }

    if (openTabM == UiTab::Skills)
    {
        drawSkills();
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

void RehtiGui::addEquipmentItemClickCallback(std::function<void(const int id)> callback)
{
    equipmentItemClickCallbackM = callback;
}

void RehtiGui::setInventory(std::vector<GameItem> inventory)
{
    std::unique_lock<std::mutex> lck(invMutexM);
    inventoryM = std::move(inventory);
}

void RehtiGui::setEquipment(std::vector<GameItem> equipment)
{
    std::unique_lock<std::mutex> lck(equipmentMutexM);
    equipmentM = std::move(equipment);
}

void RehtiGui::setSkills(std::vector<Skill> skills)
{
    std::unique_lock<std::mutex> lck(skillsMutexM);
    skillsM = std::move(skills);
}

void RehtiGui::drawInventory()
{
    std::unique_lock<std::mutex> lck(invMutexM);
    for (int i = 0; i < inventoryM.size(); i++)
    {
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));

        if (ImGui::ImageButton((ImTextureID)(guiIconsM.contains(inventoryM[i].id) ? guiIconsM[inventoryM[i].id] : guiIconsM[-10]), iconSizeM))
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

void RehtiGui::drawEquipmentSlot(int index)
{
    VkDescriptorSet icon = (equipmentM[index].id == -1 ? guiIconsM[-(index + 1)] : (guiIconsM.contains(equipmentM[index].id) ? guiIconsM[equipmentM[index].id] : guiIconsM[-10]));
    if (ImGui::ImageButton((ImTextureID)icon, iconSizeM))
    {
        std::cout << equipmentM[index].name << std::endl;
        equipmentItemClickCallbackM(equipmentM[index].instanceId);
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text(("Unequip " + equipmentM[index].name).c_str());
        ImGui::EndTooltip();
    }
}

void RehtiGui::drawEquipment()
{
    std::unique_lock<std::mutex> lck(equipmentMutexM);

    ImGui::NewLine();
    ImGui::SameLine(windowWidthM / 2 - iconSizeM[0] / 2);
    drawEquipmentSlot(0);

    ImGui::NewLine();
    ImGui::SameLine(windowWidthM / 2 - iconSizeM[0] / 2);
    drawEquipmentSlot(1);

    ImGui::NewLine();
    ImGui::SameLine(windowWidthM / 2 - iconSizeM[0] / 2);
    drawEquipmentSlot(2);

    ImGui::SameLine(windowWidthM / 4 - iconSizeM[0] / 2);
    drawEquipmentSlot(3);

    ImGui::SameLine(windowWidthM * 3 / 4 - iconSizeM[0] / 2);
    drawEquipmentSlot(4);

    ImGui::NewLine();
    ImGui::SameLine(windowWidthM / 2 - iconSizeM[0] / 2);
    drawEquipmentSlot(5);

    ImGui::NewLine();
    ImGui::SameLine(windowWidthM / 2 - iconSizeM[0] / 2);
    drawEquipmentSlot(6);

    ImGui::SameLine(windowWidthM / 4 - iconSizeM[0] / 2);
    drawEquipmentSlot(7);

    ImGui::SameLine(windowWidthM * 3 / 4 - iconSizeM[0] / 2);
    drawEquipmentSlot(8);
}

void RehtiGui::drawSkills()
{
    std::unique_lock<std::mutex> lck(skillsMutexM);
    for (auto& skill : skillsM)
    {

        VkDescriptorSet& icon = guiIconsM.contains(-(skill.id + 11)) ? guiIconsM[-(skill.id + 11)] : guiIconsM[-10];
        ImGui::Image((ImTextureID)icon, iconSizeM);
        ImGui::SameLine();
        ImGui::Text((skill.name + ": " + std::to_string(skill.xp) + " XP").c_str());
    }
}