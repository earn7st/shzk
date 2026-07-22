# =========================
# Core library
# =========================

add_library(
    shzk_runtime

    "runtime/WindowSystem.cpp"
    "runtime/global/Engine.cpp"
    "runtime/render/RenderSystem.cpp"

)

target_include_directories(
    shzk_runtime
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/extern
)

# macOS: system-installed headers (glm, etc.) from Homebrew
if(APPLE)
    target_include_directories(shzk_runtime PUBLIC /opt/homebrew/include)
endif()

target_link_libraries(
    shzk_runtime
    PUBLIC
        Vulkan::Vulkan
        fmt::fmt
        vk-bootstrap
        imgui
        fastgltf::fastgltf
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