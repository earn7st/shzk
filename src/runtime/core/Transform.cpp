#include "Transform.h"
#include "Math.h"

#include <glm/glm.hpp>

namespace shzk
{
	void Transform::Rotate(const glm::vec3& eulerDegree)
	{
		Rotate(Math::EulerDegreeToQuat(eulerDegree));
	}

	glm::mat4x4 Transform::ToMat4x4() const
	{
		glm::mat4 T = glm::translate(glm::mat4(1.0f), m_translation);
		glm::mat4 R = glm::mat4_cast(m_rotation);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), m_scale);
		return T * R * S;
	}

	// --- private functions ---
	void Transform::Rotate(const glm::quat& delta)
	{
		m_rotation = m_rotation * delta;
		m_eulerDegree = Math::QuatToEulerDegree(m_rotation);
	}
}