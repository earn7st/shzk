# =========================
# Core library
# =========================

add_library(
    SHZK_runtime

    "runtime/engine.cpp"
)

target_include_directories(
    SHZK_runtime
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/extern
)

# macOS: system-installed headers (glm, etc.) from Homebrew
if(APPLE)
    target_include_directories(SHZK_runtime PUBLIC /opt/homebrew/include)
endif()

target_link_libraries(
    SHZK_runtime
    PUBLIC
        Vulkan::Vulkan
        fmt::fmt
        vk-bootstrap
        imgui
        fastgltf::fastgltf
)

add_dependencies(
    SHZK_runtime
    Shaders
)

# =========================
# Editor executable
# =========================

add_executable(
    SHZK_editor

    "editor/main.cpp"
)

target_link_libraries(
    SHZK_editor
    PRIVATE
        SHZK_runtime
)