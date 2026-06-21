#pragma once

#include "Resource.h"
#include <string>
#include <vulkan/vulkan.hpp>

// Texture resource
class Texture : public Resource {
private:
    // Core Vulkan GPU resources for texture representation
    vk::Image image;              // GPU image object containing pixel data
    vk::DeviceMemory memory;      // GPU memory allocation backing the image
    vk::DeviceSize offset;        // Offset within the memory allocation for this texture
    vk::ImageView imageView;      // Shader-accessible view into the image
    vk::Sampler sampler;          // Sampling configuration (filtering, wrapping, etc.)

    // Texture metadata for validation and debugging
    int width = 0;                // Image width in pixels
    int height = 0;               // Image height in pixels
    int channels = 0;             // Number of color channels (RGB=3, RGBA=4, etc.)

public:
    explicit Texture(const std::string& id) : Resource(id) {}

    ~Texture() override {
        doUnload();                // Ensure proper cleanup when object is destroyed
    }

    bool doLoad() override;

    void doUnload() override;

    // Public interface for accessing Vulkan resources safely
    vk::Image GetImage() const { return image; }
    vk::ImageView GetImageView() const { return imageView; }
    vk::Sampler GetSampler() const { return sampler; }

private:

    unsigned char* LoadImageData(const std::string& filePath, int* width, int* height, int* channels);
    void FreeImageData(unsigned char* data);
    void CreateVulkanImage(unsigned char* data, int width, int height, int channels);
    vk::Device GetDevice();
};

