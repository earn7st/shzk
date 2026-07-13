# Test Vulkan Initialization
add_executable(
    test_vulkan_rhi
    test_vulkan_rhi.cpp
)

target_link_libraries(
    test_vulkan_rhi
    PRIVATE
        vkR_runtime
)
