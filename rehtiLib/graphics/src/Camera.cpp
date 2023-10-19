#include "Camera.hpp"

#include <iostream>
#include <GLFW/glfw3.h>

Camera::Camera(glm::vec3 targetPos, float width, float height, float fovRad, float near, float far, float sensitivity)
	: targetM(targetPos)
	, cameraMatrixM(1.f) // identity
	, sensitivityM(sensitivity)
{
	projectionM = glm::perspective(fovRad, width / height, near, far);
	projectionM[1][1] *= -1; // flip y axis
}

glm::mat4 Camera::getViewMatrix() const
{
	// construct inverse of the camera matrix.
	// Due to orthonormality (of the orientation), the inverse is the transpose of the orientation, with negated translation.
	glm::mat4 viewMatrix = glm::transpose(cameraMatrixM);
	glm::vec3 locationInverse = -getLocation();
	viewMatrix[3][0] = locationInverse.x;
	viewMatrix[3][1] = locationInverse.y;
	viewMatrix[3][2] = locationInverse.z;

	return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return projectionM;
}

glm::mat4 Camera::getWorldToScreenMatrix() const
{
	return projectionM * getViewMatrix();
}

void Camera::orbitRotate(glm::vec2 rotationVec)
{
	// In radians
	float horizontalAngle = rotationVec.x * sensitivityM;
	float verticalAngle = rotationVec.y * sensitivityM;

	glm::mat4 rotatedHorizontal = glm::rotate(cameraMatrixM, horizontalAngle, POSITIVE_Y_AXIS);
	glm::vec3 rightVec = glm::vec3(glm::column(cameraMatrixM, 0)); // drops the w component
	cameraMatrixM = glm::rotate(rotatedHorizontal, verticalAngle, rightVec);
}

void Camera::registerCameraControls(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, Camera::cursorPosCallback);
	cameraUpdateCallback = [&](glm::vec2 rotation) { orbitRotate(rotation); };
}

void Camera::setSensitivity(float newSensitivity)
{
	sensitivityM = newSensitivity;
}

glm::mat4 Camera::getCameraMatrixOrigon() const
{
	glm::mat4 cameraMatrix = cameraMatrixM;
	glm::vec3 targetToCamera = getLocation() - targetM;
	cameraMatrix[3][0] = targetToCamera.x;
	cameraMatrix[3][1] = targetToCamera.y;
	cameraMatrix[3][2] = targetToCamera.z;

	return cameraMatrix;
}

glm::vec3 Camera::getLocation() const
{
	return glm::vec3(glm::column(cameraMatrixM, 3));
}

void Camera::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	double deltaX = xpos - mouseX;
	double deltaY = ypos - mouseY;
	std::cout << "Mouse movement: " << deltaX << ", " << deltaY << std::endl;
	mouseX = xpos;
	mouseY = ypos;

	float rotationHorizotal = deltaX;
	float rotationVertical = deltaY;
	glm::vec2 rotationVec = glm::vec2(rotationHorizotal, rotationVertical);
	cameraUpdateCallback(rotationVec);
}

void Camera::moveLocation(glm::vec3 movement)
{
	cameraMatrixM[3][0] += movement.x;
	cameraMatrixM[3][1] += movement.y;
	cameraMatrixM[3][2] += movement.z;
}
