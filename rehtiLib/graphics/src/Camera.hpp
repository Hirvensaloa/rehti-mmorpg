#pragma once
// Include both standard functionality and the extensions
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

constexpr float STANDARD_ZOOM(7.f); // zoom used in the constructor. 10 Units away from the target
constexpr float MIN_ZOOM(2.f); // minimum zoom
constexpr float MAX_ZOOM(15.f); // maximum zoom

/// <summary>
/// Camera class, that allows for orbiting around a target.
/// Can be queried for view and projection matrices, or combination of both.
/// TODO Possibly make this a singleton Is there really need for multiple cameras is the question.
/// </summary>
class Camera
{
public:
	/// <summary>
	/// Constructor for the camera.
	/// </summary>
	/// <param name="target">Target to follow. TODO Changed later</param>
	/// <param name="width">Width of the screen</param>
	/// <param name="height">Height of the screen</param>
	/// <param name="fovRad">Field of view in radians, defaults to quarter pi = 45 degrees</param>
	/// <param name="near">Near plane distance</param>
	/// <param name="far">Far plane distance</param>
	Camera(glm::vec3 targetPos, float width, float height, float fovRad = glm::quarter_pi<float>(), float near = 0.1f, float far = 100.f, float sensitivity = 0.01f);

	/// <summary>
	/// Returns the view matrix of the camera, which is the inverse of the model matrix of the camera.
	/// It places objects to the space of the camera.
	/// </summary>
	/// <returns>The view matrix</returns>
	glm::mat4 getViewMatrix() const;
	glm::mat4 getScuffedViewMatrix() const;

	/// <summary>
	/// Returns the orientation of the camera.
	/// </summary>
	/// <returns></returns>
	glm::mat4 getOrientation() const;

	/// <summary>
	/// Returns the projection matrix of the camera.
	/// </summary>
	/// <returns>The projection matrix</returns>
	glm::mat4 getProjectionMatrix() const;

	/**
	 * @brief Returns the camera ray in world space.
	 * @return Camera ray in world space.
	*/
	glm::vec3 getCameraRay(double x, double y) const;

	/// <summary>
	/// Returns a matrix that transforms from world space to screen space.
	/// e.g. view matrix times projection.
	/// </summary>
	/// <returns></returns>
	glm::mat4 getWorldToScreenMatrix() const;

	/// <summary>
	/// Returns the size of camera push constant range.
	/// </summary>
	/// <returns></returns>
	uint32_t getUboSize();

	/// <summary>
	/// Rotates/orbits the camera around the target by the given vector.
	/// </summary>
	/// <param name="rotationVec">Vector signifying rotations around positive y axis, and around camera right vector.</param>
	void orbitRotate(glm::vec2 rotationVec);

	/// <summary>
	/// Zooms the camera by the given amount.
	/// </summary>
	/// <param name="zoomAmount"></param>
	void zoom(float zoomAmount);

	/// <summary>
	/// Registers this camera to the given window, so that it can be controlled by the mouse.
	/// Also sets the callback for updating the camera.
	/// </summary>
	/// <param name="window"></param>
	void registerCameraControls(GLFWwindow* window);

	/// <summary>
	/// Sets the sensitivity of the camera.
	/// </summary>
	/// <param name="newSensitivity"></param>
	void setSensitivity(float newSensitivity, float newZoomSens);

	/// <summary>
	/// Moves the location of the camera by the given vector.
	/// </summary>
	/// <param name="movement">Movement to the camera.</param>
	void moveLocation(glm::vec3 movement);

	/// <summary>
	/// Returns the location of the camera.
	/// </summary>
	/// <returns></returns>
	glm::vec3 getLocation() const;

	/// <summary>
	/// The following three functions return the vectors of the camera.
	/// </summary>
	/// <returns></returns>
	glm::vec3 getForward() const;
	glm::vec3 getRight() const;
	glm::vec3 getUp() const;

	/// <summary>
	/// Returns both camera movement sensitivity and zoom sensitivity.
	/// This function is useful when someone needs to set either of the values to older ones.
	/// </summary>
	/// <returns>movement sensitivity as x, and zoom sensitivity as y</returns>
	glm::vec2 getSensitivities() const;

private:

	/// <summary>
	/// Returns the camera matrix centered around origon.
	/// </summary>
	/// <returns></returns>
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
	static void cursorPosCallback(GLFWwindow* pWindow, double xpos, double ypos); // Callback for registering mouse movement
	static void scrollCallback(GLFWwindow* pWindow, double xOffSet, double yOffSet); // callback for registering mouse scroll
	static std::function<void(glm::vec2)> cameraUpdateCallback; // Callback for updating the camera
	static std::function<void(float)> cameraZoomCallback; // Callback for updating the camera
};

