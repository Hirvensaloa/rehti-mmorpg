#pragma once
// Include both standard functionality and the extensions
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include <functional>

// Forward declarations
struct GLFWwindow;

// TODO figure out how camera should function with the player target.
struct Target
{
    glm::vec3 pos;
};
// standard cartesian coordinate system
constexpr glm::vec3 POSITIVE_X_AXIS(1.f, 0.f, 0.f);
constexpr glm::vec3 POSITIVE_Y_AXIS(0.f, 1.f, 0.f);
constexpr glm::vec3 POSITIVE_Z_AXIS(0.f, 0.f, 1.f);

constexpr float STANDARD_ZOOM(12.f); // zoom used in the constructor. 10 Units away from the target
constexpr float MIN_ZOOM(2.f);       // minimum zoom
constexpr float MAX_ZOOM(25.f);      // maximum zoom

/**
 * @ brief Camera class that enables orbiting around a target.
 *		This class allows querying for view and projection matrices or a combination of both.
 */

class Camera
{
public:
    /**
     * @brief Constructor for the camera.
     *
     * @param target Target to follow. TODO Changed later
     * @param width Width of the screen
     * @param height Height of the screen
     * @param fovRad Field of view in radians, defaults to quarter pi = 45 degrees
     * @param near Near plane distance
     * @param far Far plane distance
     * @param sensitivity Sensitivity of the camera
     */
    Camera(glm::vec3 targetPos, float width, float height, float fovRad = glm::quarter_pi<float>(), float near = 0.1f, float far = 100.f, float sensitivity = 0.01f);

    /**
     * @brief Returns the view matrix of the camera, which is the inverse of the model matrix of the camera.
     *		This matrix places objects in the space of the camera.
     * @return The view matrix
     */

    glm::mat4 getViewMatrix() const;

    /**
     * @brief Returns the orientation of the camera.
     * @return orientation of the camera as a matrix
     */
    glm::mat4 getOrientation() const;

    /**
     * @brief Returns the projection matrix of the camera.
     * @return The projection matrix
     */

    glm::mat4 getProjectionMatrix() const;

    /**
     * @brief Returns the camera ray in world space.
     * @return Camera ray in world space.
     */
    glm::vec3 getCameraRay(double x, double y) const;

    /**
     * @brief Returns the world to screen matrix.
     * @return World to screen matrix.
     */
    glm::mat4 getWorldToScreenMatrix() const;

    /**
     * @brief Returns the size of the camera's UBO.
     * @return Size of the camera's UBO.
     */
    uint32_t getUboSize();

    /**
     * @brief rotates the camera around the target.
     * @param rotationVec to rotate the camera by.
     */
    void orbitRotate(glm::vec2 rotationVec);

    /**
     * @brief Zooms the camera by the given amount.
     * @param zoomAmount
     */
    void zoom(float zoomAmount);

    /**
     * @brief Registers the camera controls to the given window.
     * @param window
     */
    void registerCameraControls(GLFWwindow* window);

    /**
     * @brief sets the sensitivity of the camera.
     * @param newSensitivity of the camera movement
     * @param newZoomSens is the zoom sensitivity
     */
    void setSensitivity(float newSensitivity, float newZoomSens);

    /**
     * @brief Moves the camera by the given vector.
     * @param movement
     */
    void moveLocation(glm::vec3 movement);

    /**
     * @brief Moves both the camera and the camera target by the given vector.
     */
    void move(glm::vec3 movement);

    /**
     * @brief Sets the camera location to the given location.
     * @param location
     */
    void setLocation(glm::vec3 location);

    /**
     * @brief Sets the camera target to the given location and moves camera by the same amount.
     * @param location
     */
    void setTargetAndCamera(glm::vec3 location);

    /**
     * @brief Returns the location of the camera.
     * @return the location of the camera.
     */
    glm::vec3 getLocation() const;

    /**
     * @brief Returns the forward direction of the camera.
     * @return the forward direction of the camera.
     */
    glm::vec3 getForward() const;

    /**
     * @brief Returns the right direction of the camera.
     * @return the right direction of the camera.
     */
    glm::vec3 getRight() const;

    /**
     * @brief Returns the up direction of the camera.
     * @return the up direction of the camera.
     */
    glm::vec3 getUp() const;

    /**
     * @brief Returns the sensitivity of the camera.
     * @return the sensitivity of the camera.
     */
    glm::vec2 getSensitivities() const;

    static bool canMove;

private:
    /**
     * @brief returns the camera matrix with the target as the origon.
     * @return matrix
     */
    glm::mat4 getCameraMatrixOrigon() const;

    // These encapsulate most important aspects of the camera.
    // cameraMatrixM wastes currently 4 floats, but it's easier to work with.
    glm::mat4 cameraMatrixM;
    glm::mat4 projectionM;
    glm::vec3 targetM;
    float sensitivityM;
    float zoomSensitivityM;
    float zoomM;
    float widthM;
    float heightM;

    static double mouseX;
    static double mouseY;
    static void cursorPosCallback(GLFWwindow* pWindow, double xpos, double ypos);    ///< Callback for registering mouse movement
    static void scrollCallback(GLFWwindow* pWindow, double xOffSet, double yOffSet); ///< callback for registering mouse scroll
    static std::function<void(glm::vec2)> cameraUpdateCallback;                      ///< Callback for updating the camera
    static std::function<void(float)> cameraZoomCallback;                            ///< Callback for updating the camera
};
