#include "PipelineBuilder.h"
#include <fmt/core.h>

vk::Pipeline PipelineBuilder::BuildPipeline(vk::Device device)
{
	// —— Create PipelineLayout ——
	vk::PipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.setSetLayouts(descriptorSetLayouts);
	if (!pushConstantRanges.empty()) {
		layoutInfo.setPushConstantRanges(pushConstantRanges);
	}
	pipelineLayout = device.createPipelineLayout(layoutInfo);

	// Viewport state
	vk::PipelineViewportStateCreateInfo viewportState{};
	viewportState.setViewportCount(1)
		.setScissorCount(1);

	// Color blending
	vk::PipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.setLogicOpEnable(false)
		.setLogicOp(vk::LogicOp::eCopy)
		.setAttachments(colorBlendAttachment);

	// Vertex input
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.setVertexBindingDescriptions(vertexInputBindings);
	vertexInputInfo.setVertexAttributeDescriptions(vertexInputAttributes);

	// Dynamic states
	std::array<vk::DynamicState, 2> dynamicStates =
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicInfo{};
	dynamicInfo.setDynamicStates(dynamicStates);

	// Pipeline create info
	vk::GraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.setPNext(&renderingCI)
		.setStages(shaderStages)
		.setPVertexInputState(&vertexInputInfo)
		.setPInputAssemblyState(&inputAssemblyCI)
		.setPViewportState(&viewportState)
		.setPRasterizationState(&rasterizationCI)
		.setPMultisampleState(&multisampleCI)
		.setPColorBlendState(&colorBlending)
		.setPDepthStencilState(&depthStencilCI)
		.setLayout(pipelineLayout)
		.setPDynamicState(&dynamicInfo);

	auto result = device.createGraphicsPipeline(
		nullptr,
		pipelineInfo
	);

	if (result.result != vk::Result::eSuccess)
	{
		fmt::println("failed to create pipeline");
		return nullptr;
	}

	return result.value;
}

void PipelineBuilder::SetShaders(vk::ShaderModule vertexShader, vk::ShaderModule fragmentShader)
{
	shaderStages.clear();

	vk::PipelineShaderStageCreateInfo vertexShaderStageCI;
	vertexShaderStageCI.flags = {};
	vertexShaderStageCI.stage = vk::ShaderStageFlagBits::eVertex;
	vertexShaderStageCI.module = vertexShader;
	vertexShaderStageCI.pName = "main";
	shaderStages.push_back(vertexShaderStageCI);

	vk::PipelineShaderStageCreateInfo fragmentShaderStageCI{};
	fragmentShaderStageCI.stage = vk::ShaderStageFlagBits::eFragment;
	fragmentShaderStageCI.module = fragmentShader;
	fragmentShaderStageCI.pName = "main";
	shaderStages.push_back(fragmentShaderStageCI);
}

void PipelineBuilder::SetInputTopology(vk::PrimitiveTopology topology)
{
	inputAssemblyCI.topology = topology;
	inputAssemblyCI.primitiveRestartEnable = false;
}

void PipelineBuilder::SetPolygonMode(vk::PolygonMode polygonMode)
{
	rasterizationCI.polygonMode = polygonMode;
	rasterizationCI.lineWidth = 1.0f;
}

void PipelineBuilder::SetCullMode(vk::CullModeFlags cullMode, vk::FrontFace frontFace)
{
	rasterizationCI.cullMode = cullMode;
	rasterizationCI.frontFace = frontFace;
}

void PipelineBuilder::SetMultisamplingNone()
{
	multisampleCI.sampleShadingEnable = false;
	multisampleCI.rasterizationSamples = vk::SampleCountFlagBits::e1;
	multisampleCI.minSampleShading = 1.0f;
	multisampleCI.pSampleMask = nullptr;
	multisampleCI.alphaToCoverageEnable = false;
	multisampleCI.alphaToOneEnable = false;
}

void PipelineBuilder::DisableBlending()
{
	colorBlendAttachment.colorWriteMask = 
		vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;

	colorBlendAttachment.blendEnable = false;
}

void PipelineBuilder::EnableBlendingAdditive()
{
	colorBlendAttachment.colorWriteMask =
		vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;

	colorBlendAttachment.blendEnable = true;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
}

void PipelineBuilder::EnableBlendingAlphaBlend()
{
	colorBlendAttachment.colorWriteMask = 
		vk::ColorComponentFlagBits::eR | 
		vk::ColorComponentFlagBits::eG | 
		vk::ColorComponentFlagBits::eB | 
		vk::ColorComponentFlagBits::eA;

	colorBlendAttachment.blendEnable = true;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
}

void PipelineBuilder::SetColorAttachmentFormat(vk::Format format)
{
	colorAttachmentFormat = format;

	renderingCI.colorAttachmentCount = 1;
	renderingCI.pColorAttachmentFormats = &colorAttachmentFormat;
}

void PipelineBuilder::SetDepthFormat(vk::Format format)
{
	depthAttachmentFormat = format;
	renderingCI.setDepthAttachmentFormat(depthAttachmentFormat);
}

void PipelineBuilder::SetDescriptorSetLayouts(std::vector<vk::DescriptorSetLayout> layouts)
{
	descriptorSetLayouts = std::move(layouts);
}

void PipelineBuilder::SetPushConstantRange(vk::ShaderStageFlags stages, uint32_t size)
{
	pushConstantRanges.clear();
	pushConstantRanges.push_back({stages, 0, size});
}

void PipelineBuilder::SetVertexInput(
	std::vector<vk::VertexInputBindingDescription> bindings,
	std::vector<vk::VertexInputAttributeDescription> attributes)
{
	vertexInputBindings = std::move(bindings);
	vertexInputAttributes = std::move(attributes);
}

void PipelineBuilder::EnableDepthTest(bool writeEnable, vk::CompareOp compareOp)
{
	depthStencilCI.depthTestEnable = true;
	depthStencilCI.depthWriteEnable = writeEnable;
	depthStencilCI.depthCompareOp = compareOp;
	depthStencilCI.depthBoundsTestEnable = false;
	depthStencilCI.stencilTestEnable = false;
	depthStencilCI.front = vk::StencilOpState{};
	depthStencilCI.back = vk::StencilOpState{};
	depthStencilCI.minDepthBounds = 0.f;
	depthStencilCI.maxDepthBounds = 1.f;
}

void PipelineBuilder::DisableDepthTest()
{
	depthStencilCI.depthTestEnable = false;
	depthStencilCI.depthWriteEnable = false;
	depthStencilCI.depthCompareOp = vk::CompareOp::eNever;
	depthStencilCI.depthBoundsTestEnable = VK_FALSE;
	depthStencilCI.stencilTestEnable = VK_FALSE;
	depthStencilCI.front = vk::StencilOpState{};
	depthStencilCI.back = vk::StencilOpState{};
	depthStencilCI.minDepthBounds = 0.f;
	depthStencilCI.maxDepthBounds = 1.f;
}
