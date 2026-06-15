#include "assets/GLTFLoader.h"

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <glm/gtx/quaternion.hpp>

namespace
{
	glm::vec3 ToGlm(const fastgltf::math::fvec3& value)
	{
		return { value.x(), value.y(), value.z() };
	}

	glm::vec4 ToGlm(const fastgltf::math::nvec4& value)
	{
		return { value[0], value[1], value[2], value[3] };
	}

	AlphaMode ToAlphaMode(fastgltf::AlphaMode alphaMode)
	{
		switch (alphaMode) {
		case fastgltf::AlphaMode::Mask:
			return AlphaMode::Mask;
		case fastgltf::AlphaMode::Blend:
			return AlphaMode::Blend;
		case fastgltf::AlphaMode::Opaque:
		default:
			return AlphaMode::Opaque;
		}
	}

	glm::mat4 ToGlmTransform(const fastgltf::Node& node)
	{
		return std::visit(fastgltf::visitor{
			[](const fastgltf::TRS& trs) {
				const glm::vec3 translation{ trs.translation.x(), trs.translation.y(), trs.translation.z() };
				const glm::quat rotation{ trs.rotation.w(), trs.rotation.x(), trs.rotation.y(), trs.rotation.z() };
				const glm::vec3 scale{ trs.scale.x(), trs.scale.y(), trs.scale.z() };

				return glm::translate(glm::mat4{ 1.0f }, translation)
					* glm::mat4_cast(rotation)
					* glm::scale(glm::mat4{ 1.0f }, scale);
			},
			[](const fastgltf::math::fmat4x4& matrix) {
				return glm::make_mat4(matrix.data());
			},
		}, node.transform);
	}
}

std::optional<std::shared_ptr<LoadedGLTF>> GLTFLoader::Load(const std::filesystem::path& filePath) const
{
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "GLTF file does not exist: " << filePath << '\n';
		return std::nullopt;
	}

	fastgltf::Parser parser{};

	constexpr auto options =
		fastgltf::Options::DontRequireValidAssetMember |
		fastgltf::Options::AllowDouble |
		fastgltf::Options::LoadGLBBuffers |
		fastgltf::Options::LoadExternalBuffers |
		fastgltf::Options::GenerateMeshIndices;

	auto gltfFile = fastgltf::MappedGltfFile::FromPath(filePath);
	if (!gltfFile) {
		std::cerr << "Failed to map glTF file: " << fastgltf::getErrorMessage(gltfFile.error()) << '\n';
		return std::nullopt;
	}

	auto assetResult = parser.loadGltf(gltfFile.get(), filePath.parent_path(), options);
	if (assetResult.error() != fastgltf::Error::None) {
		std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(assetResult.error()) << '\n';
		return std::nullopt;
	}

	fastgltf::Asset asset = std::move(assetResult.get());
	auto loaded = std::make_shared<LoadedGLTF>();
	loaded->sourcePath = filePath;

	loaded->materials.reserve(asset.materials.size());
	for (const auto& material : asset.materials) {
		MaterialAsset outMaterial;
		outMaterial.name = std::string{ material.name };
		outMaterial.baseColorFactor = ToGlm(material.pbrData.baseColorFactor);
		outMaterial.metallicFactor = static_cast<float>(material.pbrData.metallicFactor);
		outMaterial.roughnessFactor = static_cast<float>(material.pbrData.roughnessFactor);
		outMaterial.alphaCutoff = static_cast<float>(material.alphaCutoff);
		outMaterial.alphaMode = ToAlphaMode(material.alphaMode);

		if (material.pbrData.baseColorTexture.has_value()) {
			outMaterial.baseColorTexture = static_cast<uint32_t>(material.pbrData.baseColorTexture->textureIndex);
		}

		loaded->materials.push_back(outMaterial);
	}

	loaded->images.reserve(asset.images.size());
	for (const auto& image : asset.images) {
		ImageAsset outImage;
		outImage.name = std::string{ image.name };
		loaded->images.push_back(std::move(outImage));
	}

	loaded->meshes.reserve(asset.meshes.size());
	for (const auto& mesh : asset.meshes) {
		auto outMesh = std::make_shared<MeshAsset>();
		outMesh->name = std::string{ mesh.name };

		for (const auto& primitive : mesh.primitives) {
			const auto positionIt = primitive.findAttribute("POSITION");
			if (positionIt == primitive.attributes.end() || !primitive.indicesAccessor.has_value()) {
				continue;
			}

			const auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
			const uint32_t firstIndex = static_cast<uint32_t>(outMesh->indices.size());
			const size_t firstVertex = outMesh->vertices.size();

			outMesh->vertices.resize(firstVertex + positionAccessor.count);

			const auto& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];
			fastgltf::iterateAccessor<uint32_t>(asset, indexAccessor, [&](uint32_t index) {
				outMesh->indices.push_back(index + static_cast<uint32_t>(firstVertex));
			});

			fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
				asset,
				positionAccessor,
				[&](const fastgltf::math::fvec3& position, size_t index) {
					outMesh->vertices[firstVertex + index].position = ToGlm(position);
				});

			if (const auto normalIt = primitive.findAttribute("NORMAL"); normalIt != primitive.attributes.end()) {
				const auto& normalAccessor = asset.accessors[normalIt->accessorIndex];
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
					asset,
					normalAccessor,
					[&](const fastgltf::math::fvec3& normal, size_t index) {
						outMesh->vertices[firstVertex + index].normal = ToGlm(normal);
					});
			}

			if (const auto uvIt = primitive.findAttribute("TEXCOORD_0"); uvIt != primitive.attributes.end()) {
				const auto& uvAccessor = asset.accessors[uvIt->accessorIndex];
				fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
					asset,
					uvAccessor,
					[&](const fastgltf::math::fvec2& uv, size_t index) {
						outMesh->vertices[firstVertex + index].uv_x = uv.x();
						outMesh->vertices[firstVertex + index].uv_y = uv.y();
					});
			}

			outMesh->surfaces.push_back(MeshSurface{
				.firstIndex = firstIndex,
				.indexCount = static_cast<uint32_t>(indexAccessor.count),
				.materialIndex = primitive.materialIndex
					? std::optional<uint32_t>{ static_cast<uint32_t>(*primitive.materialIndex) }
					: std::nullopt,
			});
		}

		loaded->meshes.push_back(std::move(outMesh));
	}

	std::vector<std::shared_ptr<SceneNode>> nodes(asset.nodes.size());
	std::function<std::shared_ptr<SceneNode>(size_t)> buildNode = [&](size_t nodeIndex) -> std::shared_ptr<SceneNode> {
		if (nodes[nodeIndex]) {
			return nodes[nodeIndex];
		}

		const auto& gltfNode = asset.nodes[nodeIndex];
		std::shared_ptr<SceneNode> node;

		if (gltfNode.meshIndex.has_value() && *gltfNode.meshIndex < loaded->meshes.size()) {
			auto meshNode = std::make_shared<MeshNode>();
			meshNode->mesh = loaded->meshes[*gltfNode.meshIndex];
			node = meshNode;
		} else {
			node = std::make_shared<SceneNode>();
		}

		node->name = std::string{ gltfNode.name };
		node->localTransform = ToGlmTransform(gltfNode);
		nodes[nodeIndex] = node;

		for (const auto childIndex : gltfNode.children) {
			node->AddChild(buildNode(childIndex));
		}

		return node;
	};

	if (!asset.scenes.empty()) {
		const size_t sceneIndex = asset.defaultScene.value_or(0);
		const auto& scene = asset.scenes[sceneIndex];
		loaded->topNodes.reserve(scene.nodeIndices.size());

		for (const auto nodeIndex : scene.nodeIndices) {
			loaded->topNodes.push_back(buildNode(nodeIndex));
		}
	}

	for (const auto& topNode : loaded->topNodes) {
		topNode->RefreshTransform(glm::mat4{ 1.0f });
	}

	return loaded;
}
