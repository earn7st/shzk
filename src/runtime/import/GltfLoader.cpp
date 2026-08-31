#include "GltfLoader.h"
#include "runtime/asset/Model.h"
#include "runtime/asset/Texture.h"
#include "runtime/asset/Material.h"
#include "runtime/core/Primitive.h"
#include "runtime/log/Log.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/render/resources/VertexFactory.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <glm/gtc/quaternion.hpp>
#include <filesystem>
#include <set>
#include <mikktspace.h>

namespace shzk
{
    namespace
    {
        struct MikktUserData
        {
            const std::vector<glm::vec3>* positions;
            const std::vector<glm::vec3>* normals;
            const std::vector<glm::vec2>* texcoords;
            const std::vector<uint32_t>* indices;      // triangle list
            std::vector<glm::vec4>* tangentsOut;       // unindexed, size = indices.size()
        };

        int GetNumFaces(const SMikkTSpaceContext* ctx)
        {
            auto* ud = static_cast<MikktUserData*>(ctx->m_pUserData);
            return (int)(ud->indices->size() / 3);
        }

        int GetNumVerticesOfFace(const SMikkTSpaceContext*, int) { return 3; }

        void GetPosition(const SMikkTSpaceContext* ctx, float out[], int face, int vert)
        {
            auto* ud = static_cast<MikktUserData*>(ctx->m_pUserData);
            uint32_t idx = (*ud->indices)[face * 3 + vert];
            const glm::vec3& p = (*ud->positions)[idx];
            out[0] = p.x; out[1] = p.y; out[2] = p.z;
        }

        void GetNormal(const SMikkTSpaceContext* ctx, float out[], int face, int vert)
        {
            auto* ud = static_cast<MikktUserData*>(ctx->m_pUserData);
            uint32_t idx = (*ud->indices)[face * 3 + vert];
            const glm::vec3& n = (*ud->normals)[idx];
            out[0] = n.x; out[1] = n.y; out[2] = n.z;
        }

        void GetTexCoord(const SMikkTSpaceContext* ctx, float out[], int face, int vert)
        {
            auto* ud = static_cast<MikktUserData*>(ctx->m_pUserData);
            uint32_t idx = (*ud->indices)[face * 3 + vert];
            const glm::vec2& uv = (*ud->texcoords)[idx];
            out[0] = uv.x; out[1] = uv.y;
        }

        void SetTSpaceBasic(const SMikkTSpaceContext* ctx, const float tangent[], const float sign, int face, int vert)
        {
            auto* ud = static_cast<MikktUserData*>(ctx->m_pUserData);
            (*ud->tangentsOut)[face * 3 + vert] = glm::vec4(tangent[0], tangent[1], tangent[2], sign);
        }

        std::vector<glm::vec4> GenerateTangents(
            const std::vector<glm::vec3>& positions,
            const std::vector<glm::vec3>& normals,
            const std::vector<glm::vec2>& texcoords,
            const std::vector<uint32_t>& indices)
        {
            std::vector<glm::vec4> result(positions.size(), glm::vec4(0.f, 0.f, 0.f, 1.f));
            if (indices.empty() || positions.empty() || normals.empty() || texcoords.empty())
                return result;

            std::vector<glm::vec4> unindexed(indices.size());

            MikktUserData ud{};
            ud.positions = &positions;
            ud.normals = &normals;
            ud.texcoords = &texcoords;
            ud.indices = &indices;
            ud.tangentsOut = &unindexed;

            SMikkTSpaceInterface iface{};
            iface.m_getNumFaces = GetNumFaces;
            iface.m_getNumVerticesOfFace = GetNumVerticesOfFace;
            iface.m_getPosition = GetPosition;
            iface.m_getNormal = GetNormal;
            iface.m_getTexCoord = GetTexCoord;
            iface.m_setTSpaceBasic = SetTSpaceBasic;
            iface.m_setTSpace = nullptr;   // normal mapping ÓÃ Basic ¾Í¹»

            SMikkTSpaceContext ctx{};
            ctx.m_pInterface = &iface;
            ctx.m_pUserData = &ud;

            if (!genTangSpaceDefault(&ctx))
                return result;

            for (size_t i = 0; i < indices.size(); ++i)
                result[indices[i]] = unindexed[i];

            return result;
        }
    }

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

        // Textures
        result.textures.resize(gltf.textures.size());
        std::vector<size_t> sRGBTextureIndices;
        std::vector<size_t> linearTextureIndices;
        for (auto& mat : gltf.materials)
        {
            if (mat.pbrData.baseColorTexture)
                sRGBTextureIndices.push_back(mat.pbrData.baseColorTexture->textureIndex);
            if (mat.emissiveTexture)
                sRGBTextureIndices.push_back(mat.emissiveTexture->textureIndex);
            if (mat.normalTexture)
                linearTextureIndices.push_back(mat.normalTexture->textureIndex);
            if (mat.pbrData.metallicRoughnessTexture)
                linearTextureIndices.push_back(mat.pbrData.metallicRoughnessTexture->textureIndex);
            if (mat.occlusionTexture)
                linearTextureIndices.push_back(mat.occlusionTexture->textureIndex);
        }
        for (auto& idx : sRGBTextureIndices)
        {
            result.textures[idx] = CreateTexture(gltf, gltf.textures[idx], RHIFormat::FORMAT_R8G8B8A8_SRGB);
            if (result.textures[idx]) SHZK_LOG_INFO("  Texture[{}] loaded: {}", idx, result.textures[idx]->GetName());
        }
        for (auto& idx : linearTextureIndices)
        {
            result.textures[idx] = CreateTexture(gltf, gltf.textures[idx], RHIFormat::FORMAT_R8G8B8A8_UNORM);
            if (result.textures[idx]) SHZK_LOG_INFO("  Texture[{}] loaded: {}", idx, result.textures[idx]->GetName());
        }

        // Materials
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

                auto indices = ReadIndices(gltf, gltfPrimitive);

                bool hasTangent = (FindAttributeAccessor(gltf, gltfPrimitive, "TANGENT") != nullptr);
                if (hasTangent)
                    prim->tangent = ReadTangents(gltf, gltfPrimitive);
                else if (!prim->normal.empty() && !prim->texcoord.empty() && !indices.empty())
                    prim->tangent = GenerateTangents(prim->position, prim->normal, prim->texcoord, indices);

                submesh.primitive = prim;

                // VertexBuffer, IndexBuffer, VertexFactory
                submesh.interleavedBuffer = std::make_shared<VertexBuffer>(submesh.primitive);
                submesh.indexBuffer = std::make_shared<IndexBuffer>(indices);

                submesh.vertexFactory = std::make_shared<InterleavedVertexFactory>();
                submesh.vertexFactory->InitRHIDeclaration();    // fixed RHIVertexDeclaration, check VertexFactory.cpp
                                                                // TODO: RHIDeclaration Cache
                VertexFactory::VertexStream stream{};
                stream.vertexBuffer = submesh.interleavedBuffer;
                stream.offset = 0;
                stream.stride = submesh.interleavedBuffer->GetStride();
                submesh.vertexFactory->AddVertexStream(stream);
                
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
                result.SetTranslation(
                    glm::vec3(
                    transform.translation.x(),
                    transform.translation.y(),
                    transform.translation.z()));

                const auto& q = transform.rotation;
                glm::quat glmQuat(q.w(), q.x(), q.y(), q.z());
                result.SetRotation(glmQuat);

                result.SetScale(
                    glm::vec3(
                    transform.scale.x(),
                    transform.scale.y(),
                    transform.scale.z()));
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

    std::vector<glm::vec4>	GltfLoader::ReadTangents(const fastgltf::Asset& gltf, const fastgltf::Primitive& primitive)
    {
        const auto* accessor = FindAttributeAccessor(gltf, primitive, "TANGENT");
        if (!accessor) return {};

        return ReadAccessorData<glm::vec4, fastgltf::math::fvec4>(gltf, *accessor);
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

    std::shared_ptr<Texture> GltfLoader::CreateTexture(const fastgltf::Asset& gltf, const fastgltf::Texture& texture, RHIFormat format)
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

        auto tex = std::make_shared<Texture>(texturePath, TextureType::Type2D, format);
        return tex;
    }

    std::shared_ptr<Material> GltfLoader::CreateMaterial(const fastgltf::Asset& gltf, const fastgltf::Material& material, const std::vector<std::shared_ptr<Texture>>& textures)
    {
        auto mat = std::make_shared<Material>();
        mat->InitRenderResources();

        mat->SetBaseColor(glm::vec4(
            material.pbrData.baseColorFactor.x(),
            material.pbrData.baseColorFactor.y(),
            material.pbrData.baseColorFactor.z(),
            material.pbrData.baseColorFactor.w()));
        mat->SetMetallic(material.pbrData.metallicFactor);
        mat->SetRoughness(material.pbrData.roughnessFactor);

        mat->SetEmission(glm::vec3(
            material.emissiveFactor.x(),
            material.emissiveFactor.y(),
            material.emissiveFactor.z()));

        mat->SetAlphaCutoff(material.alphaCutoff);

        auto getTexture = [&](const auto& optionalTexInfo) -> std::shared_ptr<Texture> {
            if (!optionalTexInfo.has_value())
                return nullptr;
            size_t idx = optionalTexInfo->textureIndex;
            if (idx >= textures.size())
                return nullptr;
            return textures[idx];
            };

        mat->SetTextureBaseColor(getTexture(material.pbrData.baseColorTexture));
        mat->SetTextureArm(getTexture(material.pbrData.metallicRoughnessTexture));
        mat->SetTextureNormal(getTexture(material.normalTexture));
        mat->SetTextureOcclusion(getTexture(material.occlusionTexture));
        mat->SetTextureEmissive(getTexture(material.emissiveTexture));
        
        // general slots
        mat->SetIntSlot(0, static_cast<int32_t>(material.alphaMode));  // 0=Opaque, 1=Mask, 2=Blend
        mat->SetIntSlot(1, material.doubleSided ? 1 : 0);
        mat->SetIntSlot(2, material.unlit ? 1 : 0);
        mat->SetFloatSlot(0, (material.normalTexture.has_value())
            ? material.normalTexture->scale         // glTF normal scale
            : 1.0f);
        mat->SetFloatSlot(1, (material.occlusionTexture.has_value())
            ? material.occlusionTexture->strength   // glTF occlusion strength
            : 1.0f);

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