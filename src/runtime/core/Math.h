#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace shzk::Math
{
	glm::quat EulerDegreeToQuat(glm::vec3 euler);
	glm::vec3 QuatToEulerDegree(glm::quat quat);

	float ClampPitch(float currentPitch, float delta);

	glm::mat4x4 PerspectiveRH_ZO_ReverseZ(float fovY, float aspect, float near, float far);
	// glm::mat4x4 LookAtLHS(glm::vec3 position, glm::vec3 front, glm::vec3 up);
	// glm::mat4x4 Perspective(float fovY, float aspect, float nearPlane, float farPlane)f;
}