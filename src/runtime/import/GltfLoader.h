#pragma once

#include "runtime/core/Transform.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <glm/glm.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

namespace shzk
{
	class Model;
	class Texture;
	class Material;
	class VertexBuffer;
	class IndexBuffer;

	struct GltfLoadResult
	{
		std::vector<std::shared_ptr<Model>> models;
		std::vector<std::shared_ptr<Texture>> textures;
		std::vector<std::shared_ptr<Material>> materials;
	};
	// GltfLoadResult is filled by GltfLoader::Load,
	// the content in it should be managed by AssetManager
	// Model/Texture/Material should be able to use already

	class GltfLoader
	{
	public:
		static std::shared_ptr<GltfLoader> g_gltfLoader;
		static std::shared_ptr<GltfLoader> Get() { return g_gltfLoader; }

	public:
		GltfLoader() = default;
		void Load(std::string path, GltfLoadResult& result);

		Transform	ReadTransform(fastgltf::Node& node);
		std::vector<glm::vec3>	ReadPositions(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);
		std::vector<glm::vec3>	ReadNormals(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);
		std::vector<glm::vec2>	ReadTexcoords(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);
		std::vector<glm::vec4>	ReadTangents(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);
		std::vector<uint32_t>	ReadIndices(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);

		std::shared_ptr<Texture>	CreateTexture(const fastgltf::Asset& gltf, const fastgltf::Texture& texture, RHIFormat format);
		std::shared_ptr<Material>	CreateMaterial(const fastgltf::Asset& gltf, const fastgltf::Material& material, const std::vector<std::shared_ptr<Texture>>& textures);

		// helpers
		const fastgltf::Accessor* FindAttributeAccessor(
			const fastgltf::Asset& gltf,
			const fastgltf::Primitive& primitive,
			const char* name);

		template <typename GlmVec, typename FastgltfVec>
		std::vector<GlmVec> ReadAccessorData(
			const fastgltf::Asset& gltf,
			const fastgltf::Accessor& accessor)
		{
			std::vector<GlmVec> result;
			result.reserve(accessor.count);

			fastgltf::iterateAccessor<FastgltfVec>(gltf, accessor,
				[&](const FastgltfVec& v) {
					if constexpr (std::is_same_v<GlmVec, glm::vec3>) {
						result.emplace_back(v.x(), v.y(), v.z());
					}
					else if constexpr (std::is_same_v<GlmVec, glm::vec2>) {
						result.emplace_back(v.x(), v.y());
					}
					else if constexpr (std::is_same_v<GlmVec, glm::vec4>) {
						result.emplace_back(v.x(), v.y(), v.z(), v.w());
					}
				});

			return result;
		}

	private:
		std::filesystem::path m_basePath;
	};
}