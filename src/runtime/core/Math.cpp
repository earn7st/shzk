#include "Math.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace shzk::Math
{
	glm::quat EulerDegreeToQuat(glm::vec3 eulerDegrees)
	{
        float pitch = glm::radians(eulerDegrees.x);
        float yaw = glm::radians(eulerDegrees.y);
        float roll = glm::radians(eulerDegrees.z);

        return  glm::angleAxis(yaw, glm::vec3(0.f, 1.f, 0.f)) *
                glm::angleAxis(pitch, glm::vec3(1.f, 0.f, 0.f)) *
                glm::angleAxis(roll, glm::vec3(0.f, 0.f, 1.f));
	}

	glm::vec3 QuatToEulerDegree(glm::quat quat)
	{
        glm::mat3 R = glm::mat3_cast(quat);

        float pitch = glm::asin(-R[2][1]);
        float yaw = glm::atan(R[2][0], R[2][2]);
        float roll = glm::atan(R[0][1], R[1][1]);

        return glm::vec3(glm::degrees(pitch), glm::degrees(yaw), glm::degrees(roll));
	}

    float ClampPitch(float currentPitch, float delta)
    {
        constexpr float kPitchLimit = 89.f;
        return glm::clamp(currentPitch + delta, -kPitchLimit, kPitchLimit) - currentPitch;
    }

    //glm::mat4x4 LookAtLHS(glm::vec3 position, glm::vec3 front, glm::vec3 up)
    //{
    //    glm::vec3 f = glm::normalize(front);
    //    glm::vec3 r = glm::normalize(glm::cross(up, f));
    //    glm::vec3 u = glm::cross(f, r); 

    //    glm::mat4 view(1.0f);
    //    view[0] = glm::vec4(r.x, u.x, f.x, 0.0f);
    //    view[1] = glm::vec4(r.y, u.y, f.y, 0.0f);
    //    view[2] = glm::vec4(r.z, u.z, f.z, 0.0f);
    //    view[3] = glm::vec4(-glm::dot(r, position), -glm::dot(u, position), -glm::dot(f, position), 1.0f);
    //    return view;    
    //}

    //glm::mat4x4 Perspective(float fovY, float aspect, float near, float far)
    //{
    //    const float f = 1.f / glm::tan(glm::radians(fovY) * 0.5f);

    //    glm::mat4 proj(0.f);
    //    proj[0][0] = f / aspect;
    //    proj[1][1] = -f;                            // Vulkan NDC +Y 朝下 → 翻转
    //    proj[2][2] = near / (near - far);           // reverse-Z 的 z 缩放
    //    proj[2][3] = 1.f;                           // clip.w = z（view 空间 +Z 前）
    //    proj[3][2] = -far * near / (near - far);    // reverse-Z 的 z 偏移
    //    return proj;
    //}
}