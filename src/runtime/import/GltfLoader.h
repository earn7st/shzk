#pragma once

#include "runtime/core/Definitions.h"

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

	class GltfLoader
	{
	public:
		GltfLoader() = default;
		void Load(std::string path, GltfLoadResult& result);

		Transform	ReadTransform(fastgltf::Node& node);
		std::vector<glm::vec3> ReadPositions(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);
		std::vector<glm::vec3> ReadNormals(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);
		std::vector<glm::vec2> ReadTexcoords(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive);

		std::shared_ptr<Texture>	CreateTexture(const fastgltf::Asset& gltf, const fastgltf::Texture& texture);
		std::shared_ptr<Material>	CreateMaterial(const fastgltf::Asset& gltf, const fastgltf::Material& material);

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