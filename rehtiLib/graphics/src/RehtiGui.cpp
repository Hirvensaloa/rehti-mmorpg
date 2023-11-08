#include "RehtiGui.hpp"

RehtiGui::RehtiGui(VkInstance instance, VkDevice logDevice, VkPhysicalDevice gpu, GLFWwindow* pWindow, VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass)
	: logDeviceM(logDevice), descPoolM(descPool)
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
}

RehtiGui::~RehtiGui()
{
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
}

void RehtiGui::recordGuiData(VkCommandBuffer cmdBuff)
{
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuff);
}

void RehtiGui::startRender()
{
	ImGui::Render();
}