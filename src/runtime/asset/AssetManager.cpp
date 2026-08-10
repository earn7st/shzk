#include "AssetManager.h"
#include "Asset.h"
#include "Model.h"
#include "Texture.h"
#include "Material.h"
#include "runtime/import/GltfLoader.h"

#include <iostream>

namespace shzk
{
	std::shared_ptr<AssetManager> AssetManager::g_assetManager = std::make_shared<AssetManager>();

	void AssetManager::Init()
	{
	}

	void AssetManager::Shutdown()
	{
		m_assets.clear();
	}

	void AssetManager::ProcessGltfLoadResult(const GltfLoadResult& result)
	{
		for (auto& it : result.models)    Register(it);
		for (auto& it : result.textures)  Register(it);
		for (auto& it : result.materials) Register(it);
	}

	void AssetManager::Register(const std::shared_ptr<Asset>& asset)
	{
		m_assets[asset->GetName()] = asset;
	}

	std::shared_ptr<Asset> AssetManager::GetOrLoadAssetInternal(std::string name, std::string path)
	{
		if (m_assets.find(name) != m_assets.end())
		{
			return m_assets[name];
		}

		// TODO: load asset from path through AssetManager, probably not gonna be needed for a while
		return nullptr;
	}

	// Debug
	void AssetManager::PrintAllAssets()
	{
		std::cout << "--- AssetManager (" << m_assets.size() << " assets) ---" << std::endl;
		for (const auto& [name, asset] : m_assets)
		{
			std::cout << "  [" << asset->GetAssetTypeName() << "] " << name << std::endl;
		}
		std::cout << "---" << std::endl;
	}
}