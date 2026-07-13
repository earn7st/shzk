#pragma once

class ResourceManager;

struct Mesh;
struct MaterialInstance;
struct Geometry;
struct Material;
struct Texture;

template<typename T>
struct ResourceHandle
{
	uint32_t id = InvalidID;
	ResourceManager* resourceManager;

	bool IsValid() const { return (id != InvalidID); }

	static constexpr uint32_t InvalidID = UINT32_MAX;
};

using MeshHandle = ResourceHandle<Mesh>;
using GeometryHandle = ResourceHandle<Geometry>;
using MaterialHandle = ResourceHandle<Material>;
using TextureHandle = ResourceHandle<Texture>;