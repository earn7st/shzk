#pragma once

#include <string>
#include <fastgltf/core.hpp>
#include <glm/glm.hpp>

class Scene;
class ResourceManager;
struct VulkanContext;
struct UploadContext;

class gltfLoader
{
public:
	gltfLoader(VulkanContext* vulkanContext, UploadContext* uploadContext, ResourceManager* resourceManager);

	bool LoadGltf(Scene* scene, const std::string& filePath);
	void LoadImage(fastgltf::Image& image);
	void LoadMesh(fastgltf::Mesh& mesh);
	void LoadMaterial(fastgltf::Material& material);
	void LoadCamera(fastgltf::Camera& camera);

private:
	fastgltf::Asset _asset;
	VulkanContext* _vulkanContext;
	UploadContext* _uploadContext;
	ResourceManager* _resourceManager;
};