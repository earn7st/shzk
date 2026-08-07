#include "GltfLoader.h"
#include "runtime/asset/Model.h"
#include "runtime/asset/Texture.h"
#include "runtime/asset/Material.h"
#include "runtime/core/Primitive.h"
#include "runtime/log/Log.h"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <glm/gtc/quaternion.hpp>
#include <filesystem>

namespace shzk
{
	void GltfLoader::Load(std::string path, GltfLoadResult& result)
	{
		fastgltf::Parser parser;

		auto data = fastgltf::GltfDataBuffer::FromPath(path);
        m_basePath = std::filesystem::path(path).parent_path();
        auto assetResult = parser.loadGltf(
            data.get(), m_basePath, fastgltf::Options::None);

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
            result.materials[i] = CreateMaterial(gltf, gltf.materials[i]);
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
                auto& primitive = mesh.primitives[primIdx];

                Submesh submesh;
                submesh.localTransform = ReadTransform(node);

                auto prim = std::make_shared<Primitive>();
                prim->position = ReadPositions(gltf, primitive);
                prim->normal = ReadNormals(gltf, primitive);
                prim->texcoord = ReadTexcoords(gltf, primitive);
                submesh.primitive = prim;

                // submesh.vertexBuffer = std::make_shared<VertexBuffer>();

                // submesh.indexBuffer = std::make_shared<IndexBuffer>();

                // submesh.material

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
                /*
                result.translation = glm::vec3(
                    transform[3][0],
                    transform[3][1],
                    transform[3][2]);
                */
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

    std::shared_ptr<Texture> GltfLoader::CreateTexture(const fastgltf::Asset& gltf, const fastgltf::Texture& texture)
    {
        std::string texturePath;

        if (texture.imageIndex.has_value())
        {
            auto& image = gltf.images[texture.imageIndex.value()];

            std::visit(fastgltf::visitor{
                [](auto&) {},
                [&](fastgltf::sources::URI& source) {
                    auto relativePath = source.uri.path();  // source.uri.path() returns the relative path
                    texturePath = (m_basePath / std::filesystem::path(relativePath)).string();
                },
                }, image.data);
        }

        if (texturePath.empty())
        {
            SHZK_LOG_WARN("Texture has no file path, skipping");
            return nullptr;
        }

        auto tex = std::make_shared<Texture>(texturePath, TextureType::Type2D);
        return tex;
    }

    std::shared_ptr<Material> GltfLoader::CreateMaterial(const fastgltf::Asset& gltf, const fastgltf::Material& material)
    {
        return std::shared_ptr<Material>();
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