#pragma once

#include "Math.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Coordinates
// World - Right-handed: forward: +x, right: +z, up: +y
// View  - Right-handed: forward: -z, right: +x, up: +y

namespace shzk
{
	class Transform
	{
	public:
		inline void SetTranslation(const glm::vec3& pos) { m_translation = pos; }
		inline void SetRotation(const glm::quat& r) { m_rotation = r; m_eulerDegree = Math::QuatToEulerDegree(m_rotation); }
		inline void SetEulerDegree(const glm::vec3& eulerDegree) { m_eulerDegree = eulerDegree; SetRotation(Math::EulerDegreeToQuat(eulerDegree)); }
		inline void SetScale(const glm::vec3& s) { m_scale = s; }

		inline glm::vec3 GetPosition() const { return m_translation; }
		inline glm::quat GetRotation() const { return m_rotation; }
		inline glm::vec3 GetEulerDegree() const { return m_eulerDegree; }
		inline glm::vec3 GetScale() const { return m_scale; }

		inline glm::vec3 Front() const { return glm::normalize(m_rotation * glm::vec3(0.f, 0.f, -1.f)); }
		inline glm::vec3 Up() const { return glm::normalize(m_rotation * glm::vec3(0.f, 1.f, 0.f)); };
		inline glm::vec3 Right() const { return glm::normalize(m_rotation * glm::vec3(1.f, 0.f, 0.f)); }

		inline void Translate(const glm::vec3& t) { m_translation += t; }
		inline void Rotate(const glm::vec3& eulerDegree);	// apply sequence: yaw(Z) pitch(Y) roll(x)
		inline void Scale(const glm::vec3& s) { m_scale *= s; }

		glm::mat4x4 ToMat4x4() const;

	private:
		inline void Rotate(const glm::quat& delta);

		glm::vec3 m_translation = { 0.f, 0.f, 0.f };
		glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);	// truth of rotation
		glm::vec3 m_eulerDegree = { 0.f, 0.f, 0.f };	// pitch(Y) yaw(Z) roll(X), user-oriented
		glm::vec3 m_scale = { 1.f, 1.f, 1.f };

	};
}