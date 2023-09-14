set(DEPENDENCY_DIR "../../dependencies")


find_package(glfw3 3.3.8 REQUIRED)
find_package(spirv-cross REQUIRED)
find_package(glslang 11.7.0 REQUIRED)
find_package(vulkan-memory-allocator 3.0.1 REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)


# message("Glfw include dirs: ${glfw3_INCLUDE_DIRS}")


target_include_directories(rehtiGraphics PUBLIC ${DEPENDENCY_DIR}/vulkan/include)
target_include_directories(rehtiGraphics PUBLIC ${glfw3_INCLUDE_DIRS})
target_include_directories(rehtiGraphics PRIVATE ${glslang_INCLUDE_DIRS})

target_sources(rehtiGraphics
	PRIVATE src/base.cpp
		src/base.hpp
	)

target_link_libraries(rehtiGraphics PUBLIC ${DEPENDENCY_DIR}/vulkan/lib/vulkan-1)
target_link_libraries(rehtiGraphics PUBLIC glfw)
target_link_libraries(rehtiGraphics PRIVATE spirv-cross::spirv-cross)
target_link_libraries(rehtiGraphics PRIVATE glslang::glslang)
target_link_libraries(rehtiGraphics PRIVATE vulkan-memory-allocator::vulkan-memory-allocator)
target_link_libraries(rehtiGraphics PRIVATE stb::stb)
target_link_libraries(rehtiGraphics PRIVATE glm::glm)

target_include_directories(rehtiGraphics INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)