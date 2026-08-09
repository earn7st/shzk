# --- glTF Loader Test ---
add_executable(test_gltf
	test_gltf.cpp
)

target_compile_definitions(test_gltf PRIVATE
      SHZK_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/"
  )

target_link_libraries(test_gltf PRIVATE shzk_runtime)
