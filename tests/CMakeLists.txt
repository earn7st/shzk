# Test Vulkan Initialization
add_executable(
    test_vulkan_initialization
    test_vulkan_initialization.cpp
)

target_link_libraries(
    test_vulkan_initialization
    PRIVATE
        vkR_core
)

# Test Resource Manager
add_executable(
    test_gltf_loading
    test_gltf_loading.cpp
)

target_link_libraries(
    test_gltf_loading
    PRIVATE
        vkR_core
)
