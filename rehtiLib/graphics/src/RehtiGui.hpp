#pragma once
#include "../../assets/reader/src/AssetCache.hpp"
#include "../../network/src/api/Types.hpp"
#include "../bindings/imgui_impl_glfw.h"
#include "../bindings/imgui_impl_vulkan.h"
#include "GraphicsTypes.hpp"

#include <functional>
#include <mutex>
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

/**
 * @brief Class that manages the GUI window
 */
class RehtiGui
{

public:
    RehtiGui(VkInstance instance, VkPhysicalDevice gpu, VkDevice logDevice, GLFWwindow* pWindow, VkQueue graphicsQueue, VkDescriptorPool descPool, uint32_t imageCount, VkRenderPass renderPass, std::shared_ptr<GraphicsObjectManager> pGfxManager);
    ~RehtiGui();

    void uploadFonts(VkCommandBuffer cmdBuffer);
    void uploadEnded();
    void newFrame();

    /**
     * @brief Records ui primitives into the given command buffer
     * @param cmdBuff
     */
    void recordGuiData(VkCommandBuffer cmdBuff);

    /**
     * @brief Starts rendering ui primitives. Must be called at the top of draw function.
     */
    void startRender();

    /**
     * @brief Adds callback function for clicking an item in inventory
     * @param callback
     */
    void addInventoryItemClickCallback(std::function<void(const int id)> callback);

    /**
     * @brief Adds callback function for dropping an item in inventory
     * @param callback
     */
    void addDropItemCallback(std::function<void(const int id)> callback);

    /**
     * @brief Adds callback function for clicking and item in equipment
     * @param callback
     */
    void addEquipmentItemClickCallback(std::function<void(const int id)> callback);

    /**
     * @brief Set contents of player's inventory
     * @param inventory
     */
    void setInventory(std::vector<GameItem> inventory);

    /**
     * @brief Get reference to the player's equipment vector
     * @return std::vector<GameItem>&
     */
    std::vector<GameItem>& getEquipment();

    /**
     * @brief Set contents of player's equipment
     * @param equipment
     */
    void setEquipment(std::vector<GameItem> equipment);

    /**
     * @brief Set skills of player
     * @param skills
     */
    void setSkills(std::vector<Skill> skills);

    /**
     * @brief Set hp of player
     * @param hp
     */
    void setHp(int hp);

    /**
     * @brief Load texture from file and save it to member map guiIconsM with key id
     * @param filename
     * @param id
     * @return success
     */
    bool LoadTextureFromFile(const char* filename, const int id);

    /**
     * @brief Load texture from ImageData and save it to member map guiIconsM with key id
     * @param id
     * @param imgData
     * @return success
     */
    bool LoadTextureFromImageData(const int id, ImageData& imgData);

private:
    /**
     * @brief Draws inventory tab to GUI window
     */
    void drawInventory();

    /**
     * @brief Draws equipment tab to GUI window
     */
    void drawEquipment();

    /**
     * @brief Draws skills tab to GUI window
     */
    void drawSkills();

    /**
     * @brief Helper function for a single equipment slot
     * @param index
     */
    void drawEquipmentSlot(int index);

    VkDevice logDeviceM;
    VkDescriptorPool descPoolM;

    VkQueue graphicsQueueM;
    VkSampler samplerM;

    std::shared_ptr<GraphicsObjectManager> pGraphicsObjectManagerM;

    UiTab openTabM;

    std::function<void(const int id)> inventoryItemClickCallbackM;
    std::function<void(const int id)> dropItemCallbackM;
    std::function<void(const int id)> equipmentItemClickCallbackM;

    std::vector<GameItem> inventoryM;
    std::vector<GameItem> equipmentM;
    std::vector<Skill> skillsM;

    int hpM;

    std::mutex invMutexM;
    std::mutex equipmentMutexM;
    std::mutex skillsMutexM;

    int windowWidthM = 300;
    int windowHeightM = 500;
    ImVec2 iconSizeM = ImVec2(windowWidthM * 0.8 / 4, windowWidthM * 0.8 / 4);

    AssetCache& assetCacheM;

    // map from id to descriptor set required for drawing.
    // GraphicsObjectManager will clean up the resources created.
    std::unordered_map<int, VkDescriptorSet> guiIconsM;
};
