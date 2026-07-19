#pragma once

#include <string>
#include <fastgltf/core.hpp>

namespace vkR
{
	class Scene;

	class gltfLoader
	{
	public:
		static bool LoadAndBuildScene(const std::string& filePath, Scene* scene);

	};
}