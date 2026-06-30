#include "Pipeline.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include "render/pipeline/PipelineBuilder.h"
#include "resource/cpu_types/Vertex.h"
#include "vulkan/VulkanHelpers.h"

Pipeline CreateDefaultPipeline(
	vk::Device device,
	vk::DescriptorSetLayout frameLayout,
	vk::DescriptorSetLayout materialLayout,
	vk::Format colorFormat,
	vk::Format depthFormat)
{
	// 1. Load SPIR-V shaders
	vk::ShaderModule vertShader = LoadShaderModule(device, "shaders/mesh.vert.spv");
	vk::ShaderModule fragShader = LoadShaderModule(device, "shaders/mesh.frag.spv");

	// 2. Configure pipeline builder
	PipelineBuilder builder;

	builder.SetShaders(vertShader, fragShader);
	builder.SetInputTopology(vk::PrimitiveTopology::eTriangleList);
	builder.SetPolygonMode(vk::PolygonMode::eFill);
	builder.SetCullMode(vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise);
	builder.SetMultisamplingNone();
	builder.DisableBlending();
	builder.SetColorAttachmentFormat(colorFormat);
	builder.SetDepthFormat(depthFormat);
	builder.EnableDepthTest(true, vk::CompareOp::eLessOrEqual);

	// Set=0 (FrameData) + Set=1 (Material)
	builder.SetDescriptorSetLayouts({frameLayout, materialLayout});

	// Push constants: mat4 modelMatrix only (no buffer address needed)
	builder.SetPushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(glm::mat4));

	// Vertex input: 1 binding, 4 attributes matching C++ Vertex struct
	vk::VertexInputBindingDescription binding{};
	binding.setBinding(0);
	binding.setStride(sizeof(Vertex));
	binding.setInputRate(vk::VertexInputRate::eVertex);

	std::vector<vk::VertexInputAttributeDescription> attributes = {
		{0, 0, vk::Format::eR32G32B32Sfloat,    offsetof(Vertex, position)},
		{1, 0, vk::Format::eR32G32B32Sfloat,    offsetof(Vertex, normal)},
		{2, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, tangent)},
		{3, 0, vk::Format::eR32G32Sfloat,       offsetof(Vertex, uv)},
	};

	builder.SetVertexInput({binding}, attributes);

	// 3. Build
	Pipeline pipeline;
	pipeline.pipeline = builder.BuildPipeline(device);
	pipeline.pipelineLayout = builder.pipelineLayout;
	pipeline.descriptorSetLayouts = {frameLayout, materialLayout};

	// 4. Shader modules no longer needed after pipeline creation
	device.destroyShaderModule(vertShader);
	device.destroyShaderModule(fragShader);

	return pipeline;
}