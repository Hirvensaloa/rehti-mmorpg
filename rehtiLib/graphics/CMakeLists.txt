include(FetchContent)
set(DEPENDENCY_DIR "../../dependencies")

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw
	GIT_TAG 3eaf1255b29fdf5c2895856c7be7d7185ef2b241
)
FetchContent_MakeAvailable(glfw)

find_package(spirv-cross REQUIRED)
find_package(glslang 11.7.0 REQUIRED)
find_package(vulkan-memory-allocator 3.0.1 REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)

# Include directories required for declared library Components (Such as glfw3 and glslang)

target_include_directories(rehtiGraphics PUBLIC ${DEPENDENCY_DIR}/vulkan/include)
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

list(TRANSFORM GRAPHICS_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

target_sources(rehtiGraphics PRIVATE ${GRAPHICS_SOURCES})

target_link_libraries(rehtiGraphics PUBLIC
	${DEPENDENCY_DIR}/vulkan/lib/vulkan-1
	glfw
	spirv-cross::spirv-cross
 	glslang::glslang
 	vulkan-memory-allocator::vulkan-memory-allocator
	stb::stb
 	glm::glm
	)

target_include_directories(rehtiGraphics INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)