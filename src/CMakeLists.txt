# =========================
# Core library
# =========================

add_library(
    shzk_runtime

    # global
    "runtime/global/Engine.cpp"

    # import
    "runtime/import/GltfLoader.cpp"

    # asset
    "runtime/asset/AssetManager.cpp"
    "runtime/asset/Model.cpp"
    "runtime/asset/Texture.cpp"
    "runtime/asset/Material.cpp"
    "runtime/asset/Shader.cpp"

    # framework
    "runtime/framework/components/TransformComponent.cpp"
    "runtime/framework/components/MeshComponent.cpp"
    "runtime/framework/Node.cpp"
    "runtime/framework/Scene.cpp"
    
    # render
    "runtime/render/RenderSystem.cpp"
    "runtime/render/MeshPassProcessor.cpp"
    "runtime/render/resources/Buffer.cpp"
    "runtime/render/resources/RenderResourceManager.cpp"
    
    # rhi
    "runtime/rhi/RHI.cpp"
    "runtime/rhi/RHICommandList.cpp"
    "runtime/rhi/RHIResource.cpp"

    # vulkan rhi
    "runtime/rhi/vulkan/VulkanRHI.cpp"
    "runtime/rhi/vulkan/VulkanRHICommandPool.cpp"
    
    
    "runtime/rhi/vulkan/VulkanRHIFence.cpp"
    "runtime/rhi/vulkan/VulkanRHISemaphore.cpp"
    "runtime/rhi/vulkan/VulkanRHISurface.cpp"
    "runtime/rhi/vulkan/VulkanRHISwapchain.cpp"
    "runtime/rhi/vulkan/VulkanRHIQueue.cpp"
    "runtime/rhi/vulkan/VulkanRHIResource.cpp"

    # others in runtime
    "runtime/WindowSystem.cpp"

)

target_compile_definitions(shzk_runtime PUBLIC 
    VK_NO_PROTOTYPES
    SHZK_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/")
if(WIN32)
    target_compile_definitions(shzk_runtime PUBLIC NOMINMAX)
endif()

target_include_directories(
    shzk_runtime
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/extern
)


# macOS: system-installed headers (glm, etc.) from Homebrew
if(APPLE)
    target_include_directories(shzk_runtime PUBLIC /opt/homebrew/include)
    # VMA (VulkanMemoryAllocator) triggers nullability warnings on Apple Clang
    target_compile_options(shzk_runtime PRIVATE -Wno-nullability-completeness)
endif()

target_link_libraries(
    shzk_runtime
    PUBLIC
        Vulkan::Vulkan
        fmt::fmt
        vk-bootstrap
        imgui
        fastgltf::fastgltf
        stb
)

add_dependencies(
    shzk_runtime
    Shaders
)

# =========================
# Editor executable
# =========================

add_executable(
    shzk_editor
    
    "editor/Editor.cpp"
    "editor/main.cpp"
)

target_link_libraries(
    shzk_editor
    PRIVATE
        shzk_runtime
)