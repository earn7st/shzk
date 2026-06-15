#pragma once

#include "VKCommon.h"
#include "assets/AssetTypes.h"
#include "scene/Scene.h"

struct LoadedGLTF
{
	std::filesystem::path sourcePath;
	std::vector<std::shared_ptr<MeshAsset>> meshes;
	std::vector<MaterialAsset> materials;
	std::vector<ImageAsset> images;
	std::vector<std::shared_ptr<SceneNode>> topNodes;
};

class GLTFLoader
{
public:
	std::optional<std::shared_ptr<LoadedGLTF>> Load(const std::filesystem::path& filePath) const;
};
