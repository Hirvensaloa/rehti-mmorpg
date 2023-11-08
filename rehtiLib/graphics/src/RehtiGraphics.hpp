#pragma once
#include <vulkan/vulkan.h>

#include "AABB.hpp"
#include "Camera.hpp"
#include "GraphicsObjectManager.hpp"
#include "Mesh.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>

// Forward declarations
class RehtiGui;

enum EngineFlags
{
    NO_FLAGS = 0,
    FRAME_BUFFER_RESIZED = 1 << 0,
    UNKNOWN = 1 << 7
};

// Useful structs
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
    bool hasTransferOnlyQueue()
    {
        return transferFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct EngineStatistics
{
    uint64_t frameTime; // In microseconds
    double ftPerSec;    // time per second
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
    RehtiGraphics(uint32_t width = 1200, uint32_t height = 900);

    /// <summary>
    /// Cleans up all the resources used by vulkan.
    /// </summary>
    ~RehtiGraphics();

    /// <summary>
    /// Adds a test cube to the graphics backend for testing purposes.
    /// </summary>
    /// <param name="id">The id of the test cube.</param>
    void addTestObject(int id);

    /**
     * @brief Adds a test game object with a test texture for testing purposes.
     * @param id of the test game object.
     */
    void addTestGameObject(int id);

    /**
     * @brief Adds a game object with the given id to the graphics backend. Also creates a bounding box for the object.
     * @param objectID
     * @param vertices of the model
     * @param indices of the triangles of the model
     * @param texture of the model
     * @param location is the location of the model
     * @return boolean indicating whether the object was added successfully.
     */
    bool addGameObject(int objectID, std::vector<Vertex> vertices, std::vector<uint32_t> indices, ImageData texture,
                       glm::vec3 location = glm::vec3(0.f));

    void moveGameObject(int objectID, glm::vec3 location, float timeInSeconds);
    void movePlayer(int playerID, glm::vec3 location, float timeInSeconds);
    void rotateGameObject(int objectID, float radians, float timeInSeconds);

    void forceGameObjectMove(int objectID, glm::vec3 location);
    void forcePlayerMove(int playerID, glm::vec3 location);

    void forceGameObjectRotate(int objectID, float radians);

    /**
     * @brief Adds an area to the game.
     * @param mapAABBData is the bounding box data of the area.
     * @param vertices are the vertices of the area. They are expected to be moved to their corresponding area
     * coordinates before calling this function.
     * @param indices of the triangles.
     * @param textures of the area. Areas support 5 textures with a blendmap: Order is: blendmap (0), black (1), red
     * (2), green (3), blue (4), alpha (5).
     * @return boolean indicating whether the area was added successfully.
     */
    bool addArea(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::array<ImageData, 6> textures);

    /// <summary>
    /// Transforms object with the given id.
    /// </summary>
    void transformTestObject(int id, glm::mat4 transformation);

    /**
     * @brief Adds a bounding box for the world map
     * @param mapAABBData see mapAABBData
     */
    void addMapBoundingBox(const MapAABBData& mapAABBData);

    /**
     * @brief Traces a ray against all bounding boxes, starting with objects, then characters and lastly the map.
     * @return Hit object containing information about the hit.
     */
    Hit traceClick();

    /// <summary>
    /// Sets flags for engine. Flags can only be set by this interface, not unset.
    /// </summary>
    /// <param name="flags"> to be set</param>
    void setEngineFlags(EngineFlags flags);

    /**
     * @brief Adds a mouse click callback
     * @param callback function after a mouse click has happened
     */
    void addMouseClickCallback(std::function<void(const Hit&)> callback);

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
    /// Creates a texture sampler.
    /// Note: Sampler is completely separate from the actual textures.
    /// Those are created in the GraphicsObjectManager as part of some graphics object.
    /// </summary>
    void createTextureSampler();

    /**
     * @brief Creates resources required for a depth buffer.
     */
    void createDepthResources();

    /**
     * @brief Creates RehtiGui member and initializes it.
     */
    void createGui();

    /// <summary>
    /// Checks for device extension support.
    /// </summary>
    /// <param name="device"> to check extensions for.</param>
    /// <returns>
    /// boolean indicating whether required extensions set by <paramref name="deviceExtensions"/> are met.
    /// </returns>
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool checkValidationLayerSupport();
    bool isDeviceSuitable(VkPhysicalDevice device);

    /**
     * @brief Checks whether the given bounding box is hit by the given ray.
     * @param min is the smaller coordinate of the bounding box.
     * @param max is the larger coordinate of the bounding box.
     * @param rayOrig is the origin of the ray.
     * @param dirInv is the inverse of the direction of the ray.
     * @param t is the distance to the hit point.
     * @return true if hit, false otherwise.
     */
    bool bbHit(const glm::vec3 min, const glm::vec3 max, const glm::vec3 rayOrig, const glm::vec3 dirInv, float& t);

    /**
     * @brief Traces a given ray against a given bounding box.
     * @param orig is the origin of the ray.
     * @param dirInv is the inverse of the direction of the ray.
     * @param boxNode is the bounding box to trace against.
     * @param boxHit is the bounding box that was hit.
     * @return the distance to the hit point.
     */
    bool trace(const glm::vec3 orig, const glm::vec3 dirInv, const AABB* pBoxNode, AABB& boxHit, float& t);

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
    std::vector<const char*> getRequiredExtensions();

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

    /**
     * @brief Looks for a suitable format from given candidates.
     * @param candidates
     * @param tiling
     * @param features
     * @return
     */
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
                                 VkFormatFeatureFlags features);

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

    /**
     * @brief Prints out the given aabb
     * @param aabb
     */
    void debugAABB(const AABB& aabb, int level);

    /**
     * @brief Fills the min and max of the given bounding box.
     * @param vertices to create a bounding box for.
     * @param box to fill.
     */
    void fillAABB(std::vector<Vertex> vertices, AABB& box);

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
    std::unique_ptr<RehtiGui> pGuiM;

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
    VkImageView depthImageViewM;
    VkFormat depthFormatM;
    // Sampler
    VkSampler textureSamplerM;

    // Pipeline
    VkRenderPass renderPassM;
    std::array<VkPipelineLayout, OBJECT_TYPE_COUNT> pipelineLayoutsM;
    std::array<VkPipeline, OBJECT_TYPE_COUNT> pipelinesM;

    // Commands
    VkCommandPool commandPoolM;
    std::vector<VkCommandBuffer> commandBuffersM;

    // Semaphores
    std::vector<VkSemaphore> imagesReadyM;
    std::vector<VkSemaphore> rendersFinishedM;
    std::vector<VkFence> frameFencesM;

    // Callbacks
    std::function<void(const Hit&)> mouseClickCallbackM;

    // Other variables
    uint32_t widthM;
    uint32_t heightM;
    float anisotropyM; // default val. Changed in <cref=isDeviceSuitable>
    EngineFlags engineFlagsM = EngineFlags::NO_FLAGS;
    EngineStatistics statsM;
    // Bounding box lists in an array. Each index corresponds to an object type.
    std::array<std::map<int, AABB>, OBJECT_TYPE_COUNT> boundingBoxesM;

    const int kConcurrentFramesM = 2;
    size_t currentFrameM = 0;

    bool validationLayersEnabledM = false;

    const std::vector<const char*> kValidationlayersM = {"VK_LAYER_KHRONOS_validation"};

    const std::vector<const char*> kDeviceExtensionsM = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // Debugging functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                 const VkAllocationCallbacks* pAllocator,
                                                 VkDebugUtilsMessengerEXT* pDebugMessenger);

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks* pAllocator);

    static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
};