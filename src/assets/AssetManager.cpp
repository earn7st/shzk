#include "assets/AssetManager.h"

std::shared_ptr<LoadedGLTF> AssetManager::LoadGLTF(const std::string& name, const std::filesystem::path& filePath)
{
	if (const auto existing = GetGLTF(name)) {
		return existing;
	}

	auto loaded = _gltfLoader.Load(filePath);
	if (!loaded) {
		return nullptr;
	}

	_gltfAssets[name] = *loaded;
	return *loaded;
}

std::shared_ptr<LoadedGLTF> AssetManager::GetGLTF(const std::string& name) const
{
	const auto it = _gltfAssets.find(name);
	if (it == _gltfAssets.end()) {
		return nullptr;
	}

	return it->second;
}

void AssetManager::Clear()
{
	_gltfAssets.clear();
}
