#include "ResourceManager.h"

#include "render/MaterialUniforms.h"
#include "render/descriptor/DescriptorAllocator.h"
#include "render/descriptor/DescriptorSetLayout.h"
#include "resource/resource_types/Texture.h"

#include "vulkan/VulkanContext.h"
#include "vulkan/UploadContext.h"
#include "vulkan/VulkanHelpers.h"


namespace DefaultResourceInit
{
    void InitDefaultResources(
        VulkanContext* vulkanContext,
        UploadContext* uploadContext,
        DescriptorAllocator* descAllocator,
        const DescriptorSetLayout& materialLayout,
        ResourceManager* resourceManager)
    {
        resourceManager->defaultWhiteTexture = CreateDefaultTexture(vulkanContext, uploadContext, 0xFFFFFFFF);

        resourceManager->defaultMaterial =
            CreateDefaultMaterial(
                vulkanContext,
                descAllocator,
                materialLayout,
                resourceManager->defaultWhiteTexture);
    }

    Material CreateDefaultMaterial(
        VulkanContext* vulkanContext,
        DescriptorAllocator* descAllocator,
        const DescriptorSetLayout& materialLayout,
        const Texture& fallbackTexture)
    {
        Material defMat{};

        defMat.baseColorFactor = glm::vec4(1.0f);
        defMat.metallicFactor = 0.0f;
        defMat.roughnessFactor = 0.5f;
        defMat.normalScale = 1.0f;
        defMat.occlusionStrength = 1.0f;
        defMat.emissiveFactor = glm::vec3(0.0f);
        defMat.alphaMode = Material::ALPHA_MODE_OPAQUE;

        MaterialUniforms uni{};
        uni.baseColorFactor = defMat.baseColorFactor;
        uni.metallicFactor = defMat.metallicFactor;
        uni.roughnessFactor = defMat.roughnessFactor;
        uni.normalScale = defMat.normalScale;
        uni.occlusionStrength = defMat.occlusionStrength;
        uni.emissiveFactor = glm::vec4(defMat.emissiveFactor, 0.0f);
        uni.emissiveStrength = 0.0f;
        uni.alphaCutoff = 0.5f;
        uni.alphaMode = defMat.alphaMode;

        defMat.materialUBO = CreateBuffer(
            vulkanContext->allocator,
            sizeof(MaterialUniforms),
            vk::BufferUsageFlagBits::eUniformBuffer,
            VMA_MEMORY_USAGE_CPU_TO_GPU);
        memcpy(defMat.materialUBO.info.pMappedData, &uni, sizeof(MaterialUniforms));

        defMat.descSet =
            descAllocator->AllocateMaterialSet(materialLayout.descSetLayout);

        vk::DescriptorBufferInfo uboInfo{};
        uboInfo.buffer = defMat.materialUBO.buffer;
        uboInfo.offset = 0;
        uboInfo.range = sizeof(MaterialUniforms);

        vk::DescriptorImageInfo texInfo{};
        texInfo.setImageView(fallbackTexture.imageView)
            .setSampler(fallbackTexture.sampler)
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

        std::array<vk::WriteDescriptorSet, 6> writes{};
        writes[0].setDstSet(defMat.descSet).setDstBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setBufferInfo(uboInfo);
        for (int i = 0; i < 5; ++i) {
            writes[1 + i].setDstSet(defMat.descSet).setDstBinding(1 + i)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setImageInfo(texInfo);
        }
        vulkanContext->device.updateDescriptorSets(writes, {});

        return defMat;
    }

    Texture CreateDefaultTexture(VulkanContext* vulkanContext, UploadContext* uploadContext, uint32_t rgba)
    {
        AllocatedImage image = CreateImage(
            vulkanContext->allocator,
            { 1, 1, 1 },
            vk::Format::eR8G8B8A8Srgb,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            VMA_MEMORY_USAGE_GPU_ONLY);

        AllocatedBuffer staging = CreateBuffer(
            vulkanContext->allocator, 4,
            vk::BufferUsageFlagBits::eTransferSrc,
            VMA_MEMORY_USAGE_CPU_ONLY);
        memcpy(staging.info.pMappedData, &rgba, 4);

        ImmediateSubmit(*vulkanContext, *uploadContext,
            [&](vk::CommandBuffer cmd) {
                TransitionImageLayout(cmd, image.image,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageAspectFlagBits::eColor);
                vk::BufferImageCopy copy{};
                copy.setImageSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 })
                    .setImageExtent({ 1, 1, 1 });
                cmd.copyBufferToImage(staging.buffer, image.image,
                    vk::ImageLayout::eTransferDstOptimal, copy);
                TransitionImageLayout(cmd, image.image,
                    vk::ImageLayout::eTransferDstOptimal,
                    vk::ImageLayout::eShaderReadOnlyOptimal,
                    vk::ImageAspectFlagBits::eColor);
            });

        DestroyBuffer(vulkanContext->allocator, staging);

        vk::ImageView view = CreateImageView(vulkanContext->device, image.image,
            vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
        vk::Sampler sampler = CreateSampler(vulkanContext->device,
            vk::Filter::eLinear, vk::Filter::eLinear,
            vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat);

        return { std::move(image), view, sampler };
    }
}