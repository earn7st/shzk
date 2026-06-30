#pragma once

#include <string>
#include <fastgltf/core.hpp>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

class Scene;
class ResourceManager;
struct VulkanContext;
struct UploadContext;
class Renderer;

#undef LoadImage

class gltfLoader
{
public:
	gltfLoader(
		VulkanContext* vulkanContext, 
		UploadContext* uploadContext, 
		Renderer* renderer,
		ResourceManager* resourceManager);

	bool LoadGltf(Scene* scene, const std::string& filePath);
	

private:
	fastgltf::Asset _asset;
	VulkanContext* _vulkanContext;
	UploadContext* _uploadContext;
	Renderer* _renderer;
	ResourceManager* _resourceManager;

	struct SamplerDef
	{
		vk::Filter	magFilter = vk::Filter::eLinear;
		vk::Filter	minFilter = vk::Filter::eLinear;
		vk::SamplerAddressMode wrapU = vk::SamplerAddressMode::eRepeat;
		vk::SamplerAddressMode wrapV = vk::SamplerAddressMode::eRepeat;
	};
	std::vector<SamplerDef> _samplerDefs;

	void LoadImage(fastgltf::Image& image);
	void LoadSamplers();
	void LoadTextures();
	void LoadMaterial(fastgltf::Material& material);
	void LoadMesh(fastgltf::Mesh& mesh);
	void LoadCamera(fastgltf::Camera& camera);
	void LoadDefaultScene(Scene& scene);

};