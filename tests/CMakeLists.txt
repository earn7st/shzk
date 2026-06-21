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

# Test ECS
add_executable(
    test_ecs
    test_ecs.cpp
)

target_link_libraries(
    test_ecs
    PRIVATE
        vkR_core
)

# Test Resource Manager
add_executable(
    test_resource_manager
    test_resource_manager.cpp
)

target_link_libraries(
    test_resource_manager
    PRIVATE
        vkR_core
)
