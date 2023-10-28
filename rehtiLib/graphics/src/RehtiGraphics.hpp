#pragma once
#include <vulkan/vulkan.h>

#include <optional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>
#include <memory>
#include <chrono>

#include "Mesh.hpp"
#include "Camera.hpp"
#include "GraphicsObjectManager.hpp"

enum EngineFlags
{
	NO_FLAGS = 0,
	FRAME_BUFFER_RESIZED = 1 << 0,
	UNKNOWN = 1 << 7
};

//Useful structs
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
    bool hasTransferOnlyQueue() {
		return transferFamily.has_value();
	}
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct EngineStatistics
{
	uint64_t frameTime; // In microseconds
	double ftPerSec; // time per second
};

// The actual application class
class RehtiGraphics
{
public:
    // Demos the latest changes to the graphics class
    void demo();

    /// <summary>
    /// Initializes the window and vulkan.
    /// </summary>
    RehtiGraphics();

    /// <summary>
    /// Cleans up all the resources used by vulkan.
    /// </summary>
    ~RehtiGraphics();

    /// <summary>
    /// Adds a test cube to the graphics backend for testing purposes.
    /// </summary>
	/// <param name="id">The id of the test cube.</param>
    void addTestObject(int id);

	/// <summary>
	/// Transforms object with the given id.
	/// </summary>
	void transformTestObject(int id, glm::mat4 transformation);

	/// <summary>
	/// Sets flags for engine. Flags can only be set by this interface, not unset.
	/// </summary>
	/// <param name="flags"> to be set</param>
	void setEngineFlags(EngineFlags flags);

private:
    // Functions
    
    /// <summary>
    /// Initializes glfw window.
    /// </summary>
    void initWindow();

    /// <summary>
    /// Initializes vulkan and creates the instance.
    /// </summary>
    void initVulkan();

    /// <summary>
    /// populates debug messenger info.
    /// </summary>
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    /// <summary>
    /// Setups the debug messenger.
    /// </summary>
    void setupDebugMessenger();

    /// <summary>
    /// Chooses an appropriate gpu.
    /// </summary>
    void pickPhysicalDevice();

    /// <summary>
    /// Creates the interactable logical device.
    /// </summary>
    void createLogicalDevice();

    /// <summary>
    /// Creates the graphics object manager.
    /// </summary>
    void createObjectManager();

    /// <summary>
    /// Creates the swapchain.
    /// </summary>
    void createSwapChain();

	/// <summary>
	/// Recreates the swapchain.
	/// </summary>
	void recreateSwapChain();

	/// <summary>
	/// Cleans up swapchain related resources. This function is used for easier recreation of the swap chain.
	/// </summary>
	void cleanupSwapChain();

    /// <summary>
    /// Creates the image views.
    /// </summary>
    void createImageViews();

    /// <summary>
    /// Creates the render pass
    /// </summary>
    void createRenderPass();

    /// <summary>
    /// Creates the graphics pipeline.
    /// </summary>
    void createGraphicsPipeline();

    /// <summary>
    /// Creates the framebuffers.
    /// </summary>
    void createFramebuffers();

    /// <summary>
    /// Creates the command pool.
    /// </summary>
    void createCommandPool();

    /// <summary>
    /// Creates the command buffers.
    /// </summary>
    void createCommandBuffers();

	/// <summary>
	/// Records command buffers
	/// </summary>
	/// <param name="cmdBuffer">  to record</param>
	/// <param name="imageIndex"> is the index of the swap chain image to write to</param>
	void recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imageIndex);

    /// <summary>
    /// Creates the appropriate semaphores and fences.
    /// </summary>
    void createSynchronization();

    /// <summary>
    /// Draws a frame.
    /// </summary>
    void drawFrame();
    
    /// <summary>
    /// Loops, polls events and draws frames.
    /// </summary>
    void mainLoop();

    /// <summary>
    /// Cleans up used resources.
    /// </summary>
    void cleanup();

    // Helper functions

    /// <summary>
    /// Creates vulkan instance.
    /// </summary>
    void createInstance();

    /// <summary>
    /// Creates a surface to draw on.
    /// </summary>
    void createSurface();

    /// <summary>
    /// Checks for device extension support.
    /// </summary>
    /// <param name="device">Gpu to check extensions for.</param>
    /// <returns>
    /// boolean indicating whether required extensions set by <paramref name="deviceExtensions"/> are met.
    /// </returns>
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool checkValidationLayerSupport();
    bool isDeviceSuitable(VkPhysicalDevice device);

    /// <summary>
    /// Rates a given GPU
    /// </summary>
    /// <param name="device">gpu.</param>
    /// <returns>A score as an integer</returns>
    int rateDevice(VkPhysicalDevice device);

    /// <summary>
    /// Looks for a queue family that supports graphics and presentation operations.
    /// </summary>
    /// <param name="device">gpu.</param>
    /// <returns>The </returns>
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    /// <summary>
    /// Looks for swapchain support.
    /// </summary>
    /// <param name="device">gpu.</param>
    /// <returns>Details of the support available.</returns>
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    /// <summary>
    /// Looks for the required extensions set by the GLFW library.
    /// </summary>
    /// <returns>The extensions.</returns>
    std::vector<const char* > getRequiredExtensions();

    /// <summary>
    /// Chooses a suitable surface format.
    /// </summary>
    /// <param name="availableFormats">List of formats available.</param>
    /// <returns>The chosen format.</returns>
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);

    /// <summary>
    /// Chooses a presentation mode.
    /// </summary>
    /// <param name="availableModes">List of available modes.</param>
    /// <returns>The chosen presentation format.</returns>
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availableModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    /// <summary>
    /// Returns push constant range object for the camera matrix.
    /// </summary>
    /// <returns></returns>
    VkPushConstantRange getCameraRange();

	/// <summary>
	/// Returns the index of the next frame to be drawn.
	/// </summary>
	/// <returns></returns>
	size_t getNextFrame();

	/// <summary>
	/// Prints out the given matrix.
	/// </summary>
	/// <param name="matrix"></param>
	void debugMatrix(glm::mat4 matrix);

    // Private members:
    GLFWwindow* pWindowM;
    VkInstance instanceM;
    VkDebugUtilsMessengerEXT debugMessengerM;
    VkSurfaceKHR surfaceM;

    // Camera
    Camera cameraM;

    // Gpu
    VkPhysicalDevice gpuM;

    // Logicaldevice
    VkDevice logDeviceM;

    // Auxiliary classes
    std::unique_ptr<GraphicsObjectManager> pObjectManagerM;

    // Queues
    VkQueue graphicsQueueM;
    VkQueue presentQueueM;

    // Swapchain
    VkSwapchainKHR swapChainM;
    std::vector<VkImage> swapChainImagesM;
    VkFormat swapChainImageFormatM;
    VkExtent2D swapChainExtentM;
    std::vector<VkImageView> swapChainImageViewsM;

    // Framebuffer
    std::vector<VkFramebuffer> swapChainFramebuffersM;
	// Depth image
	AllocatedImage depthImageM;

    // Pipeline
    VkRenderPass renderPassM;
    VkPipelineLayout pipelineLayoutM;
    VkPipeline pipelineM;

    // Commands
    VkCommandPool commandPoolM;
    std::vector<VkCommandBuffer> commandBuffersM;

    // Semaphores
    std::vector<VkSemaphore> imagesReadyM;
    std::vector<VkSemaphore> rendersFinishedM;
    std::vector<VkFence>    frameFencesM;

    // Other variables
    uint32_t widthM = 800;
    uint32_t heightM = 600;
	EngineFlags engineFlagsM = EngineFlags::NO_FLAGS;
	EngineStatistics statsM;

    const int kConcurrentFramesM = 2;
    size_t currentFrameM = 0;

    bool validationLayersEnabledM = false;

    const std::vector<const char*> kValidationlayersM = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> kDeviceExtensionsM = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Debugging functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );

    static VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger);

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

};