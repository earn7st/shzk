#include "DescriptorSetLayout.h"

#include <vector>
#include <vulkan/vulkan.hpp>

void DescriptorSetLayout::AddBinding(uint32_t index, vk::DescriptorType descType, vk::ShaderStageFlags shaderStage)
{
	vk::DescriptorSetLayoutBinding binding{};
	binding
		.setBinding(index)
		.setDescriptorType(descType)
		.setDescriptorCount(1)
		.setStageFlags(shaderStage);
	bindings.push_back(binding);
}

// Frame Data's Structure:
/*
layout(set = 0, binding = 0) uniform FrameData {

	mat4 view;
	mat4 proj; 
	mat4 viewProj;
	mat4 invViewProj;

	vec4 cameraPosition;      // .xyz = world-space
							// .w   = near plane distance
	vec4 time;                // .x = accumulated seconds
							// .y = delta seconds
							// .z = sin(elapsed) precompute
							// .w = frame counter (uint bits)
	vec4 resolution;          // .xy = viewport pixel size
							// .zw = 1.0 / viewport pixel size(fullscreen pass usually use)

	vec4 ambientColor;        // .rgb = color
							// .a= intensity

	vec4 sunDirection;        // .xyz  direction
							// .w  intensity
	vec4 sunColor;            // .rgb = color
							// .w   = reserved
} frameData;
*/

DescriptorSetLayout CreateDefaultFrameDescriptorLayout(vk::Device device)
{
	DescriptorSetLayout layout;
	layout.AddBinding(
		0,
		vk::DescriptorType::eUniformBuffer,
		vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
	
	vk::DescriptorSetLayoutCreateInfo descSetLayoutCI{};
	descSetLayoutCI.setBindings(layout.bindings);
	layout.descSetLayout = device.createDescriptorSetLayout(descSetLayoutCI);
	
	return layout;
}

// Material Data's Structure:
/*
struct MaterialData
{
	vec4 baseColorFactor;				// 16
	float metallicFactor;				// 20
	float roughnessFactor;				// 24
	float normalScale = 1.0;			// 28
	float occlusionStrength = 1.0;		// 32
	vec4 emmisiveFactor;				// 48
	float emissiveStrength;				// 52
	float alphaCutoff;					// 56
	uint32_t alphaMode;					// 60

	TextureHandle baseColorTexture;		
	TextureHandle normalTexture;
	TextureHandle metallicRoughnessTexture;
	TextureHandle occlusionTexture;	
	TextureHandle emissiveTexture;		// 64
}
*/

DescriptorSetLayout CreateDefaultMaterialDescriptorLayout(vk::Device device)
{
	DescriptorSetLayout layout;

	// binding = 0: MaterialUniforms UBO
	layout.AddBinding(
		0,
		vk::DescriptorType::eUniformBuffer,
		vk::ShaderStageFlagBits::eFragment);

	// binding = 1: baseColorSampler
	layout.AddBinding(
		1,
		vk::DescriptorType::eCombinedImageSampler,
		vk::ShaderStageFlagBits::eFragment);
	
	// binding = 2: normalSampler
	layout.AddBinding(
		2,
		vk::DescriptorType::eCombinedImageSampler,
		vk::ShaderStageFlagBits::eFragment);
	
	// binding = 3: metallicRoughnessSampler
	layout.AddBinding(
		3, 
		vk::DescriptorType::eCombinedImageSampler,
		vk::ShaderStageFlagBits::eFragment);

	// binding = 4: occlusionSampler
	layout.AddBinding(
		4,
		vk::DescriptorType::eCombinedImageSampler,
		vk::ShaderStageFlagBits::eFragment);

	// binding = 5: emissiveSampler
	layout.AddBinding(
		5,
		vk::DescriptorType::eCombinedImageSampler,
		vk::ShaderStageFlagBits::eFragment);

	vk::DescriptorSetLayoutCreateInfo descSetLayoutCI {};
	descSetLayoutCI.setBindings(layout.bindings);
	layout.descSetLayout = device.createDescriptorSetLayout(descSetLayoutCI);

	return layout;
}