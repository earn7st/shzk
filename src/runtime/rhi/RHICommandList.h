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

	class RHICommandList
	{
	private:
		static std::shared_ptr<RHICommandList> g_RhiCmdList;

	public:
		RHICommandList() = default;
        ~RHICommandList() = default;

		static void Init(bool bypass = true);
		static std::shared_ptr<RHICommandList>& Get() { return g_RhiCmdList; }

        inline void SetContext(RHICommandContext* context) { m_cmdContext = context; }
        inline RHICommandContext& GetContext() { return *m_cmdContext; }

        void BeginCommand();
        void EndCommand();
        void Submit(
            std::shared_ptr<RHIFence> fence,
            std::shared_ptr<RHISemaphore> waitSemaphore,
            std::shared_ptr<RHISemaphore> signalSemaphore);
        void TextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba);
        void TextureBarrier(const RHITextureBarrier& barrier);
        // void BeginRenderPass();
        // void EndRenderPass();


        /*
        void Execute(std::shared_ptr<RHIFence> waitFence,
            std::shared_ptr<RHISemaphore> waitSemaphore,
            std::shared_ptr<RHISemaphore> signalSemaphore);

        void BeginRenderPass(const RHIRenderPassInfo& info, const char* name);
        void EndRenderPass();

        void SetViewport(float minX, float minY, float minZ,
            float maxX, float maxY, float maxZ);
        void SetScissorRect(bool enable, uint32_t minX, uint32_t minY,
            uint32_t maxX, uint32_t maxY);
        void SetGraphicsPipelineState(std::shared_ptr<RHIGraphicsPipeline> pipeline);

        void SetStreamSource(uint32_t streamIndex, std::shared_ptr<RHIBuffer> vb,
            uint32_t offset);
        void BindDescriptorSet(std::shared_ptr<RHIDescriptorSet> descriptor,
            uint32_t set);

        void DrawPrimitive(uint32_t baseVertexIndex, uint32_t numPrimitives,
            uint32_t numInstances);
        void DrawIndexedPrimitive(std::shared_ptr<RHIBuffer> indexBuffer,
            int32_t baseVertexIndex, uint32_t firstInstance,
            uint32_t numVertices, uint32_t startIndex,
            uint32_t numPrimitives, uint32_t numInstances);

        void SetShaderRootConstants(void* data, uint16_t size, ShaderFrequency frequency);

        void TextureBarrier(const RHITextureBarrier& barrier);
        void BufferBarrier(const RHIBufferBarrier& barrier);

        void CopyBufferToTexture(std::shared_ptr<RHIBuffer> src,
            std::shared_ptr<RHITexture> dst,
            const RHICopyBufferToTextureInfo& info);
        void CopyBuffer(std::shared_ptr<RHIBuffer> dst, uint64_t dstOffset,
            std::shared_ptr<RHIBuffer> src, uint64_t srcOffset,
            uint64_t size);

        void SetContext(RHICommandContext* context);
        */

	private:
		RHICommandContext* m_cmdContext;
		bool m_bypass;
	};
}