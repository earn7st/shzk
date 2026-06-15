#pragma once

#include "VKCommon.h"
#include "assets/GLTFLoader.h"

class AssetManager
{
public:
	std::shared_ptr<LoadedGLTF> LoadGLTF(const std::string& name, const std::filesystem::path& filePath);
	std::shared_ptr<LoadedGLTF> GetGLTF(const std::string& name) const;
	void Clear();

private:
	GLTFLoader _gltfLoader;
	std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> _gltfAssets;
};
