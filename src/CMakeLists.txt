add_executable(vkR
    "main.cpp"
    "application/Application.h" 
    "application/Application.cpp" 
    "assets/AssetManager.h" 
    "assets/AssetManager.cpp"
    "assets/AssetTypes.h"
    "assets/GLTFLoader.h" 
    "assets/GLTFLoader.cpp" 
    "backend/VulkanContext.h" 
    "backend/VulkanContext.cpp"
    "render/Renderer.h" 
    "render/Renderer.cpp"
    "scene/Scene.h" 
    "scene/Scene.cpp" 
    "platform/SDLWindow.h"  
    "platform/SDLWindow.cpp" 
    "VKCommon.h"  "render/RenderResourceManager.h" "render/RenderResourceManager.cpp")

target_include_directories(
    vkR
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/extern
)

target_link_libraries(
    vkR
    PRIVATE
        Vulkan::Vulkan
        fmt::fmt
        vk-bootstrap
        imgui
        fastgltf::fastgltf
)

add_dependencies(
    vkR
    Shaders
)
