#pragma once
#include "GraphicsTypes.hpp"
#include "../../network/src/api/Types.hpp"
#include "../bindings/imgui_impl_glfw.h"
#include "../bindings/imgui_impl_vulkan.h"

#include <functional>
#include <vector>
#include <unordered_map>
// fwd decl
class GraphicsObjectManager;

class RehtiGui
{

public:
	RehtiGui(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, GLFWwindow* pWindow, VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass, std::shared_ptr<GraphicsObjectManager> pGfxManager);
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

	bool LoadTextureFromFile(const char* filename, const int id);

private:
	VkDevice logDeviceM;
	VkDescriptorPool descPoolM;

	VkQueue graphicsQueueM;
	VkSampler samplerM;

	std::shared_ptr<GraphicsObjectManager> pGraphicsObjectManagerM;

	bool inventoryOpenM = true;

	bool equipmentOpenM = false;

	bool skillsOpenM = false;

	std::function<void(const int id)> inventoryItemClickCallbackM;

	std::vector<GameItem> inventoryM;
	// map from id to descriptor set required for drawing.
	// GraphicsObjectManager will clean up the resources created.
	std::unordered_map<int, VkDescriptorSet> guiIconsM;
};
