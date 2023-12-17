#pragma once
#include <vulkan/vulkan.h>

#include "AABB.hpp"
#include "Camera.hpp"
#include "GraphicsObjectManager.hpp"
#include "Mesh.hpp"
#include "TimerCallbackSystem.hpp"

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
    /**
     * @brief Starts a rendering loop until the window is closed.
     */
    void startMainLoop();

    /**
     * @brief Initializes the graphics backend.
     * @param width of the window
     * @param height of the window
     */
    RehtiGraphics(uint32_t width = 1200, uint32_t height = 900, glm::vec3 cameraLocation = glm::vec3(0.f));

    /// <summary>
    /// Cleans up all the resources used by vulkan.
    /// </summary>
    ~RehtiGraphics();

    /**
     * @brief Adds a character object with the given id to the graphics backend. Also creates a bounding box for the object.
     * @param characterID of the character object.
     * @param vertices of the model
     * @param indices of the triangles of the model
     * @param texture of the model
     * @param animations list of Animation structs for the character
     * @param bones list of BoneNode structs for the character
     * @param transformations list of transformation matrices for the character. IMPORTANT: THE FIRST MATRIX IS THE GLOBAL INVERSE TRANSFORMATION MATRIX. So the transformations start from the second matrix.
     * @param location of the character to be placed
     * @param rotation of the character to be placed
     * @return boolean indicating whether the object was added successfully.
     */
    bool addCharacterObject(int characterID, std::vector<CharacterVertex> vertices, std::vector<uint32_t> indices, ImageData texture, std::array<Animation, ANIMATION_TYPE_COUNT> animations, std::vector<BoneNode> bones, std::vector<glm::mat4> transformations, glm::vec3 location = glm::vec3(0.f), float rotation = 0.f, bool isPlayer = false);

    /**
     * @brief Removes a character object with the given id from the graphics backend.
     * @param characterID of the character object.
     * @return boolean indicating whether the object was removed successfully.
     */
    bool removeCharacterObject(int characterID);

    /**
     * @brief Adds a game object with the given id to the graphics backend. Also creates a bounding box for the object.
     * @param objectID
     * @param vertices of the model
     * @param indices of the triangles of the model
     * @param texture of the model
     * @param location is the location of the model
     * @param rotation is the rotation around y axis clockwise
     * @return boolean indicating whether the object was added successfully.
     */
    bool addGameObject(int objectID, std::vector<Vertex> vertices, std::vector<uint32_t> indices, ImageData texture, glm::vec3 location = glm::vec3(0.f), float rotation = 0.f);

    /**
     * @brief Removes a game object with the given id from the graphics backend.
     * @param objectID of the game object.
     * @return boolean indicating whether the object was removed successfully.
     */
    bool removeGameObject(int objectID);

    /**
     * @brief Checks if a game object with the given id exists.
     * @param objectID
     * @return
     */
    bool doesGameObjectExist(int objectID);

    /**
     * @brief Checks if a characterwith the given id exists.
     * @param characterID
     * @return
     */
    bool doesCharacterExist(int characterID);

    /**
     * @brief Moves a game object to the given location in the given time.
     * @param objectID of the object to move.
     * @param location to move the object to.
     * @param timeInSeconds that it takes to move the object.
     */
    void moveGameObject(int objectID, glm::vec3 location, float timeInSeconds);

    /**
     * @brief Rotates a game object by given radian angle in the given time frame.
     * @param objectID of the object to rotate.
     * @param radians to rotate the object by.
     * @param timeInSeconds that it takes to rotate the object.
     */
    void rotateGameObject(int objectID, float radians, float timeInSeconds);

    /**
     * @brief Forces the game object to move to the given location, quitting any remaining movement callbacks.
     * @param objectID of the object to move.
     * @param location to move the object to.
     */
    void forceGameObjectMove(int objectID, glm::vec3 location);

    /**
     * @brief Forces game object rotation to the given angle, quitting any remaining rotation callbacks.
     * @param objectID of the object to rotate.
     * @param radians to rotate the object by.
     */
    void forceGameObjectRotate(int objectID, float radians);

    /**
     * @brief Moves the player to the given location in the given time.
        Also automatically rotates the player to face the given location and sets the animation.
        Player movement differs from character movement in a sense that the camera is moved as well.
     * @param playerID of the player to move.
     * @param location to move the player to.
     * @param timeInSeconds it takes for the player to move to that location.
    */
    void movePlayer(int playerID, glm::vec3 location, float timeInSeconds);

    /**
     * @brief Plays an animation for the given character.
     * @param characterID
     * @param cfg
     */
    void playAnimation(int characterID, AnimationConfig cfg);

    /**
     * @brief Forces player to move to the location given, cancelling any remaining movement callbacks and animations.
     * @param playerID of the player to move.
     * @param location to move the player to.
     */
    void forcePlayerMove(int playerID, glm::vec3 location);

    /**
     * @brief Moves character to the given location in the given time. Automatically sets the running animation and character rotation.
     * @param characterID to move
     * @param location to move the character into
     * @param timeInSeconds that this action takes.
     */
    void moveCharacter(int characterID, glm::vec3 location, float timeInSeconds);

    /**
     * @brief Forces character to the given location, cancelling any remaining movement callbacks and animations.
     * @param characterID
     * @param location
     */
    void forceCharacterMove(int characterID, glm::vec3 location);

    /**
     * @brief Adds an area to the game.
     * @param vertices are the vertices of the area. They are expected to be moved to their corresponding area coordinates before calling this function.
     * @param indices of the triangles.
     * @param textures of the area. Areas support 5 textures with a blendmap: Order is: blendmap (0), black (1), red (2), green (3), blue (4), alpha (5).
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

    std::shared_ptr<RehtiGui> getGui();

private:
    // Functions

    /**
     * @brief Initializes the window.
     */
    void initWindow();

    /**
     * @brief Initializes vulkan instance.
     */
    void initVulkan();

    /**
     * @brief Populates the debug messenger info.
     * @param createInfo
     */
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    /**
     * @brief Setups the debug messenger.
     */
    void setupDebugMessenger();

    /**
     * @brief  Chooses an appropriate gpu.
     */
    void pickPhysicalDevice();

    /**
     * @brief Creates the interactable logical device.
     */
    void createLogicalDevice();

    /**
     * @brief Creates the graphics object manager.
     */
    void createObjectManager();

    /// <summary>
    /// Creates the swapchain.
    /// </summary>
    void createSwapChain();

    /**
     * @brief Recreates the swapchain.
     */
    void recreateSwapChain();

    /**
     * @brief Cleans up swapchain related resources. This function is used for easier recreation of the swap chain.
     */
    void cleanupSwapChain();

    /**
     * @brief Creates the image views.
     */
    void createImageViews();

    /**
     * @brief Creates the render pass.
     */
    void createRenderPass();

    /**
     * @brief Creates the graphics pipeline.
     */
    void createGraphicsPipeline();

    /**
     * @brief Creates the framebuffers.
     */
    void createFramebuffers();

    /**
     * @brief Creates the command pool.
     */
    void createCommandPool();

    /**
     * @brief Creates the command buffers.
     */
    void createCommandBuffers();

    /**
     * @brief Records the command buffer.
     * @param cmdBuffer to record to.
     * @param imageIndex of the swapchain image.
     */
    void recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imageIndex);

    /**
     * @brief Initializes the appropriate semaphores and fences.
     */
    void createSynchronization();

    /**
     * @brief Draws a frame and records the previous frame time.
     */
    void drawFrame();

    /**
     * @brief Loops, polls events and draws frames.
     */
    void mainLoop();

    /**
     * @brief Cleans up used resources.
     */
    void cleanup();

    // Helper functions

    /**
     * @brief Creates vulkan instance.
     */
    void createInstance();

    /**
     * @brief Creates a surface to draw on.
     */
    void createSurface();

    /**
     * @brief Creates a texture sampler.
     */
    void createTextureSampler();

    /**
     * @brief Creates resources required for a depth buffer.
     */
    void createDepthResources();

    /**
     * @brief Creates RehtiGui member and initializes it.
     */
    void createGui();

    /**
     * @brief Checks whether the given device supports the required extensions.
     * @param device to check.
     * @return boolean indicating whether the device supports the required extensions.
     */
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    /**
     * @brief Checks whether the given device supports the required layers.
     * @return boolean indicating whether the layers are found.
     */
    bool checkValidationLayerSupport();

    /**
     * @brief Checks whether the given device supports the required features.
     * @param device to check.
     * @return boolean indicating whether the device supports the required features.
     */
    bool isDeviceSuitable(VkPhysicalDevice device);

    /**
     * @brief Checks whether the given bounding box is hit by the given ray.
     * @param min is the smaller coordinate of the bounding box.
     * @param max is the larger coordinate of the bounding box.
     * @param rayOrig is the origin of the ray.
     * @param dirInv is the coordinate-wise inverse of the direction of the ray.
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

    /**
     * @brief Rates the given gpu.
     * @param device to rate.
     */
    int rateDevice(VkPhysicalDevice device);

    /**
     * @brief Looks for queue families.
     * @param device to look queues for.
     * @return Struct indicating queue family support.
     */
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    /**
     * @brief Looks for swapchain support.
     * @param device to look support for.
     * @return Swapchain support details.
     */
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    /**
     * @brief Returns the required extensions as c strings.
     * @return vector of c strings.
     */
    std::vector<const char*> getRequiredExtensions();

    /**
     * @brief Chooses a surface format.
     * @param availableFormats to choose from.
     * @return format from the list.
     */
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats);

    /**
     * @brief Chooses a present mode.
     * @param availableModes to choose from.
     * @return Presentmode chosen.
     */
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availableModes);

    /**
     * @brief Chooses a swap extent based on the provided capabilities.
     * @param capabilities of the surface in use.
     * @return VkExtent2D object.
     */
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    /**
     * @brief Returns the size of the camera matrix.
     * @return VkPushConstantRange object.
     */
    VkPushConstantRange getCameraRange();

    /**
     * @brief Looks for a suitable format from given candidates.
     * @param candidates
     * @param tiling
     * @param features
     * @return
     */
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    /**
     * @brief Returns the index of the next frame.
     * @return
     */
    size_t getNextFrame();

    /**
     * @brief Helper function to move a bounding box.
     * @param objectID of the object
     * @param objType of the object
     * @param location that the object is to be moved to
     */
    void moveBoundingBox(int objectID, ObjectType objType, glm::vec3 location);

    /**
     * @brief Prints out a matrix
     * @param matrix to print
     */
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
    std::shared_ptr<GraphicsObjectManager> pObjectManagerM;
    std::shared_ptr<RehtiGui> pGuiM;

    // Queues
    std::shared_mutex graphicsQueueMutexM;
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
    float anisotropyM;
    EngineFlags engineFlagsM = EngineFlags::NO_FLAGS;
    EngineStatistics statsM;

    std::shared_mutex dataMutexM; ///< Mutex that must be acquired before modifying the data structures below (timer has its own mutex)
    // Bounding box lists in an array. Each index corresponds to an object type.
    std::array<std::map<int, AABB>, OBJECT_TYPE_COUNT> boundingBoxesM;
    // Location and animation storage
    std::map<int, GfxOrientation> gameObjectOrientationsM;
    std::map<int, CharacterData> characterOrientationsM;

    DirectionalLight sunM;

    // timer callback system
    TimerCallbackSystem timersM;

    const int kConcurrentFramesM = 2;
    size_t currentFrameM = 0;

    bool validationLayersEnabledM = false;

    const std::vector<const char*> kValidationlayersM = {
        "VK_LAYER_KHRONOS_validation"};

    const std::vector<const char*> kDeviceExtensionsM = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // Debugging functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

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