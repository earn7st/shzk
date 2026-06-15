#pragma once

//STL
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <span>
#include <memory>
#include <functional>
#include <concepts>
#include <format>
#include <chrono>
#include <numeric>
#include <numbers>
#include <random>
#include <string_view>
#include <type_traits>
#include <mutex>
#include <stdexcept>
#include <future>
#include <array>
#include <deque>
#include <filesystem>
#include <optional>
#include <cstdint>

//GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// extern
#include <stb_image.h>

// Vulkan
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>
#include <vulkan/utility/vk_format_utils.h>

// SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

// vk-bootstrap
#include <vk-bootstrap/src/VkBootstrap.h>

// VMA
#include <vma/vk_mem_alloc.h>
