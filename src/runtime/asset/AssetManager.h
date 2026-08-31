#pragma once

#include <memory>
#include <unordered_map>
#include <string>

namespace shzk
{
	class Asset;
	struct GltfLoadResult;

	class Texture;

	class AssetManager
	{
	private:
		static std::shared_ptr<AssetManager> g_assetManager;

		static std::shared_ptr<Texture> g_white1x1;
		static std::shared_ptr<Texture> g_black1x1;
		static std::shared_ptr<Texture> g_normal1x1;

	public:
		static std::shared_ptr<Texture> GetWhiteTexture1x1() { return g_white1x1; }
		static std::shared_ptr<Texture> GetBlackTexture1x1() { return g_black1x1; }
		static std::shared_ptr<Texture> GetNormalTexture1x1() { return g_normal1x1; }

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