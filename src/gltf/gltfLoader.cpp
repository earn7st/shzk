#include "gltfLoader.h"

#include <iostream>
#include <fmt/core.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fastgltf/core.hpp>
#define FASTGLTF_USE_GLM
#include <fastgltf/tools.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "resource/cpu_types/GeometryData.h"
#include "resource/resource_types/Mesh.h"
#include "resource/resource_types/Geometry.h"
#include "resource/resource_types/Material.h"
#include "resource/resource_types/Texture.h"
#include "resource/ResourceManager.h"

#include "scene/Scene.h"
#include "scene/Node.h"

#include "vulkan/VulkanContext.h"
#include "vulkan/UploadContext.h"
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

    LoadDefaultScene(*scene);

    return true;
}

void gltfLoader::LoadImage(fastgltf::Image& image)
{
    const std::byte* imgData = nullptr;
    size_t           imgSize = 0;

    std::visit(
        [&](const auto& source)
        {
            using T = std::decay_t<decltype(source)>;

            if constexpr (requires { source.bytes; })
            {
                imgData = source.bytes.data();
                imgSize = source.bytes.size();
            }
            else if constexpr (std::is_same_v<T, fastgltf::sources::BufferView>)
            {
                const auto& view = _asset.bufferViews[source.bufferViewIndex];
                const auto& buffer = _asset.buffers[view.bufferIndex];

                std::visit(
                    [&](const auto& buf)
                    {
                        if constexpr (requires { buf.bytes; })
                        {
                            imgData = buf.bytes.data() + view.byteOffset;
                            imgSize = view.byteLength > 0
                                ? view.byteLength
                                : (buf.bytes.size() - view.byteOffset);
                        }
                    },
                    buffer.data);
            }
        },
        image.data);

    if (!imgData || imgSize == 0)
    {
        fmt::println("[Error] Failed to extract image data: {}", image.name);
        _resourceManager->textures.push_back({});
        return;
    }

    int  width, height, channels;
    stbi_uc* pixels = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(imgData),
        static_cast<int>(imgSize),
        &width, &height, &channels, 4);

    if (!pixels)
    {
        fmt::println("[Error] Failed to decode image: {}", image.name);
        _resourceManager->textures.push_back({});
        return;
    }

    vk::Device device = _vulkanContext->device;
    vk::Extent3D extent{ static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
    size_t dataSize = static_cast<size_t>(width) * height * 4;

    AllocatedBuffer staging = CreateBuffer(
        _vulkanContext->allocator,
        dataSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        VMA_MEMORY_USAGE_CPU_ONLY);
    memcpy(staging.info.pMappedData, pixels, dataSize);
    stbi_image_free(pixels);

    AllocatedImage gpuImage = CreateImage(
        _vulkanContext->allocator,
        extent,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        VMA_MEMORY_USAGE_GPU_ONLY);

    ImmediateSubmit(*_vulkanContext, *_uploadContext,
        [&](vk::CommandBuffer cmd)
        {
            TransitionImageLayout(
                cmd,
                gpuImage.image,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageAspectFlagBits::eColor);

            vk::BufferImageCopy copyRegion{};
            copyRegion.setBufferOffset(0)
                .setImageSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 })
                .setImageExtent(extent);
            cmd.copyBufferToImage(
                staging.buffer,
                gpuImage.image,
                vk::ImageLayout::eTransferDstOptimal,
                copyRegion);

            TransitionImageLayout(
                cmd,
                gpuImage.image,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::ImageAspectFlagBits::eColor);
        });

    DestroyBuffer(_vulkanContext->allocator, staging);

    vk::ImageView imageView = CreateImageView(
        device,
        gpuImage.image,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageAspectFlagBits::eColor);

    // Default Sampler: Linear, will be override by newly-created sampler when LoadMaterial()
    vk::Sampler defaultSampler = CreateSampler(
        device,
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat);

    Texture tex{ std::move(gpuImage), imageView, defaultSampler };
    _resourceManager->textures.push_back(std::move(tex));
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

        // Build Data
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

        Geometry outGeometry = CreateGeometry(
            *_vulkanContext,
            *_uploadContext,
            geometryData);

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
    Material outMat{};

    const auto& pbr = material.pbrData;
    outMat.baseColorFactor = glm::make_vec4(pbr.baseColorFactor.data());
    outMat.metallicFactor = pbr.metallicFactor;
    outMat.roughnessFactor = pbr.roughnessFactor;
    outMat.emissiveFactor = glm::make_vec3(material.emissiveFactor.data());
    outMat.emissiveStrength = material.emissiveStrength;
    outMat.alphaCutoff = material.alphaCutoff;
    outMat.alphaMode = static_cast<int32_t>(material.alphaMode);

    if (material.normalTexture.has_value())
        outMat.normalScale = material.normalTexture->scale;
    if (material.occlusionTexture.has_value())
        outMat.occlusionStrength = material.occlusionTexture->strength;

    vk::Device device = _vulkanContext->device;

    auto resolveTexture = [&](const auto& texInfo, uint32_t Material::* slot)
        {
            if (!texInfo.has_value()) return;

            uint32_t gltfTexIdx = static_cast<uint32_t>(texInfo->textureIndex);
            const auto& gltfTex = _asset.textures[gltfTexIdx];
            uint32_t imageIdx = static_cast<uint32_t>(gltfTex.imageIndex.value());

            vk::Filter             magFilter = vk::Filter::eLinear;
            vk::Filter             minFilter = vk::Filter::eLinear;
            vk::SamplerAddressMode wrapU = vk::SamplerAddressMode::eRepeat;
            vk::SamplerAddressMode wrapV = vk::SamplerAddressMode::eRepeat;

            if (gltfTex.samplerIndex.has_value())
            {
                const auto& s = _asset.samplers[gltfTex.samplerIndex.value()];

                magFilter = (s.magFilter == fastgltf::Filter::Nearest)
                    ? vk::Filter::eNearest : vk::Filter::eLinear;

                minFilter = (s.minFilter == fastgltf::Filter::Nearest
                    || s.minFilter == fastgltf::Filter::NearestMipMapNearest
                    || s.minFilter == fastgltf::Filter::NearestMipMapLinear)
                    ? vk::Filter::eNearest : vk::Filter::eLinear;

                switch (s.wrapS)
                {
                case fastgltf::Wrap::ClampToEdge:    wrapU = vk::SamplerAddressMode::eClampToEdge; break;
                case fastgltf::Wrap::MirroredRepeat: wrapU = vk::SamplerAddressMode::eMirroredRepeat; break;
                default:                             wrapU = vk::SamplerAddressMode::eRepeat; break;
                }

                switch (s.wrapT)
                {
                case fastgltf::Wrap::ClampToEdge:    wrapV = vk::SamplerAddressMode::eClampToEdge; break;
                case fastgltf::Wrap::MirroredRepeat: wrapV = vk::SamplerAddressMode::eMirroredRepeat; break;
                default:                             wrapV = vk::SamplerAddressMode::eRepeat; break;
                }
            }

            auto& tex = _resourceManager->textures[imageIdx];
            device.destroySampler(tex.sampler);
            tex.sampler = CreateSampler(device, magFilter, minFilter, wrapU, wrapV);

            outMat.*slot = imageIdx;
        };

    resolveTexture(material.pbrData.baseColorTexture, &Material::baseColorTextureIdx);
    resolveTexture(material.normalTexture, &Material::normalTextureIdx);
    resolveTexture(material.pbrData.metallicRoughnessTexture, &Material::metallicRoughnessTextureIdx);
    resolveTexture(material.occlusionTexture, &Material::occlusionTextureIdx);
    resolveTexture(material.emissiveTexture, &Material::emissiveTextureIdx);

    _resourceManager->materials.push_back(std::move(outMat));
}

void gltfLoader::LoadCamera(fastgltf::Camera& camera)
{

}

void gltfLoader::LoadDefaultScene(Scene& scene)
{

    uint32_t sceneIndex = 0;
    if (_asset.defaultScene.has_value())
    {
        sceneIndex = static_cast<uint32_t>(_asset.defaultScene.value());
    }

    const auto& gltfScene = _asset.scenes[sceneIndex];


    // Create Nodes
    scene.nodes.resize(_asset.nodes.size());

    for (size_t i = 0; i < _asset.nodes.size(); i++)
    {
        const auto& gltfNode = _asset.nodes[i];
        Node& node = scene.nodes[i];

        node.name = gltfNode.name;

        if (gltfNode.meshIndex.has_value())
        {
            node.mesh = MeshHandle{
                static_cast<uint32_t>(gltfNode.meshIndex.value()),
                _resourceManager
            };
        }

        // Transform
        std::visit(
            fastgltf::visitor{
                [&](const fastgltf::TRS& trs)
                {
                    node.transform.position = glm::vec3(
                        trs.translation[0],
                        trs.translation[1],
                        trs.translation[2]);

                    // glTF quaternion = (x,y,z,w)
                    // glm quaternion  = (w,x,y,z)
                    node.transform.rotation = glm::quat(
                        trs.rotation[3],
                        trs.rotation[0],
                        trs.rotation[1],
                        trs.rotation[2]);

                    node.transform.scale = glm::vec3(
                        trs.scale[0],
                        trs.scale[1],
                        trs.scale[2]);
                },

                [&](const fastgltf::math::fmat4x4& m)
                {
                    glm::mat4 matrix = glm::make_mat4(m.data());

                    glm::vec3 scale;
                    glm::quat rotation;
                    glm::vec3 translation;
                    glm::vec3 skew;
                    glm::vec4 perspective;

                    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

                    node.transform.position = translation;
                    node.transform.rotation = rotation;
                    node.transform.scale = scale;
                    assert(false && "Matrix transform is not supported yet.");
                }
            },
            gltfNode.transform);
    }

    //Build hierarchy
    for (size_t parentIndex = 0; parentIndex < _asset.nodes.size(); parentIndex++)
    {
        const auto& gltfNode = _asset.nodes[parentIndex];
        Node& parentNode = scene.nodes[parentIndex];

        for (auto childIndex : gltfNode.children)
        {
            parentNode.children.push_back(
                static_cast<NodeID>(childIndex));

            scene.nodes[childIndex].parent =
                static_cast<NodeID>(parentIndex);
        }
    }

    // Save Root Nodes
    for (auto rootNodeIndex : gltfScene.nodeIndices)
    {
        scene.rootNodes.push_back(
            static_cast<NodeID>(rootNodeIndex));
    }
}