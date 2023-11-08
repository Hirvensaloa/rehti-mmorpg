#pragma once
#include "../bindings/imgui_impl_glfw.h"
#include "../bindings/imgui_impl_vulkan.h"


class RehtiGui
{
public:
	RehtiGui(VkInstance instance, VkDevice logDevice, VkPhysicalDevice gpu, GLFWwindow* pWindow, VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass);
	~RehtiGui();

	void uploadFonts(VkCommandBuffer cmdBuffer);
	void uploadEnded();
	void newFrame();

	// records ui primitives into the given command buffer.
	void recordGuiData(VkCommandBuffer cmdBuff);

	// starts rendering ui primitives. Must be called at the top of draw function.
	void startRender();

private:
	VkDevice logDeviceM;
	VkDescriptorPool descPoolM;
};
