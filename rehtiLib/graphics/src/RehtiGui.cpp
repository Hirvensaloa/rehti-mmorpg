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
	// and also perhaps use proper file paths :DDD
	bool ret = LoadTextureFromFile("H:/rehti/rehti-mmorpg/assets/item/lobster.png", 0);
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
			// TODO USE REAL ID's
			if (ImGui::ImageButton((ImTextureID)guiIconsM[0], ImVec2(32, 32)))
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