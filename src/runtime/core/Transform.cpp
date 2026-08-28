#include "Transform.h"

#include <glm/glm.hpp>

namespace shzk
{
	glm::mat4x4 Transform::ToMat4x4() const
	{
		glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
		glm::mat4 R = glm::mat4_cast(rotation);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
		return T * R * S;
	}
}