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

    VulkanRHIDescriptorSet::VulkanRHIDescriptorSet(VulkanRHI& rhi, VkDescriptorSetLayout layout)
		: RHIDescriptorSet()
    {
		VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.pSetLayouts = &layout;
        // TODO
        vkAllocateDescriptorSets(rhi.GetDevice(), &allocInfo, &m_handle);
    }

    void VulkanRHIDescriptorSet::Destroy()
    {
		vkFreeDescriptorSets(VULKAN_RHI()->GetDevice(), VULKAN_RHI()->GetDescriptorPool(), 1, &m_handle);
    }

    VulkanRHIGraphicsPipeline::VulkanRHIGraphicsPipeline(const RHIGraphicsPipelineInfo& info, VulkanRHI& rhi)
        : RHIGraphicsPipeline(info)
    {
		VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        
        /*
        *
    typedef struct VkGraphicsPipelineCreateInfo {
    VkStructureType                                  sType;
    const void*                                      pNext;
    VkPipelineCreateFlags                            flags;
    uint32_t                                         stageCount;
    const VkPipelineShaderStageCreateInfo*           pStages;
    const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
    const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
    const VkPipelineTessellationStateCreateInfo*     pTessellationState;
    const VkPipelineViewportStateCreateInfo*         pViewportState;
    const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
    const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
    const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
    const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
    const VkPipelineDynamicStateCreateInfo*          pDynamicState;
    VkPipelineLayout                                 layout;
    VkRenderPass                                     renderPass;
    uint32_t                                         subpass;
    VkPipeline                                       basePipelineHandle;
    int32_t                                          basePipelineIndex;
} VkGraphicsPipelineCreateInfo;
        */
    }

    void VulkanRHIGraphicsPipeline::Destroy()
    {
        vkDestroyPipeline(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
        vkDestroyPipelineLayout(VULKAN_RHI()->GetDevice(), m_layout, nullptr);
	}

    

}
