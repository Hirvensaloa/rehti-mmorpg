#include "Camera.hpp"

#include <iostream>
#include <GLFW/glfw3.h>


double Camera::mouseX = 0;
double Camera::mouseY = 0;
bool Camera::canMove = false;

Camera::Camera(glm::vec3 targetPos, float width, float height, float fovRad, float near, float far, float sensitivity)
	: targetM(targetPos)
	, cameraMatrixM(1.f) // identity
	, sensitivityM(sensitivity)
	, zoomM(STANDARD_ZOOM)
	, zoomSensitivityM(50.f * sensitivity)
	, widthM(width)
	, heightM(height)
{
	projectionM = glm::perspective(fovRad, width / height, near, far);
	//projectionM[1][1] *= -1; // flip y axis
	// cameraMatrixM[2][2] = -1.f; // forward is negative zed
	moveLocation(-getForward() * zoomM);
}

glm::mat4 Camera::getViewMatrix() const
{
	glm::mat4 view = glm::lookAt(getLocation(), getForward(), getUp());

	return view;
}

glm::mat4 Camera::getScuffedViewMatrix() const
{
	// construct inverse of the camera matrix.
	// Due to orthonormality (of the orientation), the inverse is the transpose of the orientation, with negated translation.
	glm::mat4 viewMatrix = glm::transpose(cameraMatrixM);
	glm::vec3 locationInverse = -getLocation();
	viewMatrix[3][0] = 0.f;
	viewMatrix[3][1] = 0.f;
	viewMatrix[3][2] = -zoomM;
	viewMatrix[0][3] = 0.f;
	viewMatrix[1][3] = 0.f;
	viewMatrix[2][3] = 0.f;
	viewMatrix[3][3] = 1.f;
	return viewMatrix;
}

glm::mat4 Camera::getOrientation() const
{
	return cameraMatrixM;
}

glm::mat4 Camera::getProjectionMatrix() const
{
	return projectionM;
}

glm::vec3 Camera::getCameraRay(double x, double y) const
{
	std::cout << "Asked screen coordinates: " << x << ", " << y << "\n" << std::endl;
	// Create a ray in world space from the camera to the given screen coordinates.
	// The ray is normalized.
	float normx = (2.f * x) / widthM - 1.f;
	float normy = 1.f - (2.f * y) / heightM;
	glm::vec3 normDir = glm::vec3(normx, normy, -1.f);
	glm::mat3 cameraOrientation = glm::mat3(getOrientation());
	glm::vec3 ray = glm::normalize(cameraOrientation * normDir);
	std::cout << "Generated ray: " << ray.x << ", " << ray.y << ", " << ray.z << "\n" << std::endl;
	return ray;
}

glm::mat4 Camera::getWorldToScreenMatrix() const
{
	return projectionM * getViewMatrix();
}

uint32_t Camera::getUboSize()
{
	return sizeof(glm::mat4);
}

void Camera::orbitRotate(glm::vec2 rotationVec)
{
	// In radians
	float horizontalAngle = rotationVec.x * sensitivityM;
	float verticalAngle = rotationVec.y * sensitivityM;

	glm::mat4 rotatedHorizontal = glm::rotate(glm::mat4(1.f), horizontalAngle, POSITIVE_Y_AXIS);
	glm::vec3 rightVec = getRight();
	glm::mat4 rotatedVertical = glm::rotate(glm::mat4(1.f), verticalAngle, rightVec);
	cameraMatrixM = rotatedVertical * rotatedHorizontal * cameraMatrixM;
}

void Camera::zoom(float zoomAmount)
{
	float newZoom = zoomM + zoomAmount * zoomSensitivityM;
	// clamp zoom
	newZoom = glm::min(newZoom, MAX_ZOOM);
	zoomM = glm::max(newZoom, MIN_ZOOM);
}

void Camera::setSensitivity(float newSensitivity, float newZoomSens)
{
	sensitivityM = newSensitivity;
	zoomSensitivityM = newZoomSens;
}

void Camera::moveLocation(glm::vec3 movement)
{
	cameraMatrixM[3][0] += movement.x;
	cameraMatrixM[3][1] += movement.y;
	cameraMatrixM[3][2] += movement.z;
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

glm::vec3 Camera::getForward() const
{
	return glm::vec3(glm::column(cameraMatrixM, 2));
}

glm::vec3 Camera::getRight() const
{
	return glm::vec3(glm::column(cameraMatrixM, 0));
}

glm::vec3 Camera::getUp() const
{
	return glm::vec3(glm::column(cameraMatrixM, 1));
}

glm::vec2 Camera::getSensitivities() const
{
	return glm::vec2(sensitivityM, zoomSensitivityM);
}

#pragma region CameraCallbacks

std::function<void(glm::vec2)> Camera::cameraUpdateCallback = nullptr;
std::function<void(float)> Camera::cameraZoomCallback = nullptr;

void Camera::registerCameraControls(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, Camera::cursorPosCallback);
	glfwSetScrollCallback(window, Camera::scrollCallback);
	cameraUpdateCallback = [&](glm::vec2 rotation) { orbitRotate(rotation); };
	cameraZoomCallback = [&](float zoomAmount) { zoom(zoomAmount); };
}

void Camera::cursorPosCallback(GLFWwindow* pWindow, double xpos, double ypos)
{
	double deltaX = xpos - mouseX;
	double deltaY = ypos - mouseY;
	mouseX = xpos;
	mouseY = ypos;

	if (canMove)
	{
		float rotationHorizontal = deltaX;
		float rotationVertical = -deltaY;
		glm::vec2 rotationVec = glm::vec2(rotationHorizontal, rotationVertical);
		cameraUpdateCallback(rotationVec);
	}
}

void Camera::scrollCallback(GLFWwindow* pWindow, double xOffSet, double yOffSet)
{
	float zoom = -yOffSet; // standard mouse wheels provide only the y offset
	cameraZoomCallback(zoom);
}
#pragma endregion
