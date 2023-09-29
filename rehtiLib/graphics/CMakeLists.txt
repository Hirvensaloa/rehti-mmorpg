#include(FetchContent)
set(DEPENDENCY_DIR "../../dependencies")

# Include directories required for declared library Components (Such as glfw3 and glslang)

target_include_directories(rehtiGraphics PRIVATE ${glfw3_INCLUDE_DIRS})
target_include_directories(rehtiGraphics PRIVATE ${glslang_INCLUDE_DIRS})
target_include_directories(rehtiGraphics PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/shaders/)


set(GRAPHICS_SOURCES
	base.cpp
	base.hpp
	ShaderManager.cpp
	ShaderManager.hpp
	shaders/shader.vert
	shaders/shader.frag
	)

set(GRAPHICS_LIBRARIES
	Vulkan::Vulkan
	Vulkan::Headers
	Vulkan::Registry
	glfw
	spirv-cross::spirv-cross
 	glslang::glslang
 	vulkan-memory-allocator::vulkan-memory-allocator
	stb::stb
 	glm::glm
	)

list(TRANSFORM GRAPHICS_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

target_sources(rehtiGraphics PRIVATE ${GRAPHICS_SOURCES})

target_link_libraries(rehtiGraphics PUBLIC
	GRAPHICS_LIBRARIES
	)

target_include_directories(rehtiGraphics INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)