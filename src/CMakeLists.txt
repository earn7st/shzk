# =========================
# Core library
# =========================

add_library(
    vkR_core

    "application/Application.cpp"
    "application/SDLWindow.cpp"

    "vulkan/SwapchainContext.cpp"
    "vulkan/VulkanContext.cpp"
    "vulkan/UploadContext.cpp"
    "vulkan/VulkanHelpers.cpp"

    "gltf/gltfLoader.cpp"

    "resource/ResourceManager.cpp"

    "render/Renderer.cpp"

    "scene/Scene.cpp"
)

target_include_directories(
    vkR_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/extern
)

target_link_libraries(
    vkR_core
    PUBLIC
        Vulkan::Vulkan
        fmt::fmt
        vk-bootstrap
        imgui
        fastgltf::fastgltf
)

target_compile_definitions(
    vkR_core
    PUBLIC
        VK_USE_PLATFORM_WIN32_KHR
)

add_dependencies(
    vkR_core
    Shaders
)

# =========================
# Main executable
# =========================

add_executable(
    vkR
    main.cpp
)

target_link_libraries(
    vkR
    PRIVATE
        vkR_core
)