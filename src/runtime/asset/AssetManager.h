#pragma once

#include <memory>
#include <unordered_map>
#include <string>

namespace shzk
{
	class Asset;
	struct GltfLoadResult;

	class AssetManager
	{
	private:
		static std::shared_ptr<AssetManager> g_assetManager;

	public:
		AssetManager() = default;
		~AssetManager() = default;

		static std::shared_ptr<AssetManager>& Get() { return g_assetManager; }


		void Init();
		void Shutdown();

		void ProcessGltfLoadResult(const GltfLoadResult& result);
		void Register(const std::shared_ptr<Asset>& asset);

		// Debug
		void PrintAllAssets();
		
	private:
		std::shared_ptr<Asset> GetOrLoadAssetInternal(std::string name, std::string path);

	private:
		std::unordered_map<std::string, std::shared_ptr<Asset>> m_assets;	// !! Name As Main Key (for now)
	};
}