#include "gltfLoader.h"

#include <iostream>
#include <fmt/core.h>

#include <fastgltf/core.hpp>
#define FASTGLTF_USE_GLM
#include <fastgltf/tools.hpp>

#include "resource/cpu_types/GeometryData.h"
#include "resource/resource_types/Mesh.h"
#include "resource/resource_types/MaterialInstance.h"
#include "resource/resource_types/Geometry.h"
#include "resource/resource_types/Material.h"
#include "resource/resource_types/Texture.h"
#include "resource/ResourceManager.h"

#include "vulkan/VulkanHelpers.h"

#undef LoadImage

gltfLoader::gltfLoader(VulkanContext* vulkanContext, UploadContext* uploadContext, ResourceManager* resourceManager)
{
    _vulkanContext = vulkanContext;
    _uploadContext = uploadContext;
    _resourceManager = resourceManager;
}


bool gltfLoader::LoadGltf(Scene* scene, const std::string& filePath)
{
    if (_resourceManager == nullptr) {
        fmt::print("[Error] : ResourceManager is null in gltfLoader!\n");
        return false;
    }

    if (_vulkanContext == nullptr) {
        fmt::print("[Error] : VulkanContext is null in gltfLoader!\n");
        return false;
    }

    fmt::print("Loading GLTF: {}\n", filePath);

    static constexpr auto supportedExtensions =
        fastgltf::Extensions::KHR_mesh_quantization |
        fastgltf::Extensions::KHR_texture_transform |
        fastgltf::Extensions::KHR_materials_variants;

    fastgltf::Parser parser(supportedExtensions);

    constexpr auto gltfOptions =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadExternalBuffers |
        fastgltf::Options::LoadExternalImages |
        fastgltf::Options::GenerateMeshIndices;

    std::filesystem::path path = filePath;

    auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
    if (!bool(gltfFile)) {
        std::cerr << "Failed to open glTF file: " << fastgltf::getErrorMessage(gltfFile.error()) << '\n';
        return false;
    }

    auto asset = parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);
    if (asset.error() != fastgltf::Error::None) {
        std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(asset.error()) << '\n';
        return false;
    }

    _asset = std::move(asset.get());
    
    for (auto& image : _asset.images)
    {
        LoadImage(image);
    }

    for (auto& mesh : _asset.meshes)
    {
        LoadMesh(mesh);
    }

    for (auto& material : _asset.materials)
    {
        LoadMaterial(material);
    }
    
    return true;
}

void gltfLoader::LoadImage(fastgltf::Image& image)
{

}

void gltfLoader::LoadMesh(fastgltf::Mesh& mesh)
{
    Mesh outMesh = {};
    std::vector<Primitive>& outPrimitives = outMesh.primitives;
    outPrimitives.resize(mesh.primitives.size());

    outMesh.name = mesh.name;

    for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it)
    {
        assert(it->indicesAccessor.has_value());

        // ---- Build Vertices Data ----
        GeometryData geometryData;

        // Position
        auto* positionIt = it->findAttribute("POSITION");
        assert(positionIt != it->attributes.end());
        auto& positionAccessor = _asset.accessors[positionIt->accessorIndex];
        geometryData.vertices.resize(positionAccessor.count);
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
            _asset,
            positionAccessor,
            [&](fastgltf::math::fvec3 position, std::size_t index)
            {
                geometryData.vertices[index].position =
                    glm::vec3(
                        position.x(),
                        position.y(),
                        position.z());
            });

        // Normal
        auto* normalIt = it->findAttribute("NORMAL");
        assert(normalIt != it->attributes.end());
        auto& normalAccessor = _asset.accessors[normalIt->accessorIndex];
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
            _asset,
            normalAccessor,
            [&](fastgltf::math::fvec3 normal, std::size_t index)
            {
                geometryData.vertices[index].normal =
                    glm::vec3(
                        normal.x(),
                        normal.y(),
                        normal.z());
            });

        // UV
        auto* uvIt = it->findAttribute("TEXCOORD_0");
        assert(uvIt != it->attributes.end());
        auto& uvAccessor = _asset.accessors[uvIt->accessorIndex];
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
            _asset,
            uvAccessor,
            [&](fastgltf::math::fvec2 uv, std::size_t index)
            {
                geometryData.vertices[index].uv =
                    glm::vec2(
                        uv.x(),
                        uv.y());
            });

        // Tangent
        auto* tangentIt = it->findAttribute("TANGENT");
        if (tangentIt != it->attributes.end())
        {
            auto& tangentAccessor = _asset.accessors[tangentIt->accessorIndex];
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
                _asset,
                tangentAccessor,
                [&](fastgltf::math::fvec4 tangent, std::size_t index)
                {
                    geometryData.vertices[index].tangent =
                        glm::vec4(
                            tangent.x(),
                            tangent.y(),
                            tangent.z(),
                            tangent.w());
                });
        }

        // Indices
        auto& indexAccessor = _asset.accessors[it->indicesAccessor.value()];
        geometryData.indices.resize(indexAccessor.count);
        fastgltf::iterateAccessorWithIndex<uint32_t>(
            _asset,
            indexAccessor,
            [&](uint32_t indexValue, std::size_t index)
            {
                geometryData.indices[index] = indexValue;
            });

        Geometry outGeometry = CreateGeometry(*_vulkanContext, *_uploadContext, geometryData);
       
        auto& geometries = _resourceManager->geometries;
        geometries.push_back(outGeometry);

        ptrdiff_t index = std::distance(mesh.primitives.begin(), it);
        Primitive& outPrimitive = outMesh.primitives[index];
        outPrimitive.geometry = GeometryHandle{ static_cast<uint32_t>(geometries.size()) - 1 };
        outPrimitive.firstIndex = 0;
        outPrimitive.indexCount = indexAccessor.count;
        outPrimitive.topology = vk::PrimitiveTopology::eTriangleList;

    }

    auto& meshes = _resourceManager->meshes;
    meshes.push_back(outMesh);

    return;
}

void gltfLoader::LoadMaterial(fastgltf::Material& material)
{

}

void gltfLoader::LoadCamera(fastgltf::Camera& camera) 
{

}
