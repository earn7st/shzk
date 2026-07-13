#pragma once

#include <vector>

#include "resource/resource_types/Mesh.h"
#include "resource/resource_types/Geometry.h"
#include "resource/resource_types/Material.h"
#include "resource/resource_types/Texture.h"

struct VulkanContext;
struct UploadContext;
class DescriptorAllocator;
struct DescriptorSetLayout;

class ResourceManager
{
public:
	// CPU Side Resources
	std::vector<Mesh> meshes;
	
	// GPU Side Resources
	std::vector<Geometry> geometries;
	std::vector<Material> materials;
	std::vector<Texture> textures;

	// Default (Fallback) Resources
	Texture defaultWhiteTexture;
	Material defaultMaterial;	// Uses defaultWhiteTexture
};

namespace DefaultResourceInit
{
	void InitDefaultResources(
		VulkanContext* vulkanContext,
		UploadContext* uploadContext,
		DescriptorAllocator* descAllocator,
		const DescriptorSetLayout& materialLayout,
		ResourceManager* resourceManager);

	Material CreateDefaultMaterial(
		VulkanContext* vulkanContext,
		DescriptorAllocator* descAllocator,
		const DescriptorSetLayout& materialLayout,
		const Texture& fallbackTexture);

	Texture CreateDefaultTexture(VulkanContext* vulkanContext, UploadContext* uploadContext, uint32_t rgba);
}