#include "Camera.hpp"


Camera::Camera(const Target& target, float width, float height, float fovRad, float, float)
	: targetM(target.pos)
	, cameraMatrixM(1.f) // identity
{
	projectionM = glm::perspective(fovRad, width / height, 0.1f, 100.f);
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
	float horizontalAngle = rotationVec.x;
	float verticalAngle = rotationVec.y;

	glm::mat4 rotatedHorizontal = glm::rotate(cameraMatrixM, horizontalAngle, POSITIVE_Y_AXIS);
	glm::vec3 rightVec = glm::vec3(glm::column(cameraMatrixM, 0)); // drops the w component
	cameraMatrixM = glm::rotate(rotatedHorizontal, verticalAngle, rightVec);
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

void Camera::moveLocation(glm::vec3 movement)
{
	cameraMatrixM[3][0] += movement.x;
	cameraMatrixM[3][1] += movement.y;
	cameraMatrixM[3][2] += movement.z;
}
