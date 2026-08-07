#include "AssetManager.h"

#include "runtime/import/GltfLoader.h"

namespace shzk
{
	std::shared_ptr<AssetManager> AssetManager::g_assetManager = std::make_shared<AssetManager>();

	std::shared_ptr<Asset> AssetManager::GetOrLoadAssetInternal(std::string name, std::string path)
	{
		if (m_assets.find(name) != m_assets.end())
		{
			return m_assets[name];
		}

		// TODO: load asset from path, probably not gonna be used for a while
		return nullptr;
	}

}