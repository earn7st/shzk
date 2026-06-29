#pragma once

#include <vector>

#include "resource/resource_types/Mesh.h"
#include "resource/resource_types/Geometry.h"
#include "resource/resource_types/Material.h"
#include "resource/resource_types/Texture.h"

class ResourceManager
{
public:
	// CPU Side Resources
	std::vector<Mesh> meshes;
	
	// GPU Side Resources
	std::vector<Geometry> geometries;
	std::vector<Material> materials;
	std::vector<Texture> textures;

};