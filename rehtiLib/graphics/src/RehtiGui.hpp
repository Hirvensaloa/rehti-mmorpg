#pragma once
#include "../../network/src/api/Types.hpp"
#include "../bindings/imgui_impl_glfw.h"
#include "../bindings/imgui_impl_vulkan.h"
#include "GraphicsTypes.hpp"

#include <functional>
#include <unordered_map>
#include <vector>

// fwd decl
class GraphicsObjectManager;

enum UiTab
{
	Inventory,
	Equipment,
	Skills
};

class RehtiGui
{

public:
	RehtiGui(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, GLFWwindow *pWindow, VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass, std::shared_ptr<GraphicsObjectManager> pGfxManager);
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

	void setEquipment(std::vector<GameItem> equipment);

	bool LoadTextureFromFile(const char *filename, const int id);

private:
	/**
	 * @brief Draws inventory tab to GUI window
	 */
	void drawInventory();

	/**
	 * @brief Draws equipment tab to GUI window
	 */
	void drawEquipment();

	VkDevice logDeviceM;
	VkDescriptorPool descPoolM;

	VkQueue graphicsQueueM;
	VkSampler samplerM;

	std::shared_ptr<GraphicsObjectManager> pGraphicsObjectManagerM;

	UiTab openTabM;

	std::function<void(const int id)> inventoryItemClickCallbackM;

	std::vector<GameItem> inventoryM;
	std::vector<GameItem> equipmentM;

	// map from id to descriptor set required for drawing.
	// GraphicsObjectManager will clean up the resources created.
	std::unordered_map<int, VkDescriptorSet> guiIconsM;
};
