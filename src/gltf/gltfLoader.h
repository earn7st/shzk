#pragma once

#include <string>
#include <fastgltf/core.hpp>
#include <glm/glm.hpp>

class Scene;
class ResourceManager;
struct VulkanContext;
struct UploadContext;
class DescriptorAllocator;

class gltfLoader
{
public:
	gltfLoader(
		VulkanContext* vulkanContext, 
		UploadContext* uploadContext, 
		DescriptorAllocator* descAllocator,
		ResourceManager* resourceManager);

	bool LoadGltf(Scene* scene, const std::string& filePath);
	

private:
	fastgltf::Asset _asset;
	VulkanContext* _vulkanContext;
	UploadContext* _uploadContext;
	DescriptorAllocator* _descAllocator;
	ResourceManager* _resourceManager;

	void LoadImage(fastgltf::Image& image);
	void LoadMaterial(fastgltf::Material& material);
	void LoadMesh(fastgltf::Mesh& mesh);
	void LoadCamera(fastgltf::Camera& camera);
	void LoadDefaultScene(Scene& scene);
};