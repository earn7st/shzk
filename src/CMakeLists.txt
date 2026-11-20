# =========================
# Core library
# =========================

# headers：仅用于 IDE（Xcode/VS）导航显示，不参与编译
file(GLOB_RECURSE SHZK_RUNTIME_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/*.h")

set(SHZK_RUNTIME_SOURCES

    #core
    "runtime/core/Math.cpp"
    "runtime/core/Transform.cpp"

    # global
    "runtime/global/Engine.cpp"

    # import
    "runtime/import/GltfLoader.cpp"

    # input
    "runtime/input/InputSystem.cpp"

    # asset
    "runtime/asset/AssetManager.cpp"
    "runtime/asset/Model.cpp"
    "runtime/asset/Texture.cpp"
    "runtime/asset/Material.cpp"
    "runtime/asset/Shader.cpp"

    # framework
    "runtime/framework/components/TransformComponent.cpp"
    "runtime/framework/components/MeshComponent.cpp"
    "runtime/framework/components/CameraComponent.cpp"
    "runtime/framework/components/SkyBoxComponent.cpp"
    "runtime/framework/components/SkyLightComponent.cpp"
    "runtime/framework/components/DirectionalLightComponent.cpp"
    "runtime/framework/Node.cpp"
    "runtime/framework/Scene.cpp"

    # render
    "runtime/render/RenderSystem.cpp"
    "runtime/render/SceneRenderer.cpp"
    "runtime/render/MeshDrawCommand.cpp"

    "runtime/render/resources/Buffer.cpp"
    "runtime/render/resources/GraphicsPipelineCache.cpp"
    "runtime/render/resources/Sampler.cpp"
    "runtime/render/resources/RenderResourceManager.cpp"
    "runtime/render/resources/VertexFactory.cpp"

    "runtime/render/passes/MeshPass.cpp"
    "runtime/render/passes/MeshPassProcessor.cpp"
    "runtime/render/passes/ForwardPass.cpp"
    "runtime/render/passes/DepthPrePass.cpp"
    "runtime/render/passes/SkyPass.cpp"
    "runtime/render/passes/PostProcessPass.cpp"
    "runtime/render/passes/IBLPass.cpp"

    # rhi
    "runtime/rhi/RHI.cpp"
    "runtime/rhi/RHICommandList.cpp"
    "runtime/rhi/RHIResource.cpp"
    "runtime/rhi/RHIDefinitions.cpp"

    # vulkan rhi
    "runtime/rhi/vulkan/VulkanRHI.cpp"
    "runtime/rhi/vulkan/VulkanRHICommandPool.cpp"
    "runtime/rhi/vulkan/VulkanRHIFence.cpp"
    "runtime/rhi/vulkan/VulkanRHISemaphore.cpp"
    "runtime/rhi/vulkan/VulkanRHISurface.cpp"
    "runtime/rhi/vulkan/VulkanRHISwapchain.cpp"
    "runtime/rhi/vulkan/VulkanRHIQueue.cpp"
    "runtime/rhi/vulkan/VulkanRHIResource.cpp"

    # window
    "runtime/window/WindowSystem.cpp"
)

add_library(
    shzk_runtime
    ${SHZK_RUNTIME_SOURCES}
    ${SHZK_RUNTIME_HEADERS}
)

target_compile_definitions(shzk_runtime PUBLIC
    VK_NO_PROTOTYPES
    SHZK_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/"
    SHZK_SPIRV_DIR="${CMAKE_SOURCE_DIR}/bin/shaders/")
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
        mikktspace
)

add_dependencies(
    shzk_runtime
    Shaders
)

# =========================
# Editor executable
# =========================

set(SHZK_EDITOR_SOURCES

    "editor/Editor.cpp"
    "editor/Editor.h"
    "editor/main.cpp"
)

add_executable(
    shzk_editor

    ${SHZK_EDITOR_SOURCES}
)

target_link_libraries(
    shzk_editor
    PRIVATE
        shzk_runtime
)

# Xcode：由 CMake 生成共享 scheme，⌘R 可直接 build + run
# （Xcode 自动创建的 scheme 在 CONFIGURATION_BUILD_DIR 被重定向到 bin/<Config> 时
#   可能解析不到可执行文件，导致只 build 不 run）
set_target_properties(shzk_editor PROPERTIES
    XCODE_GENERATE_SCHEME YES
    XCODE_SCHEME_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Xcode/VS 导航器按真实目录树分组（替代平铺的 Source Files/Header Files）
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES
    ${SHZK_RUNTIME_SOURCES} ${SHZK_RUNTIME_HEADERS}
    ${SHZK_EDITOR_SOURCES})
