#include "base.hpp"
#include "ShaderManager.hpp"

#include "GLFW/glfw3.h"

void RehtiGraphics::testRun() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}


void RehtiGraphics::initWindow() {
    // Initialize glfw
    glfwInit();
    // Some arguments
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    this->window = glfwCreateWindow(this->WIDTH, this->HEIGHT, "Vulkan", nullptr, nullptr);

}

void RehtiGraphics::initVulkan() {
    createInstance();
    setupDebugMessenger(); // Setup debugging
    createSurface();       // Create the surface to draw into. Mostly handled by glfw.
    pickPhysicalDevice(); // Choose the physical device (gpu)
    createLogicalDevice(); // Create the interactable logical device
    createSwapChain();     // Creates the swapchain
    createImageViews();    // Creates the image view (how to access the image)
    createRenderPass();
    createGraphicsPipeline(); // Creates a rendering pipeline (immutable stuff in vulkan)
    createFramebuffers();   // Creates the framebuffers
    createCommandPool();
    createCommandBuffers();
    createSynchronization();
}

void RehtiGraphics::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void RehtiGraphics::setupDebugMessenger()
{
    if (!enableValidationLayers) return; // Validationlayers are not enabled. Go back
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }

}

void RehtiGraphics::pickPhysicalDevice()
{
    this->gpu = VK_NULL_HANDLE;

    uint32_t devcount = 0;
    vkEnumeratePhysicalDevices(this->instance, &devcount, nullptr); //To get how many devices there are

    if (devcount == 0) 
    {
        throw std::runtime_error("No suitable devices found.");
    }

    std::vector<VkPhysicalDevice> devices(devcount);
    vkEnumeratePhysicalDevices(this->instance, &devcount, devices.data()); //We have to do this twice, as it is how this function works. If the pointer is not null, it will try to fill out devcount devices.
    
    // Currently picks the first suitable device
    for (const auto& device : devices) 
    {
        if (isDeviceSuitable(device)) 
        {
            this->gpu = device;
            break;
        }

    }

    if (this->gpu == VK_NULL_HANDLE) 
    {
        throw std::runtime_error("No gpu matches the requirements!");
    }

}

void RehtiGraphics::createLogicalDevice()
{
    QueueFamilyIndices indice = findQueueFamilies(this->gpu);

    //
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    std::set<uint32_t> uniqueQueueFamilies = { indice.graphicsFamily.value(), indice.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1; //1 for now

        queueCreateInfo.pQueuePriorities = &queuePriority; //hmm they all have a priority of 1 currently?
        queueCreateInfos.push_back(queueCreateInfo);
    }

    //Definition enough for now
    VkPhysicalDeviceFeatures deviceFeatures{};

    //logical device time
    VkDeviceCreateInfo devCreateInfo{};
    devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    devCreateInfo.pQueueCreateInfos = queueCreateInfos.data(); //Info inside info :DDD

    devCreateInfo.pEnabledFeatures = &deviceFeatures;

    devCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    devCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        devCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationlayers.size());
        devCreateInfo.ppEnabledLayerNames = validationlayers.data();
    }
    else {
        devCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(this->gpu, &devCreateInfo, nullptr, &this->logDevice) != VK_SUCCESS) {
        throw std::runtime_error("Logical device creation failed");
    }

    vkGetDeviceQueue(this->logDevice, indice.graphicsFamily.value(), 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->logDevice, indice.presentFamily.value(), 0, &this->presentQueue);

}

void RehtiGraphics::createSwapChain()
{
    SwapChainSupportDetails details = querySwapChainSupport(this->gpu);

    VkSurfaceFormatKHR format = chooseSwapSurfaceFormat(details.formats);
    VkPresentModeKHR mode = chooseSwapPresentMode(details.presentModes);
    VkExtent2D extent = chooseSwapExtent(details.capabilities);

    uint32_t imageCount = details.capabilities.minImageCount + 1; //for now, could choose smarter

    if (imageCount > details.capabilities.maxImageCount && details.capabilities.maxImageCount > 0) //0 means "infinite". So > 0 means it has a limit
        imageCount = details.capabilities.maxImageCount;

    //Info time!!
    VkSwapchainCreateInfoKHR swapInfo{};
    swapInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapInfo.surface = this->surface;
    swapInfo.minImageCount = imageCount;
    swapInfo.imageFormat = format.format;
    swapInfo.imageColorSpace = format.colorSpace;
    swapInfo.imageExtent = extent;
    swapInfo.imageArrayLayers = 1; //Always 1 unless vr
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    //Onto conditional members
    QueueFamilyIndices indices = findQueueFamilies(this->gpu);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        swapInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapInfo.queueFamilyIndexCount = 2;
        swapInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapInfo.queueFamilyIndexCount = 0; //optionals
        swapInfo.pQueueFamilyIndices = nullptr; //optionals
    }

    //Some more
    swapInfo.preTransform = details.capabilities.currentTransform; //no transform
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  //Defines how to use alpha channel with other windows.

    swapInfo.presentMode = mode;
    swapInfo.clipped = VK_TRUE;

    swapInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(this->logDevice, &swapInfo, nullptr, &this->swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create a swapchain");
    }

    //Get the images??
    vkGetSwapchainImagesKHR(this->logDevice, this->swapChain, &imageCount, nullptr);
    this->swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(this->logDevice, this->swapChain, &imageCount, swapChainImages.data());

    this->swapChainImageFormat = format.format;
    this->swapChainExtent = extent;

}

void RehtiGraphics::createImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageInfo.image = swapChainImages[i];
        imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; //how image should be interpreted, for example you might use images as 2d textures.
        imageInfo.format = swapChainImageFormat; //how image should be interpreted

        imageInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageInfo.subresourceRange.baseMipLevel = 0;
        imageInfo.subresourceRange.levelCount = 1;
        imageInfo.subresourceRange.baseArrayLayer = 0;
        imageInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(this->logDevice, &imageInfo, nullptr, &this->swapChainImageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to create an image view");

    }


}

void RehtiGraphics::createRenderPass()
{
    VkAttachmentDescription colorattachment{};
    colorattachment.format = swapChainImageFormat;
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


    VkSubpassDescription subpass{ };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachRef;

    VkSubpassDependency depend{};
    depend.srcSubpass = VK_SUBPASS_EXTERNAL;
    depend.dstSubpass = 0;
    depend.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depend.srcAccessMask = 0;
    depend.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depend.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderpassInfo{};
    renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpassInfo.attachmentCount = 1;
    renderpassInfo.pAttachments = &colorattachment;
    renderpassInfo.subpassCount = 1;
    renderpassInfo.pSubpasses = &subpass;
    renderpassInfo.dependencyCount = 1;
    renderpassInfo.pDependencies = &depend;

    if (vkCreateRenderPass(logDevice, &renderpassInfo, nullptr, &renderPass) != VK_SUCCESS)
        throw std::runtime_error("failed to create a renderpass");



}

void RehtiGraphics::createGraphicsPipeline()
{

    VkPipelineShaderStageCreateInfo vertInfo = ShaderManager::createVertexShaderInfo(this->logDevice);
    VkPipelineShaderStageCreateInfo fragInfo = ShaderManager::createFragmentShaderInfo(this->logDevice);

    VkPipelineShaderStageCreateInfo stages[2] = { vertInfo, fragInfo };


    VkPipelineVertexInputStateCreateInfo vertInputInfo{};
    vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInputInfo.vertexBindingDescriptionCount = 0;
    vertInputInfo.pVertexBindingDescriptions = nullptr;
    vertInputInfo.vertexAttributeDescriptionCount = 0;
    vertInputInfo.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewPort{};
    viewPort.x = 0.f;
    viewPort.y = 0.f;
    viewPort.width = swapChainExtent.width;
    viewPort.height = swapChainExtent.height;
    viewPort.minDepth = 0.f;
    viewPort.maxDepth = 1.f;

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = swapChainExtent;


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
    rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterInfo.depthBiasEnable = VK_FALSE;
    rasterInfo.depthBiasConstantFactor = 0.f; //No depth bias so optional?
    rasterInfo.depthBiasClamp = 0.f;
    rasterInfo.depthBiasSlopeFactor = 0.f;

    VkPipelineMultisampleStateCreateInfo multInfo{};
    multInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multInfo.sampleShadingEnable = VK_FALSE;
    multInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multInfo.minSampleShading = 1.f; //optional stuff
    multInfo.pSampleMask = nullptr;
    multInfo.alphaToCoverageEnable = VK_FALSE;
    multInfo.alphaToOneEnable = VK_FALSE;

    //Vk depth and stencil testing info here


    //Colorblending
    VkPipelineColorBlendAttachmentState colorBlendState{};
    colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendState.blendEnable = VK_FALSE;
    //rest is optional. Maybe adding later


    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; //optional
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendState;
    //constants optional


    //Pipelineinfo for pipelinelayout stuff, uniforms
    VkPipelineLayoutCreateInfo pipelinelayoutInfo{};
    pipelinelayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelinelayoutInfo.setLayoutCount = 0; //optional, as is the rest for now

    //create pipelinelayout
    if (vkCreatePipelineLayout(logDevice, &pipelinelayoutInfo, nullptr, &pipelineLayout))
        throw std::runtime_error("Pipeline layout creation failed");

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
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    //create the actual pipeline
    if (vkCreateGraphicsPipelines(logDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) //more parameters than usual
        throw std::runtime_error("Pipeline layout creation failed");


    // cleanup
    ShaderManager::destroyShaderModules(this->logDevice);
}

void RehtiGraphics::createFramebuffers()
{
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0u; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = { swapChainImageViews[i] };

        VkFramebufferCreateInfo frameInfo{};
        frameInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameInfo.renderPass = renderPass;
        frameInfo.attachmentCount = 1;
        frameInfo.pAttachments = attachments;
        frameInfo.width = swapChainExtent.width;
        frameInfo.height = swapChainExtent.height;
        frameInfo.layers = 1;
        if (vkCreateFramebuffer(logDevice, &frameInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create a framebuffer");

    }



}

void RehtiGraphics::createCommandPool()
{

    auto queuefamilyIndices = findQueueFamilies(gpu);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queuefamilyIndices.graphicsFamily.value();
    poolInfo.flags = 0;

    if (vkCreateCommandPool(logDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create a command pool");


}

void RehtiGraphics::createCommandBuffers()
{
    commandBuffers.resize(swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{}; //wow, alloc and not create?
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(logDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers");

    for (size_t i = 0u; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo cmdInfo{};
        cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdInfo.flags = 0; //optional


        //Let's begin recording
        if (vkBeginCommandBuffer(commandBuffers[i], &cmdInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin command buffer recording");


        VkRenderPassBeginInfo renderInfo{};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderInfo.renderPass = renderPass;
        renderInfo.framebuffer = swapChainFramebuffers[i];
        renderInfo.renderArea.offset = { 0,0 };
        renderInfo.renderArea.extent = swapChainExtent;

        VkClearValue clearCol = { {{0.f, 0.f, 0.f, 1.f}} };
        renderInfo.clearValueCount = 1;
        renderInfo.pClearValues = &clearCol;


        vkCmdBeginRenderPass(commandBuffers[i], &renderInfo, VK_SUBPASS_CONTENTS_INLINE); //void

        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("Failed to record command buffer");

        //recording has ended


    }



}

void RehtiGraphics::createSynchronization()
{
    imagesReady.resize(concurrentFrames);
    rendersFinished.resize(concurrentFrames);
    frameFences.resize(concurrentFrames);
    imageFences.resize(swapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaInfo{};
    semaInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < concurrentFrames; i++) {
        if (vkCreateSemaphore(logDevice, &semaInfo, nullptr, &imagesReady[i]) != VK_SUCCESS
            || vkCreateSemaphore(logDevice, &semaInfo, nullptr, &rendersFinished[i]) != VK_SUCCESS
            || vkCreateFence(logDevice, &fenceInfo, nullptr, &frameFences[i]) != VK_SUCCESS)
            throw std::runtime_error("Creating synchros failed");
    }
}


void RehtiGraphics::drawFrame()
{
    //wait
    vkWaitForFences(logDevice, 1, &frameFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(logDevice, swapChain, UINT64_MAX, imagesReady[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (imageFences[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(logDevice, 1, &imageFences[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imageFences[imageIndex] = frameFences[currentFrame]; //

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


    VkSemaphore waitSemaphores[] = { imagesReady[currentFrame] };
    VkSemaphore signalSemaphores[] = { rendersFinished[currentFrame] };

    VkSwapchainKHR swapChains[] = { swapChain };

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;


    vkResetFences(logDevice, 1, &frameFences[currentFrame]); //resets fence signal for use

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameFences[currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("Failed to submit draw command buffer");

    VkPresentInfoKHR presInfo{};
    presInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presInfo.waitSemaphoreCount = 1;
    presInfo.pWaitSemaphores = signalSemaphores;

    presInfo.swapchainCount = 1;
    presInfo.pSwapchains = swapChains;
    presInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presInfo);

    currentFrame = (currentFrame + 1) % concurrentFrames;
}

void RehtiGraphics::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(logDevice);
}

void RehtiGraphics::cleanup() {

    for (size_t i = 0; i < concurrentFrames; i++) {
        vkDestroySemaphore(logDevice, rendersFinished[i], nullptr);
        vkDestroySemaphore(logDevice, imagesReady[i], nullptr);
        vkDestroyFence(logDevice, frameFences[i], nullptr);
    }

    vkDestroyCommandPool(logDevice, commandPool, nullptr);

    for (auto framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(logDevice, framebuffer, nullptr);


    vkDestroyPipeline(logDevice, pipeline, nullptr);
    vkDestroyPipelineLayout(logDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(logDevice, renderPass, nullptr);

    for (auto imageView : this->swapChainImageViews)
        vkDestroyImageView(this->logDevice, imageView, nullptr);

    vkDestroySwapchainKHR(this->logDevice, this->swapChain, nullptr); //destroy swapchain

    vkDestroyDevice(this->logDevice, nullptr); //queues are destroyed when logicaldevice is destroyed

    if (this->enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(this->instance, debugMessenger, nullptr);
    }
    //Destroy the surface
    vkDestroySurfaceKHR(this->instance, this->surface, nullptr);

    //Take out the instance
    vkDestroyInstance(this->instance, nullptr);

    //Then window
    glfwDestroyWindow(window);

    glfwTerminate();
}

//Helpers or smaller functions

void RehtiGraphics::createInstance() {

    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
    //Create info
    VkApplicationInfo info{};
    //And fill in the form, as always
    info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    info.pApplicationName = "VKrenderer";
    info.applicationVersion = VK_MAKE_VERSION(1, 0, 0); // version 1.00?
    info.pEngineName = "No Engine"; //Why tho?
    info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    info.apiVersion = VK_API_VERSION_1_0;

    //Create info:
    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &info;
    //Next up, extensions
    auto extensions = this->getRequiredExtensions();

    instanceInfo.enabledExtensionCount = extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationlayers.size()); //Names are null for now
        instanceInfo.ppEnabledLayerNames = validationlayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        instanceInfo.enabledLayerCount = 0;

        instanceInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&instanceInfo, nullptr, &this->instance) != VK_SUCCESS) { //Second argument is custom allocator callback which is null for now
        throw std::runtime_error("Failed to create an instance.");
    }


}

void RehtiGraphics::createSurface()
{
    if (glfwCreateWindowSurface(this->instance, window, nullptr, &this->surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

bool RehtiGraphics::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> properties(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, properties.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& property : properties) {
        requiredExtensions.erase(property.extensionName); //tick off the required extensions
    }


    return requiredExtensions.empty();
}

bool RehtiGraphics::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties>availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : this->validationlayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }

        }

        if (!layerFound) {
            return false;
        }

    }

    return true;

}

bool RehtiGraphics::isDeviceSuitable(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props; //properties
    VkPhysicalDeviceFeatures feats; //and features of the gpu
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

    return props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && indice.isComplete() && requiredExtensionSupport; // Suitable, if it is an external gpu and has a graphics queueFamily
}


int RehtiGraphics::rateDevice(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties props; // properties
    VkPhysicalDeviceFeatures feats; // and features of the gpu
    vkGetPhysicalDeviceProperties(device, &props);
    vkGetPhysicalDeviceFeatures(device, &feats);

    return 1;
}

QueueFamilyIndices RehtiGraphics::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indexcase;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (auto i = 0u; i < queueFamilies.size(); i++) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) 
        {
            indexcase.presentFamily = i;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indexcase.graphicsFamily = i;

        if (indexcase.isComplete())
            break; // Early exit

    }


    return indexcase;
}

SwapChainSupportDetails RehtiGraphics::querySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities); //A bit unorthodox for vulkan but I guess it will do

    //now to the formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);    // Usually we just created the vector, but now that we have a struct we just resize it
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    //And the same for presentModes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);    //usually we just created the vector, but now that we have a struct we just resize it
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }


    return details;
}

std::vector<const char*> RehtiGraphics::getRequiredExtensions()
{
    uint32_t extCount;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&extCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extCount);

    if (this->enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkSurfaceFormatKHR RehtiGraphics::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats)
{
    for (const auto& format : availableFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }

    return availableFormats[0];
}

VkPresentModeKHR RehtiGraphics::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availableModes)
{
    for (const auto& presentmode : availableModes) {
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
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actual = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
        };

        actual.width = std::clamp(actual.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actual.height = std::clamp(actual.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actual;
    }
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


VkResult RehtiGraphics::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void RehtiGraphics::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }

}