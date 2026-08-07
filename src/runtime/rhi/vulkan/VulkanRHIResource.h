#pragma once

#include "runtime/rhi/RHIResource.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHIBuffer : public RHIBuffer
	{
	public:
		VulkanRHIBuffer() = delete;
		VulkanRHIBuffer(const RHIBufferInfo& info, VulkanRHI& rhi);
		~VulkanRHIBuffer() = default;

		inline const VkBuffer& GetHandle() const { return m_handle; }

	protected:
		virtual void Destroy() override final;

	private:
		VkBuffer m_handle;

		VmaAllocation m_alloc;
		VmaAllocationInfo m_allocInfo;

		bool mapped = false;
		void* ptr = nullptr;
	};

	class VulkanRHITexture : public RHITexture
	{
	public:
		VulkanRHITexture() = delete;
		VulkanRHITexture(const RHITextureInfo& info, VulkanRHI& rhi, VkImage image = VK_NULL_HANDLE);
		~VulkanRHITexture() = default;

		inline const VkImage& GetHandle() { return m_handle; }

	protected:
		virtual void Destroy() override final;

	private:
		VkImage m_handle;

		VmaAllocation m_alloc = nullptr;
		VmaAllocationInfo m_allocInfo{};
	};

	class VulkanRHITextureView : public RHITextureView
	{
	public:
		VulkanRHITextureView() = delete;
		VulkanRHITextureView(const RHITextureViewInfo& info, VulkanRHI& rhi);
		~VulkanRHITextureView() = default;
		inline const VkImageView& GetHandle() { return m_handle; }

	protected:
		virtual void Destroy() override final;
	
	private:
		VkImageView m_handle;
	};

	class VulkanRHISampler : public RHISampler
	{
	};

	class VulkanRHIShader : public RHIShader
	{

	};

	class VulkanRHIDescriptorSet : public RHIDescriptorSet
	{
	public:
		VulkanRHIDescriptorSet() = delete;
		VulkanRHIDescriptorSet(VulkanRHI& rhi, VkDescriptorSetLayout layout);
		~VulkanRHIDescriptorSet() = default;

	protected:
		virtual void Destroy() override final;

	private:
		VkDescriptorSet m_handle;
	};

	class VulkanRHIRootSignature : public RHIRootSignature
	{

	};

	class VulkanRHIGraphicsPipeline : public RHIGraphicsPipeline
	{
	public:
		VulkanRHIGraphicsPipeline() = delete;
		VulkanRHIGraphicsPipeline(const RHIGraphicsPipelineInfo& info, VulkanRHI& rhi);
		~VulkanRHIGraphicsPipeline() = default;

		inline const VkPipeline& GetHandle() { return m_handle; }
		inline const VkPipelineLayout& GetLayout() { return m_layout; }

	protected:
		void Destroy() override final;

	private:
		VkPipeline m_handle;
		VkPipelineLayout m_layout;
	};
}