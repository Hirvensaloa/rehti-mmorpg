#include "RehtiGraphics.hpp"
#include "RehtiGui.hpp"
#include "ShaderManager.hpp"

#include "GLFW/glfw3.h"
#include <cstring>

void RehtiGraphics::startMainLoop()
{
    mainLoop();
}

RehtiGraphics::RehtiGraphics(uint32_t width, uint32_t height, glm::vec3 cameraLocation)
    : widthM(width), heightM(height), anisotropyM(1.f), cameraM(Camera(cameraLocation, static_cast<float>(width), static_cast<float>(height))),
      sunM({glm::vec3(1.f), glm::vec3(1.f), 1.f})
{
    initWindow();
    cameraM.registerCameraControls(pWindowM);
    initVulkan();
}

RehtiGraphics::~RehtiGraphics()
{
    cleanup();
}

bool RehtiGraphics::addCharacterObject(int characterID, std::vector<CharacterVertex> vertices, std::vector<uint32_t> indices, ImageData texture, std::array<Animation, ANIMATION_TYPE_COUNT> animations, std::vector<BoneNode> bones, std::vector<glm::mat4> transformations, glm::vec3 location, float rotation, bool isPlayer)
{
    if (characterOrientationsM.contains(characterID)) // character already exists
        return false;

    GfxOrientation characterOrientation{location, glm::quat(), glm::vec3(1.f, 1.f, 1.f)};
    CharacterAnimationData animationData{};
    animationData.animations = animations;
    animationData.currentAnimation = AnimationType::IDLE;
    animationData.currentTicks = 0;
    CharacterData charData{};
    charData.inverseGlobalTransformation = transformations[0];
    charData.animationData = animationData;
    charData.characterOrientation = characterOrientation;
    for (size_t i = 0; i < bones.size(); i++)
    {
        BoneNode bone = bones[i];
        charData.bones.push_back(bone);
        charData.boneTransformations[i] = glm::mat4(1.f); // transformations[i + 1];
    }

    PhongMaterial characterMaterial{};
    characterMaterial.ambient = glm::vec3(0.1f);
    characterMaterial.diffuse = glm::vec3(0.5f);
    characterMaterial.specular = glm::vec3(0.5f);
    characterMaterial.shininess = 10.f;

    bool res = pObjectManagerM->addCharacter(characterID, vertices, indices, texture, characterOrientation.getTransformationMatrix(), charData.boneTransformations.data(), textureSamplerM, characterMaterial);
    if (!res)
        return false;

    characterOrientationsM[characterID] = charData;
    AABB bb{};
    bb.min = location + CHARACTER_MIN;
    bb.max = location + CHARACTER_MAX;
    bb.pLeft = nullptr;
    bb.pRight = nullptr;
    boundingBoxesM[ObjectType::CHARACTER][characterID] = bb;

    if (isPlayer)
    {
        cameraM.setTargetAndCamera(location);
    }

    // Set animation to idle to north
    playAnimation(characterID, AnimationConfig{-POSITIVE_Z_AXIS, AnimationType::IDLE, 1.0f, true});

    return true;
}

bool RehtiGraphics::removeCharacterObject(int characterID)
{
    if (!characterOrientationsM.contains(characterID))
        return false;

    pObjectManagerM->cleanResources(characterID, ObjectType::CHARACTER);
    characterOrientationsM.erase(characterID);
    boundingBoxesM[ObjectType::CHARACTER].erase(characterID);

    return true;
}

bool RehtiGraphics::addGameObject(int objectID, std::vector<Vertex> vertices, std::vector<uint32_t> indices, ImageData img, glm::vec3 location, float rotation)
{
    glm::mat4 transformation = glm::translate(glm::mat4(1.f), location);
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.f), rotation, POSITIVE_Y_AXIS);
    transformation = transformation * rotationMatrix;
    bool res = pObjectManagerM->addGameObject(objectID, vertices, indices, img, transformation, textureSamplerM);
    if (!res)
        return false;
    AABB bb;
    bb.max = location + glm::vec3(0.5f, 0.5f, 0.5f); // Game objects have 1 unit bounding boxes
    bb.min = location - glm::vec3(0.5f, 0.5f, 0.5f);
    bb.pLeft = nullptr;
    bb.pRight = nullptr;
    boundingBoxesM[ObjectType::GAMEOBJECT][objectID] = bb;

    gameObjectOrientationsM[objectID] = GfxOrientation{location, glm::quat(), glm::vec3(1.f, 1.f, 1.f)};

    return true;
}

bool RehtiGraphics::removeGameObject(int objectID)
{
    if (!gameObjectOrientationsM.contains(objectID))
        return false;

    pObjectManagerM->cleanResources(objectID, ObjectType::GAMEOBJECT);
    gameObjectOrientationsM.erase(objectID);
    boundingBoxesM[ObjectType::GAMEOBJECT].erase(objectID);

    return true;
}

bool RehtiGraphics::doesGameObjectExist(int objectID)
{
    std::shared_lock lock(dataMutexM);
    return gameObjectOrientationsM.contains(objectID);
}

bool RehtiGraphics::doesCharacterExist(int characterID)
{
    std::shared_lock lock(dataMutexM);
    return characterOrientationsM.contains(characterID);
}

void RehtiGraphics::moveGameObject(int objectID, glm::vec3 location, float timeInSeconds)
{
    dataMutexM.lock();
    float timeInv = 1.f / timeInSeconds;
    glm::vec3 currentLocation = gameObjectOrientationsM[objectID].position;
    glm::vec3 delta = (location - currentLocation) * timeInv;
    std::function<void(float)> callback = [objectID, delta, this](float dt)
    {
        std::unique_lock lock(dataMutexM);
        gameObjectOrientationsM[objectID].position += delta * dt;
        // smooth bounding box interpolation?
        moveBoundingBox(objectID, ObjectType::GAMEOBJECT, gameObjectOrientationsM[objectID].position);
        glm::mat4 currentMatrix = gameObjectOrientationsM[objectID].getTransformationMatrix();
        pObjectManagerM->updateObjectDescriptor(objectID, &currentMatrix, currentFrameM);
    };
    dataMutexM.unlock();
    timersM.addTimerCallback(objectID, timeInSeconds, callback);
}

void RehtiGraphics::rotateGameObject(int objectID, float radians, float timeInSeconds)
{
    float timeInv = 1.f / timeInSeconds;
    timersM.finishCallback(objectID);
    glm::quat currentRotation = gameObjectOrientationsM[objectID].rotation;
    glm::quat targetRotation = glm::rotate(currentRotation, radians, POSITIVE_Y_AXIS);
    std::function<void(float)> callback = [objectID, currentRotation, targetRotation, timeInv, this](float dt)
    {
        gameObjectOrientationsM[objectID].rotation = glm::slerp(currentRotation, targetRotation, dt * timeInv);
        glm::mat4 currentMatrix = gameObjectOrientationsM[objectID].getTransformationMatrix();
        pObjectManagerM->updateObjectDescriptor(objectID, &currentMatrix, currentFrameM);
    };
    timersM.addTimerCallback(objectID, timeInSeconds, callback);
}

void RehtiGraphics::forceGameObjectMove(int objectID, glm::vec3 location)
{
    // Force quit any ongoing timer events
    timersM.forceQuitCallback(objectID);
    moveBoundingBox(objectID, ObjectType::GAMEOBJECT, location);
    gameObjectOrientationsM[objectID].position = location;
    glm::mat4 locationTransform = glm::translate(glm::mat4(1.f), location);
    // Assume changes are made after draw call. hence why currentFrameM is used
    pObjectManagerM->updateObjectDescriptor(objectID, &locationTransform, currentFrameM);
}

void RehtiGraphics::movePlayer(int playerID, glm::vec3 location, float timeInSeconds)
{
    float timeInv = 1.f / timeInSeconds;
    bool succ = timersM.finishCallback(playerID);

    dataMutexM.lock();
    glm::vec3 currentLocation = characterOrientationsM[playerID].characterOrientation.position;
    glm::vec3 diff = (location - currentLocation);
    if (diff.length() < 0.0001f)
    {
        dataMutexM.unlock();
        return;
    }

    glm::vec3 delta = diff * timeInv;
    glm::vec3 dir = -glm::normalize(diff);
    characterOrientationsM[playerID].characterOrientation.rotation = glm::quatLookAt(dir, POSITIVE_Y_AXIS);
    characterOrientationsM[playerID].animationData.currentAnimation = AnimationType::WALK;
    // characterOrientationsM[playerID].animationData.currentTicks = 0;
    std::function<void(float)> callback = [playerID, delta, dir, this](float dt)
    {
        std::unique_lock lock(dataMutexM);
        characterOrientationsM[playerID].characterOrientation.position += delta * dt;
        moveBoundingBox(playerID, ObjectType::CHARACTER, characterOrientationsM[playerID].characterOrientation.position);
        characterOrientationsM[playerID].advanceAnimation(dt);
        glm::mat4 currentMatrix = characterOrientationsM[playerID].characterOrientation.getTransformationMatrix();
        pObjectManagerM->updateCharacterDescriptor(playerID, &currentMatrix, characterOrientationsM[playerID].boneTransformations.data(), currentFrameM);
        cameraM.setTargetAndCamera(characterOrientationsM[playerID].characterOrientation.position);
    };
    dataMutexM.unlock();
    timersM.addTimerCallback(playerID, timeInSeconds, callback);
}

void RehtiGraphics::playAnimation(int characterID, AnimationConfig cfg)
{
    // reset all animations currently playing
    timersM.finishCallback(characterID);
    dataMutexM.lock();
    characterOrientationsM[characterID].animationData.currentAnimation = cfg.animType;
    characterOrientationsM[characterID].animationData.currentTicks = 0;
    glm::vec3 direction = glm::normalize(cfg.animationDirection);
    characterOrientationsM[characterID].characterOrientation.rotation = glm::quatLookAt(direction, POSITIVE_Y_AXIS);
    //  add new callback to play the animation
    float factor = 1.0f;
    if (cfg.looping || cfg.animType == AnimationType::IDLE)
    {
        factor = 0.f;
        cfg.duration = characterOrientationsM[characterID].animationData.animations[getAnimIndex(cfg.animType)].duration;
    }
    std::function<void(float)> callback = [characterID, cfg, this](float dt)
    {
        characterOrientationsM[characterID].advanceAnimation(dt);
        glm::mat4 currentMatrix = characterOrientationsM[characterID].characterOrientation.getTransformationMatrix();
        pObjectManagerM->updateCharacterDescriptor(characterID, &currentMatrix, characterOrientationsM[characterID].boneTransformations.data(), currentFrameM);
    };
    dataMutexM.unlock();
    timersM.addTimerCallback(characterID, cfg.duration, callback, factor);
}

void RehtiGraphics::forcePlayerMove(int playerID, glm::vec3 location)
{
    std::unique_lock lock(dataMutexM);
    if (!boundingBoxesM[ObjectType::CHARACTER].contains(playerID))
        return;
    glm::mat4 locationTransform = glm::translate(glm::mat4(1.f), location);
    characterOrientationsM[playerID].characterOrientation.position = location;
    // Assume changes are made after draw call. hence why currentFrameM is used
    pObjectManagerM->updateCharacterDescriptor(playerID, &locationTransform, characterOrientationsM[playerID].boneTransformations.data(), currentFrameM);
    // move the bounding box as well
    boundingBoxesM[ObjectType::CHARACTER][playerID].min = location + CHARACTER_MIN;
    boundingBoxesM[ObjectType::CHARACTER][playerID].max = location + CHARACTER_MAX;

    cameraM.setTargetAndCamera(location);
}

void RehtiGraphics::moveCharacter(int characterID, glm::vec3 location, float timeInSeconds)
{
    float timeInv = 1.f / timeInSeconds;
    bool succ = timersM.finishCallback(characterID);

    dataMutexM.lock();
    CharacterData& charData = characterOrientationsM[characterID];
    glm::vec3 currentLocation = characterOrientationsM[characterID].characterOrientation.position;
    glm::vec3 diff = (location - currentLocation);
    glm::vec3 delta = diff * timeInv;
    glm::vec3 dir = glm::normalize(diff);
    characterOrientationsM[characterID].characterOrientation.rotation = glm::quatLookAt(dir, POSITIVE_Y_AXIS);
    std::function<void(float)> callback = [characterID, delta, dir, this](float dt)
    {
        std::unique_lock lock(dataMutexM);
        characterOrientationsM[characterID].characterOrientation.position += delta * dt;
        moveBoundingBox(characterID, ObjectType::CHARACTER, characterOrientationsM[characterID].characterOrientation.position);
        characterOrientationsM[characterID].advanceAnimation(dt);
        glm::mat4 currentMatrix = characterOrientationsM[characterID].characterOrientation.getTransformationMatrix();
        pObjectManagerM->updateCharacterDescriptor(characterID, &currentMatrix, characterOrientationsM[characterID].boneTransformations.data(), currentFrameM);
    };
    dataMutexM.unlock();
    timersM.addTimerCallback(characterID, timeInSeconds, callback);
}

void RehtiGraphics::forceCharacterMove(int characterID, glm::vec3 location)
{
    timersM.forceQuitCallback(characterID);
    moveBoundingBox(characterID, ObjectType::CHARACTER, location);
    characterOrientationsM[characterID].characterOrientation.position = location;
    glm::mat4 locationTransform = glm::translate(glm::mat4(1.f), location);
    // Assume changes are made after draw call. hence why currentFrameM is used
    pObjectManagerM->updateCharacterDescriptor(characterID, &locationTransform, characterOrientationsM[characterID].boneTransformations.data(), currentFrameM);
}

void RehtiGraphics::forceGameObjectRotate(int objectID, float radians)
{
    glm::mat4 rotation = glm::rotate(glm::mat4(1.f), radians, POSITIVE_Y_AXIS);
    pObjectManagerM->updateObjectDescriptor(objectID, &rotation, currentFrameM);
}

bool RehtiGraphics::addArea(std::vector<Vertex> vertices, std::vector<uint32_t> indices,
                            std::array<ImageData, 6> textures)
{
    bool res = pObjectManagerM->addArea(vertices, indices, textures, textureSamplerM);
    if (!res)
        return false;
    return true;
}

void RehtiGraphics::transformTestObject(int id, glm::mat4 transformation)
{
    glm::mat4 trans = transformation;
    pObjectManagerM->updateTestObject(id, &transformation, currentFrameM);
}

void RehtiGraphics::addMapBoundingBox(const MapAABBData& mapAABBData)
{

    std::vector<std::unique_ptr<AABB>> aabbList = createMapAABB(mapAABBData);

    for (int i = 0; i < aabbList.size(); i++)
    {
        boundingBoxesM[ObjectType::AREA][i] = *aabbList[i];
    }
}

Hit RehtiGraphics::traceClick()
{
    Hit hit{};
    hit.objectType = ObjectType::UNDEFINED;
    double x, y;
    glfwGetCursorPos(pWindowM, &x, &y);
    glm::vec3 rayDir = cameraM.getCameraRay(x, y);
    glm::vec3 rayOrigin = cameraM.getLocation();
    glm::vec3 inverseDir = glm::vec3(1.f / rayDir.x, 1.f / rayDir.y, 1.f / rayDir.z);
    float earliest = FLT_MAX;
    for (const auto& boxPair : boundingBoxesM[ObjectType::CHARACTER])
    {
        const auto& box = boxPair.second;
        AABB boxHit{};
        float newT;
        bool res = trace(rayOrigin, inverseDir, &box, boxHit, newT);

        if (res && newT < earliest)
        {
            earliest = newT;
            hit.hitPoint = rayOrigin + rayDir * newT;
            hit.id = boxPair.first;
            hit.objectType = ObjectType::CHARACTER;
        }
    }
    for (const auto& boxPair : boundingBoxesM[ObjectType::GAMEOBJECT])
    {
        const auto& box = boxPair.second;
        AABB boxHit{};
        float newT;

        bool res = trace(rayOrigin, inverseDir, &box, boxHit, newT);

        if (res && newT < earliest)
        {
            earliest = newT;
            hit.hitPoint = rayOrigin + rayDir * newT;
            hit.id = boxPair.first;
            hit.objectType = ObjectType::GAMEOBJECT;
        }
    }
    // Exit if we have a hit already
    if (earliest < FLT_MAX)
        return hit;
    // map gets traced if no other hits were made
    for (const auto& boxPair : boundingBoxesM[ObjectType::AREA])
    {
        const auto& box = boxPair.second;
        AABB boxHit{};
        float newT;
        // debugAABB(box, 0);
        bool res = trace(rayOrigin, inverseDir, &box, boxHit, newT);

        if (res && newT < earliest)
        {
            earliest = newT;
            hit.hitPoint = 0.5f * (boxHit.min + boxHit.max);
            hit.id = boxPair.first;
            hit.objectType = ObjectType::AREA;
        }
    }

    return hit;
}

void RehtiGraphics::setEngineFlags(EngineFlags flags)
{
    engineFlagsM = EngineFlags(engineFlagsM | flags);
}

void RehtiGraphics::initWindow()
{
    // Initialize glfw
    glfwInit();
    // Some arguments
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindowM = glfwCreateWindow(widthM, heightM, "REHTI MMORPG", nullptr, nullptr);

    glfwSetWindowUserPointer(pWindowM, this);
    glfwSetFramebufferSizeCallback(pWindowM, RehtiGraphics::frameBufferResizeCallback);
}

void RehtiGraphics::frameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<RehtiGraphics*>(glfwGetWindowUserPointer(window));
    app->setEngineFlags(EngineFlags::FRAME_BUFFER_RESIZED);
}

void RehtiGraphics::initVulkan()
{
    createInstance();
    setupDebugMessenger();  // Setup debugging
    createSurface();        // Create the surface to draw into. Mostly handled by glfw.
    pickPhysicalDevice();   // Choose the physical device (gpu)
    createLogicalDevice();  // Create the interactable logical device
    createTextureSampler(); // Create a sampler for textures
    createObjectManager();  // Initializes the object management
    createSwapChain();      // Creates the swapchain
    createDepthResources();
    createImageViews(); // Creates the image view (how to access the image)
    createRenderPass();
    createGraphicsPipeline(); // Creates a rendering pipeline
    createFramebuffers();     // Creates the framebuffers
    createCommandPool();
    createCommandBuffers();
    createSynchronization();
    createGui();
}

void RehtiGraphics::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void RehtiGraphics::setupDebugMessenger()
{
    if (!validationLayersEnabledM)
        return; // Validationlayers are not enabled. Go back
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instanceM, &createInfo, nullptr, &debugMessengerM) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void RehtiGraphics::pickPhysicalDevice()
{
    gpuM = VK_NULL_HANDLE;

    uint32_t devcount = 0;
    vkEnumeratePhysicalDevices(instanceM, &devcount, nullptr); // To get how many devices there are

    if (devcount == 0)
    {
        throw std::runtime_error("No suitable devices found.");
    }

    std::vector<VkPhysicalDevice> devices(devcount);
    vkEnumeratePhysicalDevices(instanceM, &devcount, devices.data()); // We have to do this twice, as it is how this function works. If the pointer is not null, it will try to fill out devcount devices.

    // Currently picks the first suitable device
    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device))
        {
            gpuM = device;
            break;
        }
    }

    if (gpuM == VK_NULL_HANDLE)
    {
        throw std::runtime_error("No gpu matches the requirements!");
    }
}

void RehtiGraphics::createLogicalDevice()
{
    QueueFamilyIndices indice = findQueueFamilies(gpuM);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    std::set<uint32_t> uniqueQueueFamilies = {indice.graphicsFamily.value(), indice.presentFamily.value(), indice.transferFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;

        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Check for anisotropy support
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo devCreateInfo{};
    devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    devCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    devCreateInfo.pEnabledFeatures = &deviceFeatures;

    devCreateInfo.enabledExtensionCount = static_cast<uint32_t>(kDeviceExtensionsM.size());
    devCreateInfo.ppEnabledExtensionNames = kDeviceExtensionsM.data();

    // TODO: this is not correct, as there can be layers other than validation layers
    if (validationLayersEnabledM)
    {
        devCreateInfo.enabledLayerCount = static_cast<uint32_t>(kValidationlayersM.size());
        devCreateInfo.ppEnabledLayerNames = kValidationlayersM.data();
    }
    else
    {
        devCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(gpuM, &devCreateInfo, nullptr, &logDeviceM) != VK_SUCCESS)
    {
        throw std::runtime_error("Logical device creation failed");
    }
    // Transfer queue handle is requested later
    vkGetDeviceQueue(logDeviceM, indice.graphicsFamily.value(), 0, &graphicsQueueM);
    vkGetDeviceQueue(logDeviceM, indice.presentFamily.value(), 0, &presentQueueM);
}

void RehtiGraphics::createObjectManager()
{
    QueueFamilyIndices indices = findQueueFamilies(gpuM);

    pObjectManagerM = std::make_shared<GraphicsObjectManager>(instanceM, gpuM, logDeviceM, graphicsQueueM, graphicsQueueMutexM, indices.graphicsFamily.value(), kConcurrentFramesM);

    uint32_t transferQueueFamily;
    VkQueue transferQueue;
    // Add the other queue families to the buffer manager
    if (indices.hasTransferOnlyQueue())
    {
        vkGetDeviceQueue(logDeviceM, indices.transferFamily.value(), 0, &transferQueue);
        transferQueueFamily = indices.transferFamily.value();
        pObjectManagerM->addTransferQueueFamilyAccess(transferQueueFamily, transferQueue);
    }
    // update sun right away
    for (uint32_t frame = 0; frame < kConcurrentFramesM; frame++)
    {
        pObjectManagerM->updateSunDescriptor(&sunM, frame);
    }
}

void RehtiGraphics::createSwapChain()
{
    SwapChainSupportDetails details = querySwapChainSupport(gpuM);

    VkSurfaceFormatKHR format = chooseSwapSurfaceFormat(details.formats);
    VkPresentModeKHR mode = chooseSwapPresentMode(details.presentModes);
    VkExtent2D extent = chooseSwapExtent(details.capabilities);

    uint32_t imageCount = details.capabilities.minImageCount + 1; // for now, could choose smarter

    if (imageCount > details.capabilities.maxImageCount && details.capabilities.maxImageCount > 0) // 0 means "infinite". So > 0 means it has a limit
        imageCount = details.capabilities.maxImageCount;

    // Info time!!
    VkSwapchainCreateInfoKHR swapInfo{};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapInfo.surface = this->surfaceM;
    swapInfo.minImageCount = imageCount;
    swapInfo.imageFormat = format.format;
    swapInfo.imageColorSpace = format.colorSpace;
    swapInfo.imageExtent = extent;
    swapInfo.imageArrayLayers = 1; // Always 1 unless vr
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Onto conditional members
    QueueFamilyIndices indices = findQueueFamilies(gpuM);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapInfo.queueFamilyIndexCount = 2;
        swapInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapInfo.queueFamilyIndexCount = 0;     // optionals
        swapInfo.pQueueFamilyIndices = nullptr; // optionals
    }

    // Some more
    swapInfo.preTransform = details.capabilities.currentTransform; // no transform
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;   // Defines how to use alpha channel with other windows.

    swapInfo.presentMode = mode;
    swapInfo.clipped = VK_TRUE;

    swapInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logDeviceM, &swapInfo, nullptr, &swapChainM) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create a swapchain");
    }

    vkGetSwapchainImagesKHR(logDeviceM, swapChainM, &imageCount, nullptr);
    swapChainImagesM.resize(imageCount);
    vkGetSwapchainImagesKHR(logDeviceM, swapChainM, &imageCount, swapChainImagesM.data());

    swapChainImageFormatM = format.format;
    swapChainExtentM = extent;
}

void RehtiGraphics::recreateSwapChain()
{
    // Minimization handler
    int width = 0, height = 0;
    glfwGetFramebufferSize(pWindowM, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(pWindowM, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(logDeviceM);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFramebuffers();
}

void RehtiGraphics::createImageViews()
{
    swapChainImageViewsM.resize(swapChainImagesM.size());

    for (size_t i = 0; i < swapChainImagesM.size(); i++)
    {
        VkImageViewCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageInfo.image = swapChainImagesM[i];
        imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // how image should be interpreted, for example you might use images as 2d textures.
        imageInfo.format = swapChainImageFormatM;   // how image should be interpreted

        imageInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageInfo.subresourceRange.baseMipLevel = 0;
        imageInfo.subresourceRange.levelCount = 1;
        imageInfo.subresourceRange.baseArrayLayer = 0;
        imageInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logDeviceM, &imageInfo, nullptr, &this->swapChainImageViewsM[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create an image view");
    }
}

void RehtiGraphics::createRenderPass()
{
    VkAttachmentDescription colorattachment{};
    colorattachment.format = swapChainImageFormatM;
    colorattachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorattachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorattachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorattachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorattachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorattachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorattachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachRef{};
    attachRef.attachment = 0;
    attachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = depthFormatM;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachRef{};
    depthAttachRef.attachment = 1;
    depthAttachRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachRef;
    subpass.pDepthStencilAttachment = &depthAttachRef;

    VkSubpassDependency depend{};
    depend.srcSubpass = VK_SUBPASS_EXTERNAL;
    depend.dstSubpass = 0;
    depend.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    depend.srcAccessMask = 0;
    depend.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    depend.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorattachment, depthAttachment};
    VkRenderPassCreateInfo renderpassInfo{};
    renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderpassInfo.pAttachments = attachments.data();
    renderpassInfo.subpassCount = 1;
    renderpassInfo.pSubpasses = &subpass;
    renderpassInfo.dependencyCount = 1;
    renderpassInfo.pDependencies = &depend;

    if (vkCreateRenderPass(logDeviceM, &renderpassInfo, nullptr, &renderPassM) != VK_SUCCESS)
        throw std::runtime_error("failed to create a renderpass");
}

void RehtiGraphics::createGraphicsPipeline()
{
    // Create a pipeline for all the object types
    for (ObjectType objectType : getObjectTypes())
    {

        VkVertexInputBindingDescription bindingDesc = getBindingDescription(objectType);
        std::vector<VkVertexInputAttributeDescription> attributeDescs = getAttributeDescription(objectType);

        VkPipelineVertexInputStateCreateInfo vertInputInfo{};
        vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertInputInfo.vertexBindingDescriptionCount = 1;
        vertInputInfo.pVertexBindingDescriptions = &bindingDesc;
        vertInputInfo.vertexAttributeDescriptionCount = attributeDescs.size();
        vertInputInfo.pVertexAttributeDescriptions = attributeDescs.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        VkViewport viewPort{};
        viewPort.x = 0.f;
        viewPort.y = 0.f;
        viewPort.width = swapChainExtentM.width;
        viewPort.height = swapChainExtentM.height;
        viewPort.minDepth = 0.f;
        viewPort.maxDepth = 1.f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtentM;

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &viewPort;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterInfo{};
        rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterInfo.depthClampEnable = VK_FALSE;
        rasterInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterInfo.lineWidth = 1.0f;
        rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // This is because y is flipped in perspective matrix
        rasterInfo.depthBiasEnable = VK_FALSE;
        rasterInfo.depthBiasConstantFactor = 0.f;
        rasterInfo.depthBiasClamp = 0.f;
        rasterInfo.depthBiasSlopeFactor = 0.f;

        VkPipelineMultisampleStateCreateInfo multInfo{};
        multInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multInfo.sampleShadingEnable = VK_FALSE;
        multInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multInfo.minSampleShading = 1.f; // optional stuff
        multInfo.pSampleMask = nullptr;
        multInfo.alphaToCoverageEnable = VK_FALSE;
        multInfo.alphaToOneEnable = VK_FALSE;

        // Vk depth and stencil testing info here

        // Colorblending
        VkPipelineColorBlendAttachmentState colorBlendState{};
        colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendState.blendEnable = VK_FALSE;
        // Rest is optional. Maybe adding later

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.logicOpEnable = VK_FALSE;
        colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // optional
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &colorBlendState;
        // Constants optional

        // Pipelineinfo for pipelinelayout stuff, uniforms
        VkPipelineLayoutCreateInfo pipelinelayoutInfo{};
        pipelinelayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        std::array<VkDescriptorSetLayout, 2> descLayouts = {pObjectManagerM->getSunLayout(), pObjectManagerM->getLayout(objectType)};
        pipelinelayoutInfo.setLayoutCount = static_cast<uint32_t>(descLayouts.size());
        pipelinelayoutInfo.pSetLayouts = descLayouts.data();

        VkPushConstantRange cameraRange = getCameraRange();
        pipelinelayoutInfo.pushConstantRangeCount = 1;
        pipelinelayoutInfo.pPushConstantRanges = &cameraRange;

        // Create pipelinelayout for the given object
        if (vkCreatePipelineLayout(logDeviceM, &pipelinelayoutInfo, nullptr, &pipelineLayoutsM[objectType]))
            throw std::runtime_error("Pipeline layout creation failed");

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilInfo.depthTestEnable = VK_TRUE;
        depthStencilInfo.depthWriteEnable = VK_TRUE;
        depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.minDepthBounds = 0.f; // Optional
        depthStencilInfo.maxDepthBounds = 1.f;
        depthStencilInfo.stencilTestEnable = VK_FALSE; // no stencil test
        depthStencilInfo.front = {};                   // Optional
        depthStencilInfo.back = {};

        VkPipelineShaderStageCreateInfo vertInfo = ShaderManager::createVertexShaderInfo(logDeviceM, objectType);
        VkPipelineShaderStageCreateInfo fragInfo = ShaderManager::createFragmentShaderInfo(logDeviceM, objectType);

        VkPipelineShaderStageCreateInfo stages[2] = {vertInfo, fragInfo};

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterInfo;
        pipelineInfo.pMultisampleState = &multInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.layout = pipelineLayoutsM[objectType];
        pipelineInfo.renderPass = renderPassM;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        // Create the actual pipeline
        if (vkCreateGraphicsPipelines(logDeviceM, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelinesM[objectType]) != VK_SUCCESS)
            throw std::runtime_error("Pipeline layout creation failed");

        // Cleanup
        ShaderManager::destroyShaderModules(logDeviceM);
    }
}

void RehtiGraphics::createFramebuffers()
{
    swapChainFramebuffersM.resize(swapChainImageViewsM.size());

    for (size_t i = 0u; i < swapChainImageViewsM.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {swapChainImageViewsM[i], depthImageViewM};

        VkFramebufferCreateInfo frameInfo{};
        frameInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameInfo.renderPass = renderPassM;
        frameInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        frameInfo.pAttachments = attachments.data();
        frameInfo.width = swapChainExtentM.width;
        frameInfo.height = swapChainExtentM.height;
        frameInfo.layers = 1;
        if (vkCreateFramebuffer(logDeviceM, &frameInfo, nullptr, &swapChainFramebuffersM[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create a framebuffer");
    }
}

void RehtiGraphics::createCommandPool()
{
    auto queuefamilyIndices = findQueueFamilies(gpuM);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queuefamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(logDeviceM, &poolInfo, nullptr, &commandPoolM) != VK_SUCCESS)
        throw std::runtime_error("Failed to create a command pool");
}

void RehtiGraphics::createCommandBuffers()
{
    commandBuffersM.resize(swapChainFramebuffersM.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPoolM;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffersM.size());

    if (vkAllocateCommandBuffers(logDeviceM, &allocInfo, commandBuffersM.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers");
}

void RehtiGraphics::recordCommandBuffer(VkCommandBuffer cmdBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo cmdInfo{};
    cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdInfo.flags = 0; // optional

    // Let's begin recording
    if (vkBeginCommandBuffer(cmdBuffer, &cmdInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin command buffer recording");

    VkRenderPassBeginInfo renderInfo{};
    renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderInfo.renderPass = renderPassM;
    renderInfo.framebuffer = swapChainFramebuffersM[imageIndex];
    renderInfo.renderArea.offset = {0, 0};
    renderInfo.renderArea.extent = swapChainExtentM;

    std::array<VkClearValue, 2> clearValues{};

    clearValues[0].color = {{0.f, 0.f, 0.f, 1.f}};
    clearValues[1].depthStencil = {1.f, 0};

    renderInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmdBuffer, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);

    glm::mat4 renderMat = cameraM.getWorldToScreenMatrix();

    for (ObjectType objectType : getObjectTypes())
    {
        // Bind correct pipeline for object type
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelinesM[objectType]);
        // Push constant is the same for all pipelines
        vkCmdPushConstants(cmdBuffer, pipelineLayoutsM[objectType], VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &renderMat);
        VkDescriptorSet sunDescriptor = pObjectManagerM->getSunDescriptorSet(currentFrameM);
        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayoutsM[objectType], 0, 1, &sunDescriptor, 0, nullptr);
        for (DrawableObject obj : pObjectManagerM->getDrawableObjects(objectType, currentFrameM))
        {
            // Fetch object data
            VkBuffer vertexBuffer = obj.vertexBuffer;
            VkBuffer indexBuffer = obj.indexBuffer;
            VkDescriptorSet descSet = obj.descriptorSet;

            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &vertexBuffer, offsets);
            vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayoutsM[objectType], 1, 1, &descSet, 0, nullptr);

            vkCmdDrawIndexed(cmdBuffer, obj.indexCount, 1, 0, 0, 0);
        }
    }

    // record gui data
    pGuiM->recordGuiData(cmdBuffer);

    vkCmdEndRenderPass(cmdBuffer);

    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer");
}

void RehtiGraphics::createSynchronization()
{
    imagesReadyM.resize(kConcurrentFramesM);
    rendersFinishedM.resize(kConcurrentFramesM);
    frameFencesM.resize(kConcurrentFramesM);

    VkSemaphoreCreateInfo semaInfo{};
    semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < kConcurrentFramesM; i++)
    {
        if (vkCreateSemaphore(logDeviceM, &semaInfo, nullptr, &imagesReadyM[i]) != VK_SUCCESS || vkCreateSemaphore(logDeviceM, &semaInfo, nullptr, &rendersFinishedM[i]) != VK_SUCCESS || vkCreateFence(logDeviceM, &fenceInfo, nullptr, &frameFencesM[i]) != VK_SUCCESS)
            throw std::runtime_error("Creating synchros failed");
    }
}

void RehtiGraphics::drawFrame()
{
    pGuiM->startRender();
    // wait
    vkWaitForFences(logDeviceM, 1, &frameFencesM[currentFrameM], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult res = vkAcquireNextImageKHR(logDeviceM, swapChainM, UINT64_MAX, imagesReadyM[currentFrameM], VK_NULL_HANDLE, &imageIndex);

    if (res == VK_ERROR_OUT_OF_DATE_KHR ||
        res == VK_SUBOPTIMAL_KHR ||
        engineFlagsM & EngineFlags::FRAME_BUFFER_RESIZED)
    {
        engineFlagsM = EngineFlags(engineFlagsM & ~EngineFlags::FRAME_BUFFER_RESIZED);
        recreateSwapChain();
        return;
    }
    else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    vkResetFences(logDeviceM, 1, &frameFencesM[currentFrameM]);

    vkResetCommandBuffer(commandBuffersM[currentFrameM], 0);
    recordCommandBuffer(commandBuffersM[currentFrameM], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imagesReadyM[currentFrameM]};
    VkSemaphore signalSemaphores[] = {rendersFinishedM[currentFrameM]};

    VkSwapchainKHR swapChains[] = {swapChainM};

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffersM[currentFrameM];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    graphicsQueueMutexM.lock();
    if (vkQueueSubmit(graphicsQueueM, 1, &submitInfo, frameFencesM[currentFrameM]) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer");
    graphicsQueueMutexM.unlock();
    VkPresentInfoKHR presInfo{};
    presInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presInfo.waitSemaphoreCount = 1;
    presInfo.pWaitSemaphores = signalSemaphores;

    presInfo.swapchainCount = 1;
    presInfo.pSwapchains = swapChains;
    presInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueueM, &presInfo);

    currentFrameM = getNextFrame();
}

void RehtiGraphics::mainLoop()
{
    double invMicro = 1.0 / 1e6;

    statsM.ftPerSec = 0;
    statsM.frameTime = 0;
    float frameCount = 0.f;
    auto applicationStart = std::chrono::high_resolution_clock::now();
    CameraData camData{};
    camData.position = cameraM.getLocation();
    camData.direction = cameraM.getForward();
    while (!glfwWindowShouldClose(pWindowM))
    {
        auto start = std::chrono::high_resolution_clock::now();
        glfwPollEvents();
        pGuiM->newFrame();
        drawFrame();
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto mus = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        statsM.frameTime = static_cast<uint64_t>(mus);
        statsM.ftPerSec = mus * invMicro;
        camData.position = cameraM.getLocation();
        camData.direction = cameraM.getForward();
        pObjectManagerM->updateCameraDescriptor(&camData, currentFrameM);
        timersM.elapseTime(static_cast<float>(statsM.ftPerSec));
    }

    vkDeviceWaitIdle(logDeviceM);
}

void RehtiGraphics::cleanupSwapChain()
{

    for (auto framebuffer : swapChainFramebuffersM)
        vkDestroyFramebuffer(logDeviceM, framebuffer, nullptr);

    for (auto imageView : swapChainImageViewsM)
        vkDestroyImageView(logDeviceM, imageView, nullptr);

    vkDestroySwapchainKHR(logDeviceM, swapChainM, nullptr); // destroy swapchain
}

void RehtiGraphics::cleanup()
{

    for (size_t i = 0; i < kConcurrentFramesM; i++)
    {
        vkDestroySemaphore(logDeviceM, rendersFinishedM[i], nullptr);
        vkDestroySemaphore(logDeviceM, imagesReadyM[i], nullptr);
        vkDestroyFence(logDeviceM, frameFencesM[i], nullptr);
    }

    pObjectManagerM->destroyImage(depthImageM);
    vkDestroyImageView(logDeviceM, depthImageViewM, nullptr);
    // Delete the managers and builders
    pGuiM.reset();
    pObjectManagerM.reset();
    // Destroy sampler
    vkDestroySampler(logDeviceM, textureSamplerM, nullptr);

    vkDestroyCommandPool(logDeviceM, commandPoolM, nullptr);

    cleanupSwapChain();

    for (ObjectType objType : getObjectTypes())
    {
        vkDestroyPipeline(logDeviceM, pipelinesM[objType], nullptr);
        vkDestroyPipelineLayout(logDeviceM, pipelineLayoutsM[objType], nullptr);
    }
    vkDestroyRenderPass(logDeviceM, renderPassM, nullptr);

    vkDestroyDevice(logDeviceM, nullptr); // queues are destroyed when logicaldevice is destroyed

    if (validationLayersEnabledM)
        DestroyDebugUtilsMessengerEXT(instanceM, debugMessengerM, nullptr);

    // Destroy the surface
    vkDestroySurfaceKHR(instanceM, surfaceM, nullptr);

    // Take out the instance
    vkDestroyInstance(instanceM, nullptr);

    // Then window
    glfwDestroyWindow(pWindowM);

    glfwTerminate();
}

// Helpers or smaller functions

void RehtiGraphics::createInstance()
{

    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        std::cout << "WARNING: Validation layers requested, but not available!\n Continuing without validation." << std::endl;
    }

    // Create info
    VkApplicationInfo info{};
    // And fill in the form, as always
    info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    info.pApplicationName = "RehtiMMORPG";
    info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    info.pEngineName = "RehtiEngine";
    info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    info.apiVersion = VK_API_VERSION_1_3; // Vulkan 1.3

    // Create info:
    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &info;
    // Next up, extensions
    auto extensions = this->getRequiredExtensions();

    instanceInfo.enabledExtensionCount = extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (validationLayersEnabledM)
    {
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(kValidationlayersM.size()); // Names are null for now
        instanceInfo.ppEnabledLayerNames = kValidationlayersM.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&instanceInfo, nullptr, &instanceM) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create an instance.");
    }
}

void RehtiGraphics::createSurface()
{
    if (glfwCreateWindowSurface(instanceM, pWindowM, nullptr, &surfaceM) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }
}

void RehtiGraphics::createTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    // magnification and minification filters
    samplerInfo.magFilter = VK_FILTER_LINEAR; // Linear filter. No minecraft filtering.
    samplerInfo.minFilter = VK_FILTER_LINEAR; // Linear as well.
    // Address mode: What happens when we go over the texture coordinates
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // Repeat the texture
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; // Repeat the texture
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // Repeat the texture
    // Anisotropic filtering
    samplerInfo.anisotropyEnable = VK_TRUE; // Enable anisotropic filtering
    samplerInfo.maxAnisotropy = anisotropyM;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // Border color
    samplerInfo.unnormalizedCoordinates = VK_FALSE;             // use 0.0 to 1.0 coordinates
    samplerInfo.compareEnable = VK_FALSE;                       // No custom comparison
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.f;
    samplerInfo.mipLodBias = 0.f;
    samplerInfo.minLod = 0.f;
    samplerInfo.maxLod = 0.f;

    if (vkCreateSampler(logDeviceM, &samplerInfo, nullptr, &textureSamplerM) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create texture sampler");
    }
}

void RehtiGraphics::createDepthResources()
{
    depthFormatM = findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    depthImageM = pObjectManagerM->createDepthImage(swapChainExtentM.width, swapChainExtentM.height, depthFormatM);
    depthImageViewM = pObjectManagerM->createImageView(depthImageM.image, depthFormatM, VK_IMAGE_ASPECT_DEPTH_BIT);
    pObjectManagerM->transitionDepthImageLayout(depthImageM, depthFormatM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void RehtiGraphics::createGui()
{
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pPoolSizes = STANDARD_POOL_SIZES.data();
    poolInfo.poolSizeCount = static_cast<uint32_t>(STANDARD_POOL_SIZES.size());
    poolInfo.maxSets = 100;
    poolInfo.flags = 0;
    VkDescriptorPool guiPool;
    if (vkCreateDescriptorPool(logDeviceM, &poolInfo, nullptr, &guiPool))
    {
        throw std::runtime_error("Failed to create a descriptor pool for gui..");
    }

    pGuiM = std::make_shared<RehtiGui>(instanceM, gpuM, logDeviceM, pWindowM, graphicsQueueM, guiPool,
                                       static_cast<uint32_t>(swapChainImagesM.size()), renderPassM, pObjectManagerM);

    vkResetCommandPool(logDeviceM, commandPoolM, 0);
    VkCommandBufferBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffersM[0], &info);
    pGuiM->uploadFonts(commandBuffersM[0]);
    vkEndCommandBuffer(commandBuffersM[0]);
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffersM[0];
    graphicsQueueMutexM.lock();
    vkQueueSubmit(graphicsQueueM, 1, &submitInfo, nullptr);
    vkQueueWaitIdle(graphicsQueueM);
    graphicsQueueMutexM.unlock();
    pGuiM->uploadEnded();
}

bool RehtiGraphics::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> properties(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, properties.data());

    std::set<std::string> requiredExtensions(kDeviceExtensionsM.begin(), kDeviceExtensionsM.end());

    for (const auto& property : properties)
    {
        requiredExtensions.erase(property.extensionName); // tick off the required extensions
    }

    return requiredExtensions.empty();
}

bool RehtiGraphics::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : kValidationlayersM)
    {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                validationLayersEnabledM = true;
                break;
            }
        }

        if (!layerFound) // if one of the layers is not found, return false
        {
            validationLayersEnabledM = false;
            return false;
        }
    }

    return true;
}

bool RehtiGraphics::isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props; // properties
    VkPhysicalDeviceFeatures feats;   // and features of the gpu
    vkGetPhysicalDeviceProperties(device, &props);
    vkGetPhysicalDeviceFeatures(device, &feats);

    QueueFamilyIndices indice = findQueueFamilies(device);

    bool requiredExtensionSupport = checkDeviceExtensionSupport(device);

    bool swapChainOk = false;
    if (requiredExtensionSupport) // swap chain is an extension
    {
        SwapChainSupportDetails supportDetails = querySwapChainSupport(device);
        swapChainOk = !supportDetails.formats.empty() && !supportDetails.presentModes.empty();
    }
    if (feats.samplerAnisotropy)
    { // some logic to determine the anisotropy level.
        float proposedSamples = ceilf(props.limits.maxSamplerAnisotropy * 0.5f);
        anisotropyM = (proposedSamples > 1.0f) ? proposedSamples : 2.0f;
    }

    return indice.isComplete() &&
           requiredExtensionSupport &&
           swapChainOk && feats.samplerAnisotropy; // Suitable, if it is has a graphics queueFamily, extensions, swapchain support, and anisotropic filtering
}

bool RehtiGraphics::bbHit(const glm::vec3 min, const glm::vec3 max, const glm::vec3 rayOrig, const glm::vec3 dirInv, float& t)
{
    float original = t;
    float tx1 = (min.x - rayOrig.x) * dirInv.x;
    float tx2 = (max.x - rayOrig.x) * dirInv.x;

    float tmin = std::min(tx1, tx2);
    float tmax = std::max(tx1, tx2);

    float ty1 = (min.y - rayOrig.y) * dirInv.y;
    float ty2 = (max.y - rayOrig.y) * dirInv.y;

    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));

    float tz1 = (min.z - rayOrig.z) * dirInv.z;
    float tz2 = (max.z - rayOrig.z) * dirInv.z;

    tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));

    t = std::max(0.0f, tmin);
    if (tmax < tmin)
        t = original;

    return tmin <= tmax;
}

bool RehtiGraphics::trace(const glm::vec3 orig, const glm::vec3 dirInv, const AABB* pBoxNode, AABB& boxHit, float& t)
{
    float hitT = FLT_MAX;
    if (bbHit(pBoxNode->min, pBoxNode->max, orig, dirInv, hitT))
    {
        if (pBoxNode->isLeaf())
        {
            boxHit.max = pBoxNode->max;
            boxHit.min = pBoxNode->min;
            t = hitT;
            return true;
        }
        else
        {
            AABB box1{};
            AABB box2{};
            float t1 = FLT_MAX;
            float t2 = FLT_MAX;
            bool hit1 = trace(orig, dirInv, pBoxNode->pLeft.get(), box1, t1);
            bool hit2 = trace(orig, dirInv, pBoxNode->pRight.get(), box2, t2);

            if (!hit1 && !hit2)
            {
                t = FLT_MAX;
                return false;
            }

            if (t1 < t2)
            {
                boxHit.max = box1.max;
                boxHit.min = box1.min;
                t = t1;
            }
            else
            {
                boxHit.max = box2.max;
                boxHit.min = box2.min;
                t = t2;
            }
            return true;
        }
    }
    return false;
}

int RehtiGraphics::rateDevice(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props; // properties
    VkPhysicalDeviceFeatures feats;   // and features of the gpu
    vkGetPhysicalDeviceProperties(device, &props);
    vkGetPhysicalDeviceFeatures(device, &feats);

    return 1;
}

QueueFamilyIndices RehtiGraphics::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indexes;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (auto i = 0u; i < queueFamilies.size(); i++)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surfaceM, &presentSupport);

        // Find a transfer queue that is not a graphics queue (Otherwise it will be the same.)
        if ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
            !(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            indexes.transferFamily = i;
        }
        else if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport)
        {
            indexes.graphicsFamily = i;
            indexes.presentFamily = i; // same family
        }

        if (indexes.isComplete() && indexes.hasTransferOnlyQueue()) // break if we have found all the queues we need
            break;
    }

    return indexes;
}

SwapChainSupportDetails RehtiGraphics::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surfaceM, &details.capabilities); // A bit unorthodox for vulkan but I guess it will do

    // now to the formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceM, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount); // Usually we just created the vector, but now that we have a struct we just resize it
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surfaceM, &formatCount, details.formats.data());
    }

    // And the same for presentModes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceM, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount); // usually we just created the vector, but now that we have a struct we just resize it
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surfaceM, &presentModeCount, details.presentModes.data());
    }

    return details;
}

std::vector<const char*> RehtiGraphics::getRequiredExtensions()
{
    uint32_t extCount;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&extCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extCount);

    if (validationLayersEnabledM)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkSurfaceFormatKHR RehtiGraphics::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats)
{
    for (const auto& format : availableFormats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }

    return availableFormats[0];
}

VkPresentModeKHR RehtiGraphics::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availableModes)
{
    for (const auto& presentmode : availableModes)
    {
        if (presentmode == VK_PRESENT_MODE_MAILBOX_KHR)
            return presentmode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D RehtiGraphics::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    { // not the special value so the current window size will do?
        return capabilities.currentExtent;
    }
    else
    { // ohno special value
        int width, height;
        glfwGetFramebufferSize(pWindowM, &width, &height);

        VkExtent2D actual = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actual.width = std::clamp(actual.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual.height = std::clamp(actual.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actual;
    }
}

VkPushConstantRange RehtiGraphics::getCameraRange()
{
    VkPushConstantRange cameraRange;
    cameraRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Camera information is only relevant in vertex shader for now.
    cameraRange.offset = 0;
    cameraRange.size = cameraM.getUboSize(); // one matrix for now
    return cameraRange;
}

VkFormat RehtiGraphics::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpuM, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }
    throw std::runtime_error("Failed to find supported format");
}

size_t RehtiGraphics::getNextFrame()
{
    return (currentFrameM + 1) % kConcurrentFramesM;
}

void RehtiGraphics::moveBoundingBox(int objectID, ObjectType objType, glm::vec3 location)
{
    if (!boundingBoxesM[objType].contains(objectID))
        return;
    // move the bounding box as well
    // TODO change the offsets to be queried from somewhere.
    boundingBoxesM[objType][objectID].min = location + GAMEOBJECT_MIN;
    boundingBoxesM[objType][objectID].max = location + GAMEOBJECT_MAX;
}

void RehtiGraphics::debugMatrix(glm::mat4 matrix)
{
    std::cout << "Matrix values are " << std::endl;
    for (int j = 0; j < 4; j++)
    { // column major
        std::cout << matrix[0][j] << " " << matrix[1][j] << " " << matrix[2][j] << " " << matrix[3][j] << std::endl;
    }
}

void RehtiGraphics::debugAABB(const AABB& aabb, int level)
{
    for (int i = 0; i < level; i++)
    { // indent
        std::cout << "  ";
    }
    if (aabb.isLeaf())
    {
        std::cout << "Leaf AABB values are \n"
                  << "Level: " << level << "\n"
                  << "Min: " << aabb.min.x << " " << aabb.min.y << " " << aabb.min.z << "\n"
                  << "Max: " << aabb.max.x << " " << aabb.max.y << " " << aabb.max.z << std::endl;
        return;
    }
    else
    {
        std::cout << "AABB values are \n"
                  << "Level: " << level << "\n"
                  << "Min: " << aabb.min.x << " " << aabb.min.y << " " << aabb.min.z << "\n"
                  << "Max: " << aabb.max.x << " " << aabb.max.y << " " << aabb.max.z << std::endl;
    }
    debugAABB(*aabb.pLeft.get(), level + 1);
    debugAABB(*aabb.pRight.get(), level + 1);
}

void RehtiGraphics::fillAABB(std::vector<Vertex> vertices, AABB& box)
{
    glm::vec3 min = glm::vec3(FLT_MAX);
    glm::vec3 max = glm::vec3(-FLT_MAX);

    for (const Vertex& vert : vertices)
    {
        min = glm::min(min, vert.pos);
        max = glm::max(max, vert.pos);
    }
    box.max = max;
    box.min = min;
}

VKAPI_ATTR VkBool32 VKAPI_CALL RehtiGraphics::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult RehtiGraphics::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void RehtiGraphics::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void RehtiGraphics::addMouseClickCallback(std::function<void(const Hit&)> callback)
{
    this->mouseClickCallbackM = callback;
    glfwSetWindowUserPointer(this->pWindowM, this);
    glfwSetMouseButtonCallback(this->pWindowM, [](GLFWwindow* window, int button, int action, int mods)
                               {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(button, action);
        if (!io.WantCaptureMouse)
        {
            if ((button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
            {
                RehtiGraphics* pGraphics = reinterpret_cast<RehtiGraphics*>(glfwGetWindowUserPointer(window));
                Hit hit = pGraphics->traceClick();
                hit.button = button;
                pGraphics->mouseClickCallbackM(hit);
            }
            else if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS)
            {
                Camera::canMove = true;
            }
            else if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE)
            {
                Camera::canMove = false;
            }
        } });
}

std::shared_ptr<RehtiGui> RehtiGraphics::getGui()
{
    return pGuiM;
}
