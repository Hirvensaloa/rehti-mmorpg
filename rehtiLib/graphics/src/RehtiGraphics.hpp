
#include <vulkan/vulkan.h>

#include <optional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <set>

// Forward declarations
struct GLFWwindow;

//Useful structs
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }

};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

// The actual application class
class RehtiGraphics
{
public:
    // Demos the latest changes to the graphics class
    void demo();

private:
    // Functions
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
    /// Creates the swapchain.
    /// </summary>
    void createSwapChain();

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

    // Private members:
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;

    // Gpu
    VkPhysicalDevice gpu;

    // Logicaldevice
    VkDevice logDevice;

    // Queues
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    // Swapchain
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    // Framebuffer
    std::vector<VkFramebuffer> swapChainFramebuffers;

    // Pipeline
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    // Commands
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    // Semaphores
    std::vector<VkSemaphore> imagesReady;
    std::vector<VkSemaphore> rendersFinished;
    std::vector<VkFence>    frameFences; // bad wording?
    std::vector<VkFence>    imageFences;

    // Other variables
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    const int concurrentFrames = 2;
    size_t currentFrame = 0;

    const std::vector<const char*> validationlayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
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


#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

};