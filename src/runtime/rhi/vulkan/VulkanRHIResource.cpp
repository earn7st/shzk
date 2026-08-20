#include "VulkanRHIResource.h"
#include "VulkanUtil.h"
#include "VulkanRHI.h"

#include "runtime/rhi/RHIDefinitions.h"
#include "runtime/rhi/RHIUtil.h"

#include <volk/volk.h>

namespace shzk
{
    VulkanRHIBuffer::VulkanRHIBuffer(const RHIBufferInfo& info, VulkanRHI& rhi)
        : RHIBuffer(info)
    {
        VkBufferUsageFlags usage = VulkanUtil::ResourceTypeToVkBufferUsage(info.type);
        if (info.memoryUsage == MemoryUsage::GPUOnly || info.memoryUsage == MemoryUsage::GPUToCPU)
        {
            usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = info.size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.queueFamilyIndexCount = 0,
            bufferInfo.pQueueFamilyIndices = NULL;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = VulkanUtil::MemoryUsageToVma(info.memoryUsage);
        if (info.creationFlag & BUFFER_CREATION_PERSISTENT_MAP)
        {
            allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
            m_mapped = true;
        }

        m_allocInfo = {};
        VK_CHECK(vmaCreateBuffer(rhi.GetAllocator(), &bufferInfo, &allocationCreateInfo, &m_handle, &m_alloc, &m_allocInfo));
    }

    void* VulkanRHIBuffer::Map()
    {
        if (m_info.creationFlag & BUFFER_CREATION_PERSISTENT_MAP) return m_allocInfo.pMappedData;
        if (!m_mapped)
        {
            vmaMapMemory(VULKAN_RHI()->GetAllocator(), m_alloc, &m_ptr);
            m_mapped = true;
        }
        return m_ptr;
    }

    void VulkanRHIBuffer::UnMap()
    {
        if (m_mapped && !(m_info.creationFlag & BUFFER_CREATION_PERSISTENT_MAP))
        {
            vmaUnmapMemory(VULKAN_RHI()->GetAllocator(), m_alloc);
            m_ptr = nullptr;
            m_mapped = false;
        }
    }

    void VulkanRHIBuffer::Destroy()
    {
        vmaDestroyBuffer(VULKAN_RHI()->GetAllocator(), m_handle, m_alloc);
    }

	VulkanRHITexture::VulkanRHITexture(const RHITextureInfo& info, VulkanRHI& rhi, VkImage image)	
		: RHITexture(info), m_handle(image)
	{
		TextureAspectFlags aspects =    
            RHIUtil::IsDepthStencilFormat(info.format)  ?   TEXTURE_ASPECT_DEPTH_STENCIL :
            RHIUtil::IsDepthFormat(info.format)         ?   TEXTURE_ASPECT_DEPTH :
            RHIUtil::IsStencilFormat(info.format)       ?   TEXTURE_ASPECT_STENCIL : 
                                                            TEXTURE_ASPECT_COLOR;
        m_defaultRange = {aspects, 0, info.mipLevels, 0, info.arrayLayers};
        m_defaultLayers = {aspects, 0, 0, info.arrayLayers};

        if(image != VK_NULL_HANDLE)     // for swapchain images encapsulation
        {
            m_handle = image; 
            return;
        }

        VkFormat format = VulkanUtil::RHIFormatToVkFormat(info.format);

        VkImageUsageFlags usage = VulkanUtil::ResourceTypeToVkImageUsage(info.type);
        if (RHIUtil::IsDepthFormat(info.format) || RHIUtil::IsStencilFormat(info.format))
        {
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        else if (info.type & RESOURCE_TYPE_RENDER_TARGET)
        {   
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }

        VkImageType type =  
            info.extent.depth > 1   ?   VK_IMAGE_TYPE_3D :
            info.extent.height > 1  ?   VK_IMAGE_TYPE_2D : 
                                        VK_IMAGE_TYPE_1D;
        if(info.creationFlag & TEXTURE_CREATION_FORCE_2D) type = VK_IMAGE_TYPE_2D;
        if(info.creationFlag & TEXTURE_CREATION_FORCE_3D) type = VK_IMAGE_TYPE_3D;

        VkImageCreateFlags flag = 0;
        if(info.type & RESOURCE_TYPE_TEXTURE_CUBE)      flag |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        if(type      & VK_IMAGE_TYPE_3D)                flag |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT_KHR;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = type;
        imageInfo.extent.width = info.extent.width;
        imageInfo.extent.height = info.extent.height;
        imageInfo.extent.depth = info.extent.depth;
        imageInfo.mipLevels = info.mipLevels;
        imageInfo.arrayLayers = info.arrayLayers;   
        if (info.type & RESOURCE_TYPE_TEXTURE_CUBE)
        {
            imageInfo.arrayLayers = std::max(imageInfo.arrayLayers, (uint32_t)6);
        }
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = flag; // Optional

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = VulkanUtil::MemoryUsageToVma(info.memoryUsage);

        m_allocInfo = {};
        VK_CHECK(vmaCreateImage(rhi.GetAllocator(), &imageInfo, &allocationCreateInfo, &m_handle, &m_alloc, &m_allocInfo));
	}

    void VulkanRHITexture::Destroy()
    {
        vmaDestroyImage(VULKAN_RHI()->GetAllocator(), m_handle, m_alloc);
    }

    VulkanRHITextureView::VulkanRHITextureView(const RHITextureViewInfo& info, VulkanRHI& rhi)
        : RHITextureView(info)
    {
        if (info.subresourceRange.aspect == TEXTURE_ASPECT_NONE)
        {
            this->m_info.subresourceRange = info.texture->GetDefaultSubresourceRange();
        }

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = CastTo<VulkanRHITexture>(info.texture)->GetHandle();
        viewInfo.viewType = VulkanUtil::TextureViewTypeToVk(info.viewType);
        viewInfo.format = VulkanUtil::RHIFormatToVkFormat(info.format);
        viewInfo.subresourceRange.aspectMask = VulkanUtil::TextureAspectFlagsToVk(info.subresourceRange.aspect);
        viewInfo.subresourceRange.baseMipLevel = info.subresourceRange.baseMipLevel;
        viewInfo.subresourceRange.levelCount = info.subresourceRange.levelCount;
        viewInfo.subresourceRange.baseArrayLayer = info.subresourceRange.baseArrayLayer;
        viewInfo.subresourceRange.layerCount = info.subresourceRange.layerCount;
		
		VK_CHECK(vkCreateImageView(rhi.GetDevice(), &viewInfo, nullptr, &m_handle));
    }

    void VulkanRHITextureView::Destroy()
    {
		vkDestroyImageView(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
    }

    VulkanRHIShader::VulkanRHIShader(const RHIShaderInfo& info, VulkanRHI& rhi)
        : RHIShader(info)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = info.code.size();
        createInfo.pCode = (const uint32_t*)info.code.data();

        VK_CHECK(vkCreateShaderModule(rhi.GetDevice(), &createInfo, nullptr, &m_handle));
        this->m_info.code.clear();
    }

    VkPipelineShaderStageCreateInfo VulkanRHIShader::GetShaderStageCreateInfo()
    {
        VkPipelineShaderStageCreateInfo shaderStage = {};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.stage = VulkanUtil::ShaderFrequencyToVkStageFlagBits(m_info.frequency);
        shaderStage.module = m_handle;
        shaderStage.pName = m_info.entry.c_str();

        return shaderStage;
    }

    void VulkanRHIShader::Destroy()
    {
        vkDestroyShaderModule(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
    }

    VulkanRHIDescriptorSet::VulkanRHIDescriptorSet(VkDescriptorSetLayout layout, VulkanRHI& rhi)
		: RHIDescriptorSet()
    {
		VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = rhi.GetDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;
        
        VK_CHECK(vkAllocateDescriptorSets(rhi.GetDevice(), &allocInfo, &m_handle));
    }

    void VulkanRHIDescriptorSet::Destroy()
    {
		vkFreeDescriptorSets(VULKAN_RHI()->GetDevice(), VULKAN_RHI()->GetDescriptorPool(), 1, &m_handle);
    }

    VulkanRHIRootSignature::VulkanRHIRootSignature(const RHIRootSignatureInfo& info, VulkanRHI& rhi)
        : RHIRootSignature(info)
    {
        const std::vector<ShaderResourceEntry>& entries = info.GetEntries();
        for (const ShaderResourceEntry& entry : entries)
        {
            VkDescriptorSetLayoutBinding layoutBinding = {};
            layoutBinding.binding = entry.binding;
            layoutBinding.stageFlags = VulkanUtil::ShaderFrequencyToVkStageFlags(entry.frequency);
            layoutBinding.descriptorType = VulkanUtil::ResourceTypeToVkDescriptorType(entry.type);
            layoutBinding.descriptorCount = entry.size;     // TODO: Bindless ?  
            layoutBinding.pImmutableSamplers = nullptr;

            if (m_layouts.size() < entry.set + 1) m_layouts.resize(entry.set + 1);
            m_layouts[entry.set].bindings.push_back(layoutBinding);
        }

        for (Layout& layout: m_layouts)
        {
            if (layout.bindings.size() > 0)
            {
                VkDescriptorSetLayoutCreateInfo layoutInfo;
                layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = (uint32_t)layout.bindings.size();
                layoutInfo.pBindings = layout.bindings.data();
                layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;  //TODO descriptor instant update ?
                layoutInfo.pNext = nullptr;

                // TODO: bindless 可变描述符大小扩展

                VK_CHECK(vkCreateDescriptorSetLayout(rhi.GetDevice(), &layoutInfo, nullptr, &layout.handle));
            }
        }
    }

    std::shared_ptr<RHIDescriptorSet> VulkanRHIRootSignature::CreateDescriptorSet(uint32_t set)
    {
        if (m_layouts.size() > set && m_layouts[set].bindings.size() > 0)
        {
            std::shared_ptr<RHIDescriptorSet> descriptorSet = std::make_shared<VulkanRHIDescriptorSet>(m_layouts[set].handle, *VULKAN_RHI());
            VULKAN_RHI()->RegisterResource(descriptorSet);

            return descriptorSet;
        }

        SHZK_LOG_WARN("DescriptorSetLayouts are empty!");
        return nullptr;
    }

    void VulkanRHIRootSignature::Destroy()
    {
        for (Layout& layout : m_layouts)
        {
            vkDestroyDescriptorSetLayout(VULKAN_RHI()->GetDevice(), layout.handle, nullptr);
        }
    }

    VulkanRHIGraphicsPipeline::VulkanRHIGraphicsPipeline(const RHIGraphicsPipelineInfo& info, VulkanRHI& rhi)
        : RHIGraphicsPipeline(info)
    {
        // Pipeline Layout
        std::vector<VkPushConstantRange> pushConstantRanges;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        for (const auto& pushConstant : info.rootSignature->GetInfo().GetPushConstants())
        {
            pushConstantRanges.push_back(VulkanUtil::PushConstantInfoToVk(pushConstant));
        }
        for (const auto& layout : CastTo<VulkanRHIRootSignature>(info.rootSignature)->GetLayouts())
        {
            descriptorSetLayouts.push_back(layout.handle);
        }
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)pushConstantRanges.size();
        pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();
        VK_CHECK(vkCreatePipelineLayout(VULKAN_RHI()->GetDevice(), &pipelineLayoutInfo, nullptr, &m_layout));

        // Shaders
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        if (info.vertexShader)   shaderStages.push_back(CastTo<VulkanRHIShader>(info.vertexShader)->GetShaderStageCreateInfo());
        if (info.fragmentShader) shaderStages.push_back(CastTo<VulkanRHIShader>(info.fragmentShader)->GetShaderStageCreateInfo());
        //if (info.geometryShader) shaderStages.push_back(ResourceCast(info.geometryShader)->GetShaderStageCreateInfo());   // TODO: GeometryShader

        // Render Pass
        uint32_t attachmentCount = 0;
        std::vector<VkFormat> colorAttachmentFormats;
        for (const RHIFormat& format : info.colorAttachmentFormats)
        {
            attachmentCount++;
            if (format == FORMAT_UKNOWN) break;
            colorAttachmentFormats.push_back(VulkanUtil::RHIFormatToVkFormat(format));
        }

        VkPipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingInfo.viewMask = info.viewMask;
        renderingInfo.colorAttachmentCount = attachmentCount;
        renderingInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
        renderingInfo.depthAttachmentFormat = VulkanUtil::RHIFormatToVkFormat(info.depthStencilAttachmentFormat);
        renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        // States
        VkPipelineVertexInputStateCreateInfo    vertexInputStateCI      = GetVertexInputStateCreateInfo(info.vertexInputState);
        VkPipelineInputAssemblyStateCreateInfo  inputAssemblyStateCI    = GetInputAssemblyStateCreateInfo(info.primitiveType);
        // VkPipelineTessellationStateCreateInfo   tessellationStateCI;   
        // VkPipelineViewportStateCreateInfo       viewportStateCI;  // Use Dynamic
        VkPipelineRasterizationStateCreateInfo  rasterizationStateCI    = GetRasterizationStateCreateInfo(info.rasterizerState);
        VkPipelineMultisampleStateCreateInfo    multisampleStateCI      = GetMultisampleStateCreateInfo();
        VkPipelineDepthStencilStateCreateInfo   depthStencilStateCI     = GetDepthStencilStateCreateInfo(info.depthStencilState);
        VkPipelineColorBlendStateCreateInfo     colorBlendStateInfo     = GetColorBlendStateCreateInfo(info.blendState, attachmentCount);
        VkPipelineDynamicStateCreateInfo        dynamicStateInfo        = GetDynamicStateCreateInfo();

		VkGraphicsPipelineCreateInfo pipelineCI{};
        pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCI.flags = 0;
        pipelineCI.stageCount = (uint32_t)shaderStages.size();
        pipelineCI.pStages = shaderStages.data();
        pipelineCI.pVertexInputState = &vertexInputStateCI;
        pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
        pipelineCI.pTessellationState = nullptr;
        pipelineCI.pViewportState = nullptr;    // use dynamic
        pipelineCI.pRasterizationState = &rasterizationStateCI;
        pipelineCI.pMultisampleState = &multisampleStateCI;
        pipelineCI.pDepthStencilState = &depthStencilStateCI;
        pipelineCI.pColorBlendState;
        pipelineCI.pDynamicState;
        pipelineCI.layout = m_layout;
        pipelineCI.renderPass = VK_NULL_HANDLE;
        pipelineCI.subpass = 0;     //?
        pipelineCI.basePipelineHandle = VK_NULL_HANDLE; //?
        pipelineCI.basePipelineIndex = 0; //?

        VK_CHECK(vkCreateGraphicsPipelines(VULKAN_RHI()->GetDevice(), VK_NULL_HANDLE, 1, &pipelineCI, nullptr, &m_handle));
    }

    inline VkPipelineVertexInputStateCreateInfo VulkanRHIGraphicsPipeline::GetVertexInputStateCreateInfo(const RHIVertexDeclaration& vertexInputState)
    {
        for (const VertexElement& element : vertexInputState.elements)
        {
            uint8_t binding = element.streamIndex;
            VkVertexInputAttributeDescription attributeDescription = {};
            attributeDescription.binding = binding;
            attributeDescription.location = element.attributeIndex;
            attributeDescription.format = VulkanUtil::VertexElementTypeToVkFormat(element.type);
            attributeDescription.offset = element.offset;
            m_attributeDescriptions.push_back(attributeDescription);

            while (m_bindingDescriptions.size() < element.streamIndex + 1) m_bindingDescriptions.push_back({});
            m_bindingDescriptions[binding].binding = binding;
            m_bindingDescriptions[binding].stride = element.stride;
            m_bindingDescriptions[binding].inputRate = element.bUseInstanceIndex ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        }

        VkPipelineVertexInputStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.vertexBindingDescriptionCount = (uint32_t)m_bindingDescriptions.size();
        info.pVertexBindingDescriptions = m_bindingDescriptions.data();
        info.vertexAttributeDescriptionCount = (uint32_t)m_attributeDescriptions.size();
        info.pVertexAttributeDescriptions = m_attributeDescriptions.data();

        return info;
    }

    inline VkPipelineInputAssemblyStateCreateInfo VulkanRHIGraphicsPipeline::GetInputAssemblyStateCreateInfo(PrimitiveType primitiveType)
    {
        VkPipelineInputAssemblyStateCreateInfo info{};
        info.sType      = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.pNext      = nullptr;
        info.flags      = 0;
        info.topology   = VulkanUtil::PrimitiveTypeToVkTopology(primitiveType);
        info.primitiveRestartEnable = VK_FALSE;

        return info;
    }

    inline VkPipelineRasterizationStateCreateInfo VulkanRHIGraphicsPipeline::GetRasterizationStateCreateInfo(const RHIRasterizerState& state)
    {
        VkPipelineRasterizationStateCreateInfo info{};
        info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.pNext                      = nullptr;
        info.flags                      = 0;
        info.depthClampEnable           = state.depthClipMode == RasterizerDepthClipMode::DepthClamp ? VK_TRUE : VK_FALSE;
        info.rasterizerDiscardEnable    = VK_FALSE;
        info.polygonMode                = VulkanUtil::RasterizerFillModeToVkPolygonMode(state.fillMode);
        info.cullMode                   = VulkanUtil::RasterizerCullModeToVkCullMode(state.cullMode);
        info.frontFace                  = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        info.depthBiasEnable            = VK_TRUE;
        info.depthBiasConstantFactor    = state.depthBias;
        info.depthBiasClamp             = 0.0f;
        info.depthBiasSlopeFactor       = state.slopeScaleDepthBias;
        info.lineWidth                  = 1.0f;

        return info;
    }
    
    inline VkPipelineMultisampleStateCreateInfo VulkanRHIGraphicsPipeline::GetMultisampleStateCreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.sampleShadingEnable = VK_FALSE;
        info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;     // TODO: MSAA
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;
        info.flags = 0;

        return info;
    }

    inline VkPipelineDepthStencilStateCreateInfo VulkanRHIGraphicsPipeline::GetDepthStencilStateCreateInfo(const RHIDepthStencilState& state)
    {
        VkPipelineDepthStencilStateCreateInfo info{};
        info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        info.pNext                  = nullptr;
        info.flags                  = 0;
        info.depthTestEnable        = state.bEnableDepthTest;
        info.depthWriteEnable       = state.bEnableDepthWrite;
        info.depthCompareOp         = VulkanUtil::CompareOpToVk(state.depthTest);

        // not implemented
        info.depthBoundsTestEnable  = VK_FALSE;
        info.stencilTestEnable      = VK_FALSE;
        info.front                  = {};
        info.back                   = {};
        info.minDepthBounds         = 0.f;
        info.maxDepthBounds         = 1.f;

        return info;
    }

    inline VkPipelineColorBlendStateCreateInfo VulkanRHIGraphicsPipeline::GetColorBlendStateCreateInfo(const RHIBlendState& state, uint32_t attachmentCount)
    {
        m_blendStates.clear();
        m_blendStates.reserve(attachmentCount);

        for (uint32_t i = 0; i < attachmentCount; ++i)
        {
            const RHIBlendState::RenderTarget& rt = state.renderTargets[i];

            VkPipelineColorBlendAttachmentState attachment{};
            attachment.blendEnable = rt.bEnable ? VK_TRUE : VK_FALSE;
            attachment.srcColorBlendFactor = VulkanUtil::BlendFactorToVk(rt.colorSrcBlend);
            attachment.dstColorBlendFactor = VulkanUtil::BlendFactorToVk(rt.colorDstBlend);
            attachment.colorBlendOp = VulkanUtil::BlendOpToVk(rt.colorBlendOp);
            attachment.srcAlphaBlendFactor = VulkanUtil::BlendFactorToVk(rt.alphaSrcBlend);
            attachment.dstAlphaBlendFactor = VulkanUtil::BlendFactorToVk(rt.alphaDstBlend);
            attachment.alphaBlendOp = VulkanUtil::BlendOpToVk(rt.alphaBlendOp);
            attachment.colorWriteMask = rt.colorWriteMask;
            m_blendStates.push_back(attachment);
        }

        VkPipelineColorBlendStateCreateInfo info{};
        info.sType              = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        info.pNext              = nullptr;
        info.flags              = 0;
        info.logicOpEnable      = VK_FALSE;
        info.logicOp            = VK_LOGIC_OP_COPY;
        info.attachmentCount    = attachmentCount;
        info.pAttachments       = m_blendStates.data();
        info.blendConstants;

        return info;
    }

    inline VkPipelineDynamicStateCreateInfo VulkanRHIGraphicsPipeline::GetDynamicStateCreateInfo()
    {
        VkPipelineDynamicStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info.dynamicStateCount = (uint32_t)m_dynamicStates.size();
        info.pDynamicStates = m_dynamicStates.data();

        return info;
    }

    void VulkanRHIGraphicsPipeline::Destroy()
    {
        vkDestroyPipeline(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
        vkDestroyPipelineLayout(VULKAN_RHI()->GetDevice(), m_layout, nullptr);
	}

}
