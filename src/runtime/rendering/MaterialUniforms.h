#pragma once

#include <glm/glm.hpp>

// Matches Binding 0 in defaultMaterialDescSet;
struct MaterialUniforms
{
	glm::vec4 baseColorFactor;	// 16
	float metallicFactor;		// 20
	float roughnessFactor;		// 24
	float normalScale;			// 28
	float occlusionStrength;	// 32
	glm::vec4 emissiveFactor;	// 48
	float emissiveStrength;		// 52
	float alphaCutoff;			// 56
	int32_t alphaMode;			//60
	int32_t padding = 0;
};

static_assert(sizeof(MaterialUniforms) == 64, "MaterialUniforms must match shader layout");