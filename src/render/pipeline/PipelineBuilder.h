#pragma once

#include <vulkan/vulkan.hpp>

class PipelineBuilder
{
public:
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCI;
    vk::PipelineRasterizationStateCreateInfo rasterizationCI;
    vk::PipelineMultisampleStateCreateInfo multisampleCI;
    vk::PipelineDepthStencilStateCreateInfo depthStencilCI;
    vk::PipelineRenderingCreateInfo renderingCI;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;

    vk::PipelineLayout pipelineLayout;

    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    std::vector<vk::PushConstantRange> pushConstantRanges;

    std::vector<vk::VertexInputBindingDescription> vertexInputBindings;
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributes;

    vk::Format colorAttachmentFormat;
    vk::Format depthAttachmentFormat;

public:
    void clear();

    vk::Pipeline BuildPipeline(vk::Device device);

    void SetShaders(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader);
    void SetInputTopology(vk::PrimitiveTopology topology);
    void SetPolygonMode(vk::PolygonMode polygonMode);
    void SetCullMode(vk::CullModeFlags cullMode, vk::FrontFace frontFace);
    void SetMultisamplingNone();
    void EnableBlendingAdditive();
    void EnableBlendingAlphaBlend();
    void DisableBlending();
    void SetColorAttachmentFormat(vk::Format format);
    void SetDepthFormat(vk::Format format);
    void EnableDepthTest(bool writeEnable, vk::CompareOp compareOp);
    void DisableDepthTest();

    void SetDescriptorSetLayouts(std::vector<vk::DescriptorSetLayout> layouts);
    void SetPushConstantRange(vk::ShaderStageFlags stages, uint32_t size);

    void SetVertexInput(
        std::vector<vk::VertexInputBindingDescription> bindings,
        std::vector<vk::VertexInputAttributeDescription> attributes);

};