#pragma once

#include "RHIDefinitions.h"

#include <iostream>
#include <memory>
#include <glm/glm.hpp>

namespace shzk
{
	class RHICommandContext;
    class RHIFence;
    class RHISemaphore;
    class RHIGraphicsPipeline;
    class RHIBuffer;
    class RHIDescriptorSet;

	class RHICommandList
	{
	private:
		static std::shared_ptr<RHICommandList> g_rhiCmdList;

	public:
		RHICommandList() = default;
        ~RHICommandList() = default;

		static void Init(bool bypass = true);
		static std::shared_ptr<RHICommandList>& Get() { return g_rhiCmdList; }

        inline void SetContext(RHICommandContext* context) { m_cmdContext = context; }
        inline RHICommandContext& GetContext() { return *m_cmdContext; }

        void BeginCommand();
        void EndCommand();
        void Submit(
            std::shared_ptr<RHIFence> fence,
            std::shared_ptr<RHISemaphore> waitSemaphore,
            std::shared_ptr<RHISemaphore> signalSemaphore);

        void BlitTexture(std::shared_ptr<RHITexture> src, std::shared_ptr<RHITexture> dst,
            TextureSubresourceLayers srcSubresource, TextureSubresourceLayers dstSubresource,
            FilterType filter);
        void TextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba);
        void TextureBarrier(const RHITextureBarrier& barrier);
        void BufferBarrier(const RHIBufferBarrier& barrier);
        void CopyTextureToBuffer(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset);
        void CopyBufferToTexture(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource);
        void CopyBuffer(std::shared_ptr<RHIBuffer> src, uint64_t srcOffset, std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset, uint64_t size);
        void CopyTexture(std::shared_ptr<RHITexture> src, TextureSubresourceLayers srcSubresource, std::shared_ptr<RHITexture> dst, TextureSubresourceLayers dstSubresource);
        void GenerateMips(std::shared_ptr<RHITexture> src);

        void SetViewport(Offset2D min, Offset2D max);
        void SetScissor(Offset2D min, Offset2D max);
        void ClearScissors(const std::vector<ClearAttachment>& attachments, const std::vector<Rect2D>& scissors, uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);
        void SetDepthBias(float constantBias, float slopeBias, float clampBias);
        void SetLineWidth(float width);

        void SetGraphicsPipeline(std::shared_ptr<RHIGraphicsPipeline> graphicsPipeline);
        // void SetComputePipeline(RHIComputePipelineRef computePipeline);

        void BeginRendering(const RHIRenderPassInfo& info);
        void EndRendering();

        void PushConstants(void* data, uint16_t size, ShaderFrequency frequency);
        void BindDescriptorSet(std::shared_ptr<RHIDescriptorSet> descriptorSet, uint32_t set = 0);
        void BindVertexBuffer(std::shared_ptr<RHIBuffer> vertexBuffer, uint32_t streamIndex = 0, uint32_t offset = 0);
        void BindIndexBuffer(std::shared_ptr<RHIBuffer> indexBuffer, uint32_t offset = 0);

        void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
        void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, uint32_t vertexOffset = 0, uint32_t firstInstance = 0);

        // Debug
        // void PushEvent(const std::string& name, glm::vec3 color = { 0.0f, 0.0f, 0.0f });
        // void PopEvent();

	private:
		RHICommandContext* m_cmdContext = nullptr;
        bool m_bypass = true;
	};
}