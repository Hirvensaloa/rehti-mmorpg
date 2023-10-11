#pragma once
// Include both standard functionality and the extensions
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <functional>

// TODO figure out how camera should function with the player target.
struct Target
{
	glm::vec3 pos;
};
// standard cartesian coordinate system
constexpr glm::vec3 POSITIVE_X_AXIS(1.f, 0.f, 0.f);
constexpr glm::vec3 POSITIVE_Y_AXIS(0.f, 1.f, 0.f); 
constexpr glm::vec3 POSITIVE_Z_AXIS(0.f, 0.f, 1.f);

constexpr float STANDARD_ZOOM(10.f); // zoom used in the constructor. 10 Units away from the target

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
	Camera(const Target& target, float width, float height, float fovRad = glm::quarter_pi<float>(), float near, float far);

	/// <summary>
	/// Returns the view matrix of the camera, which is the inverse of the model matrix of the camera.
	/// It places objects to the space of the camera.
	/// </summary>
	/// <returns>The view matrix</returns>
	glm::mat4 getViewMatrix() const;

	/// <summary>
	/// Returns the projection matrix of the camera.
	/// </summary>
	/// <returns>The projection matrix</returns>
	glm::mat4 getProjectionMatrix() const;

	/// <summary>
	/// Returns a matrix that transforms from world space to screen space.
	/// e.g. view matrix times projection.
	/// </summary>
	/// <returns></returns>
	glm::mat4 getWorldToScreenMatrix() const;

	/// <summary>
	/// Rotates/orbits the camera around the target by the given vector.
	/// </summary>
	/// <param name="rotationVec">Vector signifying rotations around positive y axis, and around camera right vector.</param>
	void orbitRotate(glm::vec2 rotationVec);

	/// <summary>
	/// Moves the location of the camera by the given vector.
	/// </summary>
	/// <param name="movement">Movement to the camera.</param>
	void moveLocation(glm::vec3 movement);

private:

	/// <summary>
	/// Returns the camera matrix centered around origon.
	/// </summary>
	/// <returns></returns>
	glm::mat4 getCameraMatrixOrigon() const;

	/// <summary>
	/// ´Returns the location of the camera.
	/// </summary>
	/// <returns></returns>
	glm::vec3 getLocation() const;

	// These encapsulate most important aspects of the camera.
	// cameraMatrixM wastes currently 4 floats, but it's easier to work with.
	glm::mat4 cameraMatrixM;
	glm::mat4 projectionM;
	const glm::vec3& targetM;
};

