#pragma once

#include "VKCommon.h"

enum class AlphaMode : uint8_t
{
	Opaque,
	Mask,
	Blend
};

struct MaterialAsset
{
	std::string name;
	glm::vec4 baseColorFactor{ 1.0f };
	float metallicFactor{ 1.0f };
	float roughnessFactor{ 1.0f };
	float alphaCutoff{ 0.5f };
	AlphaMode alphaMode{ AlphaMode::Opaque };
	std::optional<uint32_t> baseColorTexture;
};

struct ImageAsset
{
	std::string name;
	std::filesystem::path sourcePath;
	std::vector<std::byte> encodedData;
};

struct Vertex
{
	glm::vec3 position{ 0.0f };
	float uv_x{ 0.0f };
	glm::vec3 normal{ 0.0f, 1.0f, 0.0f };
	float uv_y{ 0.0f };
	glm::vec4 color{ 1.0f };
};

struct MeshSurface
{
	uint32_t firstIndex{ 0 };
	uint32_t indexCount{ 0 };
	std::optional<uint32_t> materialIndex;
};

struct MeshAsset
{
	std::string name;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<MeshSurface> surfaces;
};
