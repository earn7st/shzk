#include "Renderer.h"
#include <VkBootstrap.h>
#include <SDL3/SDL_vulkan.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "application/SDLWindow.h"
#include "render/descriptor/DescriptorAllocator.h"
#include "render/descriptor/DescriptorSetLayoutCache.h"
#include "render/pipeline/PipelineBuilder.h"
#include "render/FrameUniforms.h"
#include "scene/Scene.h"
#include "vulkan/VulkanContext.h"
#include "vulkan/SwapchainContext.h"
#include "vulkan/UploadContext.h"
#include "vulkan/VulkanHelpers.h"

Renderer::Renderer() = default;

Renderer::Renderer(const SDLWindow& window)
{
    // Contexts
    vulkanContext = CreateVulkanContext(window);
    swapchainContext = CreateSwapchainContext(*vulkanContext, window.extent.width, window.extent.height);
    uploadContext = CreateUploadContext(*vulkanContext);

    // DescriptorSetLayouts, Managed by descSetLayoutCache
    descSetLayoutCache = DescriptorSetLayoutCache::Create(vulkanContext->device);
    descAllocator = DescriptorAllocator::Create(vulkanContext->device);

    // Pipelines
    whitePipeline = CreateWhitePipeline(
        vulkanContext->device,
        descSetLayoutCache->defaultFrameLayout.descSetLayout,
        descSetLayoutCache->defaultMaterialLayout.descSetLayout,
        swapchainContext->format,
        vk::Format::eD32Sfloat);

    // FrameSlots
    for (int i = 0; i < FRAME_OVERLAP; ++i)
    {
        InitSlotCommand(vulkanContext->device, vulkanContext->graphicsQueueFamily, frameSlots[i]);
        InitSlotSync(vulkanContext->device, frameSlots[i]);
        InitSlotDepth(
            vulkanContext->allocator,
            vulkanContext->device,
            swapchainContext->extent,   // Currently set depth images's extent equals to swapchain images's
            vk::Format::eD32Sfloat,
            frameSlots[i]);
        InitSlotUniform(
            vulkanContext->allocator,
            vulkanContext->device,
            *descAllocator,
            descSetLayoutCache->defaultFrameLayout.descSetLayout,
            frameSlots[i]);
    }

    // —— Depth image initial layout transition (undefined → depth attachment) ——
    for (int i = 0; i < FRAME_OVERLAP; ++i)
    {
        ImmediateSubmit(
            *vulkanContext,
            *uploadContext,
            [&](vk::CommandBuffer cmd) {
                TransitionImageLayout(
                    cmd,
                    frameSlots[i].depthImage.image,
                    vk::ImageLayout::eUndefined,
                    vk::ImageLayout::eDepthStencilAttachmentOptimal,
                    vk::ImageAspectFlagBits::eDepth);
            });
    }

}

void Renderer::ResizeSwapchain(uint32_t width, uint32_t height)
{

}

void Renderer::DrawFrame(const RenderView& renderView)
{
}

Renderer::~Renderer() = default;

namespace
{
    void InitSlotCommand(
        vk::Device device,
        uint32_t graphicsQueueFamily,
        FrameSlot& slot)
    {
        vk::CommandPoolCreateInfo commandPoolInfo{};
        commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolInfo.queueFamilyIndex = graphicsQueueFamily;

        slot.commandPool = device.createCommandPool(commandPoolInfo);

        vk::CommandBufferAllocateInfo commandBufferAllocInfo{};
        commandBufferAllocInfo.commandPool = slot.commandPool;
        commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
        commandBufferAllocInfo.commandBufferCount = 1;

        auto commandBuffers = device.allocateCommandBuffers(commandBufferAllocInfo);

        slot.mainCommandBuffer = commandBuffers.front();
    }

    void InitSlotSync(
        vk::Device device,
        FrameSlot& slot)
    {
        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        slot.renderFence = device.createFence(fenceInfo);

        vk::SemaphoreCreateInfo semInfo{};
        slot.swapchainSemaphore = device.createSemaphore(semInfo);
        slot.renderSemaphore = device.createSemaphore(semInfo);
    }

    void InitSlotDepth(
        VmaAllocator allocator,
        vk::Device device,
        vk::Extent2D extent,
        vk::Format format,
        FrameSlot& slot)
    {
        slot.depthImage = CreateImage(
            allocator,
            {extent.width, extent.height, 1},
            format,
            vk::ImageUsageFlagBits::eDepthStencilAttachment
                | vk::ImageUsageFlagBits::eSampled,
            VMA_MEMORY_USAGE_GPU_ONLY);

        slot.depthImageView = CreateImageView(
            device,
            slot.depthImage.image,
            format,
            vk::ImageAspectFlagBits::eDepth);
    }

    void InitSlotUniform(
        VmaAllocator allocator,
        vk::Device device,
        DescriptorAllocator& descAllocator,
        vk::DescriptorSetLayout layout,
        FrameSlot& slot)
    {
        slot.frameDataUBO = CreateBuffer(
            allocator,
            sizeof(FrameUniforms),
            vk::BufferUsageFlagBits::eUniformBuffer,
            VMA_MEMORY_USAGE_CPU_TO_GPU);

        slot.frameDescriptorSet = descAllocator.AllocateFrameSet(layout);

        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = slot.frameDataUBO.buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(FrameUniforms);

        vk::WriteDescriptorSet write{};
        write.setDstSet(slot.frameDescriptorSet);
        write.setDstBinding(0);
        write.setDescriptorType(vk::DescriptorType::eUniformBuffer);
        write.setBufferInfo(bufferInfo);

        device.updateDescriptorSets(write, {});
    }

    void DestroySlotCommand(vk::Device device, FrameSlot& slot)
    {
        device.destroyCommandPool(slot.commandPool);
    }

    void DestroySlotSync(vk::Device device, FrameSlot& slot)
    {
        device.destroyFence(slot.renderFence);
        device.destroySemaphore(slot.swapchainSemaphore);
        device.destroySemaphore(slot.renderSemaphore);
    }

    void DestroySlotDepth(VmaAllocator allocator, vk::Device device, FrameSlot& slot)
    {
        device.destroyImageView(slot.depthImageView);
        DestroyImage(allocator, slot.depthImage);
    }

    void DestroySlotUniform(VmaAllocator allocator, FrameSlot& slot)
    {
        DestroyBuffer(allocator, slot.frameDataUBO);
    }
}

/*
struct FrameSlot
{
    vk::CommandPool commandPool;
    vk::CommandBuffer mainCommandBuffer;

    vk::Fence renderFence;
    vk::Semaphore swapchainSemaphore;
    vk::Semaphore renderSemaphore;

    AllocatedImage depthImage;
    vk::ImageView depthImageView;

    AllocatedBuffer frameDataUBO;
    vk::DescriptorSet frameDescriptorSet;
};
*/