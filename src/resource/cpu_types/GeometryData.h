#pragma once

#include "resource/cpu_types/Vertex.h"

struct GeometryData
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};