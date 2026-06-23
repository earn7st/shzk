#pragma once

#include <vector>

#include "scene/Node.h"

class Scene
{
public:
	std::vector<Node> nodes;

	std::vector<uint32_t> rootNodes;
};