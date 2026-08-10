#include "GltfLoader.h"
#include "runtime/asset/Model.h"
#include "runtime/asset/Texture.h"
#include "runtime/asset/Material.h"
#include "runtime/core/Primitive.h"
#include "runtime/log/Log.h"
#include "runtime/render/resources/Buffer.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <glm/gtc/quaternion.hpp>
#include <filesystem>
#include <iostream>

namespace shzk
{
	void GltfLoader::Load(std::string path, GltfLoadResult& result)
	{
        m_basePath = std::filesystem::path(path).parent_path();

		fastgltf::Parser parser;

		auto data = fastgltf::GltfDataBuffer::FromPath(path);
        auto assetResult = parser.loadGltf(
            data.get(), m_basePath, fastgltf::Options::LoadExternalBuffers);

        if (assetResult.error() != fastgltf::Error::None) {
            SHZK_LOG_ERROR("Failed to parse glTF: {} ¡ª error code: {}",
                path, static_cast<int>(assetResult.error()));
            return;
        }

        auto& gltf = assetResult.get();

        result.textures.resize(gltf.textures.size());
        for (size_t i = 0; i < gltf.textures.size(); ++i)
        {
            result.textures[i] = CreateTexture(gltf, gltf.textures[i]);
            if (result.textures[i])
            {
                SHZK_LOG_INFO("  Texture[{}] loaded: {}", i, result.textures[i]->GetName());
            }
        }

        result.materials.resize(gltf.materials.size());
        for (size_t i = 0; i < gltf.materials.size(); ++i)
        {
            auto& material = gltf.materials[i];
            result.materials[i] = CreateMaterial(gltf, material, result.textures);
            if (!material.name.empty())
                result.materials[i]->SetName(std::string(material.name));
            else
                result.materials[i]->SetName("Material_" + std::to_string(uint32_t(i)));
            if (result.materials[i])
            {
                SHZK_LOG_INFO("  Material[{}] loaded: {}", i, result.materials[i]->GetName());
            }
        }

        auto& scene = gltf.scenes[gltf.defaultScene.value_or(0)];

        for (size_t nodeIdx : scene.nodeIndices)
        {
            auto& node = gltf.nodes[nodeIdx];
            if (!node.meshIndex.has_value())
                continue;   // skip cameras and lights

            auto model = std::make_shared<Model>();
            model->SetName(node.name.c_str());

            auto& mesh = gltf.meshes[node.meshIndex.value()];
            for (size_t primIdx = 0; primIdx < mesh.primitives.size(); ++primIdx)
            {
                auto& gltfPrimitive = mesh.primitives[primIdx];

                Submesh submesh;
                // Transform
                submesh.localTransform = ReadTransform(node);
                // Primitive
                auto prim = std::make_shared<Primitive>();
                prim->position = ReadPositions(gltf, gltfPrimitive);
                prim->normal = ReadNormals(gltf, gltfPrimitive);
                prim->texcoord = ReadTexcoords(gltf, gltfPrimitive);
                submesh.primitive = prim;
                // VertexBuffer, IndexBuffer
                submesh.vertexBuffer = std::make_shared<VertexBuffer>(submesh.primitive);
                auto indices = ReadIndices(gltf, gltfPrimitive);
                submesh.indexBuffer = std::make_shared<IndexBuffer>(indices);
                // Material
                submesh.material = gltfPrimitive.materialIndex.has_value()
                    ? result.materials[gltfPrimitive.materialIndex.value()]
                    : nullptr;

                model->AddSubmesh(std::move(submesh));
            }

            result.models.push_back(model);
            SHZK_LOG_INFO("Model loaded: {} ({} submeshes)",
                model->GetName(), model->GetSubmeshes().size());
        }

        SHZK_LOG_INFO("glTF load complete: {} models, {} textures, {} materials",
            result.models.size(),
            result.textures.size(),
            result.materials.size());
	}

    Transform GltfLoader::ReadTransform(fastgltf::Node& node)
    {
        Transform result{};

        std::visit([&result](auto&& transform) {
            using T = std::decay_t<decltype(transform)>;

            if constexpr (std::is_same_v<T, fastgltf::TRS>)
            {
                result.translation = glm::vec3(
                    transform.translation.x(),
                    transform.translation.y(),
                    transform.translation.z());

                const auto& q = transform.rotation;
                glm::quat glmQuat(q.w(), q.x(), q.y(), q.z());
                result.rotation = glmQuat;

                result.scale = glm::vec3(
                    transform.scale.x(),
                    transform.scale.y(),
                    transform.scale.z());
            }
            else if constexpr (std::is_same_v<T, fastgltf::math::fmat4x4>)
            {
                // TODO: mat4x4
            }
            }, node.transform);

        return result;
    }

    std::vector<glm::vec3> GltfLoader::ReadPositions(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive)
    {
        const auto* accessor = FindAttributeAccessor(gltf, primitive, "POSITION");
        if (!accessor) return {};

        return ReadAccessorData<glm::vec3, fastgltf::math::fvec3>(gltf, *accessor);
    }

    std::vector<glm::vec3> GltfLoader::ReadNormals(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive)
    {
        const auto* accessor = FindAttributeAccessor(gltf, primitive, "NORMAL");
        if (!accessor) return {};

        return ReadAccessorData<glm::vec3, fastgltf::math::fvec3>(gltf, *accessor);
    }

    std::vector<glm::vec2> GltfLoader::ReadTexcoords(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive)
    {
        const auto* accessor = FindAttributeAccessor(gltf, primitive, "TEXCOORD_0");
        if (!accessor) return {};
        
        return ReadAccessorData<glm::vec2, fastgltf::math::fvec2>(gltf, *accessor);
    }

    std::vector<uint32_t> GltfLoader::ReadIndices(
        const fastgltf::Asset& gltf,
        const fastgltf::Primitive& primitive)
    {
        if (!primitive.indicesAccessor.has_value())
            return {};

        const auto& accessor = gltf.accessors[primitive.indicesAccessor.value()];

        std::vector<uint32_t> result;
        result.reserve(accessor.count);
        
        // all cast to uint32_t
        switch (accessor.componentType)
        {
        case fastgltf::ComponentType::UnsignedInt:
            fastgltf::iterateAccessor<uint32_t>(gltf, accessor,
                [&](uint32_t idx) { result.push_back(idx); });
            break;

        case fastgltf::ComponentType::UnsignedShort:
            fastgltf::iterateAccessor<uint16_t>(gltf, accessor,
                [&](uint16_t idx) { result.push_back(static_cast<uint32_t>(idx)); });
            break;

        case fastgltf::ComponentType::UnsignedByte:
            fastgltf::iterateAccessor<uint8_t>(gltf, accessor,
                [&](uint8_t idx) { result.push_back(static_cast<uint32_t>(idx)); });
            break;

        default:
            SHZK_LOG_ERROR("Unsupported index component type");
            return {};
        }

        return result;
    }

    std::shared_ptr<Texture> GltfLoader::CreateTexture(const fastgltf::Asset& gltf, const fastgltf::Texture& texture)
    {
        std::string texturePath;

        if (texture.imageIndex.has_value())
        {
            auto& image = gltf.images[texture.imageIndex.value()];

            if (auto* uri = std::get_if<fastgltf::sources::URI>(&image.data))
            {
                texturePath = (m_basePath / uri->uri.fspath()).generic_string();
                SHZK_LOG_INFO("  URI: {}", texturePath);
            }
            // TODO: BufferView, Array... 
        }

        if (texturePath.empty())
        {
            SHZK_LOG_WARN("Texture has no file path, skipping");
            return nullptr;
        }

        auto tex = std::make_shared<Texture>(texturePath, TextureType::Type2D);
        return tex;
    }

    std::shared_ptr<Material> GltfLoader::CreateMaterial(const fastgltf::Asset& gltf, const fastgltf::Material& material, const std::vector<std::shared_ptr<Texture>>& textures)
    {
        auto mat = std::make_shared<Material>();

        mat->m_baseColor = glm::vec4(
            material.pbrData.baseColorFactor.x(),
            material.pbrData.baseColorFactor.y(),
            material.pbrData.baseColorFactor.z(),
            material.pbrData.baseColorFactor.w());
        mat->m_metallic = material.pbrData.metallicFactor;
        mat->m_roughness = material.pbrData.roughnessFactor;

        mat->m_emission = glm::vec3(
            material.emissiveFactor.x(),
            material.emissiveFactor.y(),
            material.emissiveFactor.z());

        mat->m_alphaCutoff = material.alphaCutoff;

        auto getTexture = [&](const auto& optionalTexInfo) -> std::shared_ptr<Texture> {
            if (!optionalTexInfo.has_value())
                return nullptr;
            size_t idx = optionalTexInfo->textureIndex;
            if (idx >= textures.size())
                return nullptr;
            return textures[idx];
            };

        mat->m_textureDiffuse = getTexture(material.pbrData.baseColorTexture);
        mat->m_textureNormal = getTexture(material.normalTexture);
        mat->m_textureArm = getTexture(material.pbrData.metallicRoughnessTexture);
        mat->m_textureSpecular = nullptr;   // KHR_materials_pbrSpecularGlossiness

        // general slots
        mat->m_texture2D[0] = getTexture(material.occlusionTexture);  // glTF occlusion ¡ú 2D[0]
        mat->m_texture2D[1] = getTexture(material.emissiveTexture);   // glTF emissive ¡ú 2D[1]

        mat->m_ints[0] = static_cast<int32_t>(material.alphaMode);  // 0=Opaque, 1=Mask, 2=Blend
        mat->m_ints[1] = material.doubleSided ? 1 : 0;
        mat->m_ints[2] = material.unlit ? 1 : 0;
        mat->m_floats[0] = (material.normalTexture.has_value())
            ? material.normalTexture->scale         // glTF normal scale
            : 1.0f;
        mat->m_floats[1] = (material.occlusionTexture.has_value())
            ? material.occlusionTexture->strength   // glTF occlusion strength
            : 1.0f;

        return mat;
    }

    const fastgltf::Accessor* GltfLoader::FindAttributeAccessor(
        const fastgltf::Asset& gltf,
        const fastgltf::Primitive& primitive,
        const char* name)
    {
        auto it = std::find_if(
            primitive.attributes.begin(),
            primitive.attributes.end(),
            [name](const fastgltf::Attribute& attr) {
                return attr.name == name;
            });

        if (it == primitive.attributes.end())
            return nullptr;

        return &gltf.accessors[it->accessorIndex];
    }
}