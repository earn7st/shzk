#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace shzk
{

	/*
	Primitive: CPU data of a mesh
	(TODO) Should be serialized
	*/
	class Primitive
	{
	public:
		Primitive() = default;
		
		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normal;
		std::vector<glm::vec2> texcoord;
	};
}