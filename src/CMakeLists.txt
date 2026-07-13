# =========================
# Core library
# =========================

add_library(
    vkR_runtime

    "runtime/core/Engine.cpp"
    "runtime/core/WindowSystem.cpp"

    "runtime/framework/component/TransformComponent.cpp"
    "runtime/framework/component/MeshComponent.cpp"

    "runtime/render/RenderSystem.cpp"

    "runtime/rhi/RHIDevice.cpp"
    "runtime/rhi/RHIUtil.cpp"
    
)

target_include_directories(
    vkR_runtime
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/extern
)

target_link_libraries(
    vkR_runtime
    PUBLIC
        Vulkan::Vulkan
        fmt::fmt
        vk-bootstrap
        imgui
        fastgltf::fastgltf
)

target_compile_definitions(
    vkR_runtime
    PUBLIC
        VK_USE_PLATFORM_WIN32_KHR
)

add_dependencies(
    vkR_runtime
    Shaders
)

# =========================
# Main executable
# =========================

add_executable(
    vkR_editor
    "editor/main.cpp"
    "editor/Editor.cpp"
)

target_link_libraries(
    vkR_editor
    PRIVATE
        vkR_runtime
)