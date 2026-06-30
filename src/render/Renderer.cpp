#include "Renderer.h"
#include <VkBootstrap.h>
#include <SDL3/SDL_vulkan.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "application/SDLWindow.h"
#include "render/FrameUniforms.h"
#include "render/RenderView.h"
#include "render/descriptor/DescriptorAllocator.h"
#include "render/descriptor/DescriptorSetLayoutCache.h"
#include "render/pipeline/PipelineBuilder.h"
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
    defaultPipeline = CreateDefaultPipeline(
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
    auto device = vulkanContext->device;
    FrameSlot& slot = frameSlots[currentFrame];

    VK_CHECK(device.waitForFences(slot.renderFence, true, UINT64_MAX));
    device.resetFences(slot.renderFence);

    auto acquireResult = device.acquireNextImageKHR(
        swapchainContext->swapchain,
        UINT64_MAX,
        slot.swapchainSemaphore);

    if (acquireResult.result != vk::Result::eSuccess &&
        acquireResult.result != vk::Result::eSuboptimalKHR)
    {
        fmt::println("[Vulkan Error] acquireNextImageKHR: {}",
            vk::to_string(acquireResult.result));
        return;
    }
    uint32_t imageIndex = acquireResult.value;

    memcpy(slot.frameDataUBO.info.pMappedData,
        &renderView.frameData,
        sizeof(FrameUniforms));

    slot.mainCommandBuffer.reset();
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    slot.mainCommandBuffer.begin(beginInfo);

    vk::CommandBuffer cmd = slot.mainCommandBuffer;

    vk::Image swapchainImage = swapchainContext->images[imageIndex];
    TransitionImageLayout(cmd, swapchainImage,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageAspectFlagBits::eColor);

    vk::RenderingAttachmentInfo colorAttachment{};
    colorAttachment
        .setImageView(swapchainContext->imageViews[imageIndex])
        .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setClearValue(vk::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f });

    vk::RenderingAttachmentInfo depthAttachment{};
    depthAttachment
        .setImageView(slot.depthImageView)
        .setImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setClearValue(vk::ClearDepthStencilValue{ 1.0f, 0 });

    vk::RenderingInfo renderingInfo{};
    renderingInfo
        .setRenderArea(vk::Rect2D{ {0, 0}, swapchainContext->extent })
        .setLayerCount(1)
        .setColorAttachments(colorAttachment)
        .setPDepthAttachment(&depthAttachment);

    cmd.beginRendering(renderingInfo);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, defaultPipeline.pipeline);

    vk::Viewport viewport{};
    viewport
        .setX(0.0f)
        .setY(0.0f)
        .setWidth(static_cast<float>(swapchainContext->extent.width))
        .setHeight(static_cast<float>(swapchainContext->extent.height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    cmd.setViewport(0, viewport);

    vk::Rect2D scissor{ {0, 0}, swapchainContext->extent };
    cmd.setScissor(0, scissor);

    cmd.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics,
        defaultPipeline.pipelineLayout,
        0,                              
        slot.frameDescriptorSet,        
        {});

    for (const auto& draw : renderView.opaqueDraws)
    {
        // Push constants: model matrix (64 bytes)
        cmd.pushConstants(
            defaultPipeline.pipelineLayout,
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(glm::mat4),
            &draw.modelMatrix);

        // Bind material descriptor set (set = 1)
        cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            defaultPipeline.pipelineLayout,
            1,
            draw.materialSet,
            {});

        // Bind geometry
        vk::DeviceSize vbOffset = 0;
        cmd.bindVertexBuffers(0, draw.vertexBuffer, vbOffset);
        cmd.bindIndexBuffer(draw.indexBuffer, 0, vk::IndexType::eUint32);

        cmd.drawIndexed(draw.indexCount, 1, 0, 0, 0);
    }

    cmd.endRendering();

    TransitionImageLayout(cmd, swapchainImage,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::ImageAspectFlagBits::eColor);

    cmd.end();

    vk::PipelineStageFlags waitStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo{};
    submitInfo
        .setWaitSemaphores(slot.swapchainSemaphore)
        .setWaitDstStageMask(waitStage)
        .setCommandBuffers(slot.mainCommandBuffer)
        .setSignalSemaphores(swapchainContext->renderSemaphores[imageIndex]);

    vulkanContext->graphicsQueue.submit(submitInfo, slot.renderFence);

    vk::PresentInfoKHR presentInfo{};
    presentInfo
        .setWaitSemaphores(swapchainContext->renderSemaphores[imageIndex])
        .setSwapchains(swapchainContext->swapchain)
        .setImageIndices(imageIndex);

    auto presentResult = vulkanContext->graphicsQueue.presentKHR(presentInfo);
    if (presentResult != vk::Result::eSuccess &&
        presentResult != vk::Result::eSuboptimalKHR)
    {
        fmt::println("[Vulkan Error] presentKHR: {}", vk::to_string(presentResult));
    }

    currentFrame = (currentFrame + 1) % FRAME_OVERLAP;
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

    AllocatedImage depthImage;
    vk::ImageView depthImageView;

    AllocatedBuffer frameDataUBO;
    vk::DescriptorSet frameDescriptorSet;
};
*/