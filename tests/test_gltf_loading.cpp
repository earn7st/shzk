#include <iostream>

#include "application/Application.h"
#include "gltf/gltfLoader.h"
#include "render/Renderer.h"
#include "resource/ResourceManager.h"
#include "scene/Scene.h"
#include "vulkan/VulkanContext.h"


int main()
{
	const std::string filePath = "C:/Users/earn/workspace/glTF-Sample-Assets/Models/Sponza/glTF/Sponza.gltf";
	// const std::string filePath = "C:/Users/earn/workspace/vkR/assets/DamagedHelmet/glTF/DamagedHelmet.gltf";

	SDLWindow window("vkR");
	Renderer renderer(window);

	ResourceManager resourceManager;
	Scene scene;

	gltfLoader loader(renderer.GetVulkanContext(), renderer.GetUpladContext(), &resourceManager);
	loader.LoadGltf(&scene, filePath);

	auto& meshes = resourceManager.meshes;
	std::cout << "Mesh Count: " << meshes.size() << std::endl;
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		auto& mesh = *it;
		std::cout << "Mesh Name: " << mesh.name << std::endl;
		std::cout << "Primitive Count: " << mesh.primitives.size() << std::endl;
	}

	while (1) {}

	return 0;
}