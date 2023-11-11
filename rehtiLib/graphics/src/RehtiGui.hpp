#pragma once
#include "../../network/src/api/Types.hpp"
#include "../bindings/imgui_impl_glfw.h"
#include "../bindings/imgui_impl_vulkan.h"

#include <functional>
#include <vector>

class RehtiGui
{

	// A struct to manage data related to one image in vulkan
	struct MyTextureData
	{
		VkDescriptorSet DS; // Descriptor set: this is what you'll pass to Image()
		int Width;
		int Height;
		int Channels;

		// Need to keep track of these to properly cleanup
		VkImageView ImageView;
		VkImage Image;
		VkDeviceMemory ImageMemory;
		VkSampler Sampler;
		VkBuffer UploadBuffer;
		VkDeviceMemory UploadBufferMemory;

		MyTextureData() { memset(this, 0, sizeof(*this)); }
	};

public:
	RehtiGui(VkInstance instance, VkDevice logDevice, VkPhysicalDevice gpu, GLFWwindow *pWindow, VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass, std::vector<VkCommandBuffer> commandBuffers);
	~RehtiGui();

	void uploadFonts(VkCommandBuffer cmdBuffer);
	void uploadEnded();
	void newFrame();

	// records ui primitives into the given command buffer.
	void recordGuiData(VkCommandBuffer cmdBuff);

	// starts rendering ui primitives. Must be called at the top of draw function.
	void startRender();

	void addInventoryItemClickCallback(std::function<void(const int id)> callback);

	void setInventory(std::vector<GameItem> inventory);

	uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
	bool LoadTextureFromFile(const char *filename, MyTextureData *tex_data);
	void RemoveTexture(MyTextureData *tex_data);

private:
	VkDevice logDeviceM;
	VkDescriptorPool descPoolM;

	VkPhysicalDevice physDeviceM;

	std::vector<VkCommandBuffer> commandBuffersM;

	VkQueue graphicsQueueM;

	MyTextureData testTextureM;

	bool inventoryOpenM = true;

	bool equipmentOpenM = false;

	bool skillsOpenM = false;

	std::function<void(const int id)> inventoryItemClickCallbackM;

	std::vector<GameItem> inventoryM;
};
