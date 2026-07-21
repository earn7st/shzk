cmake_minimum_required(VERSION 3.22)

project(vkR LANGUAGES C CXX)

#
# C++ Settings
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# Platform Detection
#
if(WIN32)
    set(VKR_PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    set(VKR_PLATFORM_MACOS TRUE)
    # Ensure Homebrew-installed packages are discoverable
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
elseif(UNIX)
    set(VKR_PLATFORM_LINUX TRUE)
endif()

#
# Vulkan Platform Surface Extension
# Must be set globally so all targets (including extern/) see the correct type.
#
if(VKR_PLATFORM_WINDOWS)
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
elseif(VKR_PLATFORM_MACOS)
    add_compile_definitions(VK_USE_PLATFORM_METAL_EXT)
endif()

#
# Packages
#
find_package(Vulkan REQUIRED)

#
# SDL3
#
if(VKR_PLATFORM_MACOS)
    # macOS: SDL3 installed via Homebrew, provides full CMake config
    find_package(SDL3 REQUIRED CONFIG)
elseif(VKR_PLATFORM_WINDOWS)
    # Windows: SDL3 bundled with the Vulkan SDK
    if(NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR
            "VULKAN_SDK environment variable is not set. "
            "Please install the Vulkan SDK from https://vulkan.lunarg.com/")
    endif()
    add_library(SDL3::SDL3 SHARED IMPORTED)
    set_target_properties(SDL3::SDL3 PROPERTIES
        IMPORTED_IMPLIB
            "$ENV{VULKAN_SDK}/Lib/SDL3.lib"
        IMPORTED_LOCATION
            "$ENV{VULKAN_SDK}/Bin/SDL3.dll"
        INTERFACE_INCLUDE_DIRECTORIES
            "$ENV{VULKAN_SDK}/Include"
    )
else()
    # Linux/other: attempt to find via config
    find_package(SDL3 REQUIRED CONFIG)
endif()

#
# Output Directories
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/bin)

#
# Third Party Libraries
#
add_subdirectory(extern)

#
# Engine
#
add_subdirectory(src)

#
# Tests
#
add_subdirectory(tests)

#
# Shader Compilation
#
find_program(
    GLSLANG_VALIDATOR
    NAMES glslangValidator
    HINTS
        $ENV{VULKAN_SDK}/Bin
        $ENV{VULKAN_SDK}/Bin32
        /usr/local/bin
        /opt/homebrew/bin
)

if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR
        "glslangValidator not found. Please install the Vulkan SDK "
        "or ensure glslangValidator is available in your PATH.\n"
        "  - Windows: Install from https://vulkan.lunarg.com/\n"
        "  - macOS:   brew install glslang  OR install the Vulkan SDK")
endif()

file(MAKE_DIRECTORY
    "${PROJECT_SOURCE_DIR}/bin/shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
)

set(SPIRV_BINARY_FILES)

foreach(GLSL ${GLSL_SOURCE_FILES})

    get_filename_component(FILE_NAME ${GLSL} NAME)

    set(SPIRV
        "${PROJECT_SOURCE_DIR}/bin/shaders/${FILE_NAME}.spv"
    )

    add_custom_command(
        OUTPUT ${SPIRV}

        COMMAND
            ${GLSLANG_VALIDATOR}
            -V
            ${GLSL}
            -o
            ${SPIRV}

        DEPENDS ${GLSL}

        COMMENT "Compiling shader ${FILE_NAME}"

        VERBATIM
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})

endforeach()

add_custom_target(
    Shaders ALL
    DEPENDS ${SPIRV_BINARY_FILES}
)