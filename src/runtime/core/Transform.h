#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace shzk
{
	class Transform
	{
	public:
		inline void SetTranslation(const glm::vec3& t) { m_translation = t; }
		inline void SetRotation(const glm::quat& r) { m_rotation = r; }
		inline void SetScale(const glm::vec3& s) { m_scale = s; }

		inline glm::vec3 GetTranslation() const { return m_translation; }
		inline glm::quat GetRoatation() const { return m_rotation; }
		inline glm::vec3 GetScale() const { return m_scale; }
		inline glm::vec3 GetEulerAngle() const { return m_euler; }

		inline glm::vec3 Front() const { return m_front; }
		inline glm::vec3 Up() const { return m_up; }
		inline glm::vec3 Right() const { return m_right; }

		inline void Translate(glm::vec3 t) { m_translation += t; }
		inline void Rotate(glm::vec3 eulerAngle);
		inline void Rotate(glm::quat quat);
		inline void Scale(glm::vec3 s) { m_scale *= s; }
		inline glm::mat4x4 ToMat4x4() const;

	private:
		glm::vec3 m_translation = { 0.f, 0.f, 0.f };
		glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 m_euler = { 0.f, 0.f, 0.f };
		glm::vec3 m_scale = { 1.f, 1.f, 1.f };

		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;
	};
}