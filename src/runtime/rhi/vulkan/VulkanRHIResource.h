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

		virtual void* Map() override;
		virtual void UnMap() override;

	protected:
		virtual void Destroy() override final;

	private:
		VkBuffer m_handle;

		VmaAllocation m_alloc;
		VmaAllocationInfo m_allocInfo;

		bool m_mapped = false;
		void* m_ptr = nullptr;
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
	public:
		VulkanRHISampler() = delete;
		VulkanRHISampler(const RHISamplerInfo& info, VulkanRHI& rhi);
		~VulkanRHISampler() = default;

		inline const VkSampler& GetHandle() { return m_handle; }
	
	protected:
		virtual void Destroy() override final;

	private:
		VkSampler m_handle;
	};

	class VulkanRHIShader : public RHIShader
	{
	public:
		VulkanRHIShader() = delete;
		VulkanRHIShader(const RHIShaderInfo& info, VulkanRHI& rhi);
		~VulkanRHIShader() = default;

		virtual void Destroy() override final;

		VkPipelineShaderStageCreateInfo GetShaderStageCreateInfo();

		inline VkShaderModule& GetHandle() { return m_handle; }

	private:
		VkShaderModule	m_handle;
	};

	class VulkanRHIDescriptorSet : public RHIDescriptorSet
	{
	public:
		VulkanRHIDescriptorSet() = delete;
		VulkanRHIDescriptorSet(VkDescriptorSetLayout layout, VulkanRHI& rhi);
		~VulkanRHIDescriptorSet() = default;

		inline VkDescriptorSet& GetHandle() { return m_handle; }

		virtual void UpdateBuffer(uint32_t binding, std::shared_ptr<RHIBuffer> buffer) override final;
		virtual void UpdateTexture(uint32_t binding, std::shared_ptr<RHITextureView> view, std::shared_ptr<RHISampler> sampler) override final;
			 
	protected:
		virtual void Destroy() override final;

	private:
		VkDescriptorSet m_handle;
	};

	class VulkanRHIRootSignature : public RHIRootSignature
	{
	public:
		struct Layout
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			VkDescriptorSetLayout handle;
		};

		VulkanRHIRootSignature(const RHIRootSignatureInfo& info, VulkanRHI& rhi);

		virtual std::shared_ptr<RHIDescriptorSet> CreateDescriptorSet(uint32_t set) override final;

		const std::vector<Layout>& GetLayouts() { return m_layouts; }

		virtual void Destroy() override final;

	private:
		std::vector<Layout> m_layouts;
	};

	class VulkanRHIGraphicsPipeline : public RHIGraphicsPipeline
	{
	public:
		VulkanRHIGraphicsPipeline() = delete;
		VulkanRHIGraphicsPipeline(const RHIGraphicsPipelineInfo& info, VulkanRHI& rhi);
		~VulkanRHIGraphicsPipeline() = default;

		inline const VkPipeline& GetHandle() { return m_handle; }
		inline const VkPipelineLayout& GetLayout() { return m_layout; }

		void Bind(VkCommandBuffer cmBuffer);

		inline VkPipelineVertexInputStateCreateInfo GetVertexInputStateCreateInfo(std::shared_ptr<const RHIVertexDeclaration> declaration);
		inline VkPipelineInputAssemblyStateCreateInfo GetInputAssemblyStateCreateInfo(PrimitiveType primitiveType);
		inline VkPipelineRasterizationStateCreateInfo GetRasterizationStateCreateInfo(const RHIRasterizerState& state);
		inline VkPipelineMultisampleStateCreateInfo GetMultisampleStateCreateInfo();
		inline VkPipelineDepthStencilStateCreateInfo GetDepthStencilStateCreateInfo(const RHIDepthStencilState& state);
		inline VkPipelineColorBlendStateCreateInfo GetColorBlendStateCreateInfo(const RHIBlendState& state, uint32_t attachmentCount);
		inline VkPipelineDynamicStateCreateInfo GetDynamicStateCreateInfo();

	protected:
		void Destroy() override final;

	private:
		VkPipeline m_handle;
		VkPipelineLayout m_layout;

		std::vector<VkVertexInputBindingDescription> m_bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> m_attributeDescriptions;
		std::vector<VkPipelineColorBlendAttachmentState> m_blendStates;
		
		// TODO: More Dynamic State
		std::vector<VkDynamicState> m_dynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			// VK_DYNAMIC_STATE_LINE_WIDTH,
			// VK_DYNAMIC_STATE_VERTEX_INPUT_EXT,
			// VK_DYNAMIC_STATE_DEPTH_BIAS,
			// VK_DYNAMIC_STATE_BLEND_CONSTANTS,
			// VK_DYNAMIC_STATE_DEPTH_BOUNDS,
			// VK_DYNAMIC_STATE_STENCIL_REFERENCE
		};

	};
}