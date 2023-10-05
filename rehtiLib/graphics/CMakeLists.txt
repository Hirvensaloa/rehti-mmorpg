set(DEPENDENCY_DIR "${CMAKE_CURRENT_LIST_DIR}/../../third_party")

set(ENABLE_CTEST OFF CACHE BOOL "" FORCE) # possibly enables SPIRV

add_subdirectory(${DEPENDENCY_DIR}/glslang ${DEPENDENCY_DIR}/glslang)

find_package(glfw3 3.3.8 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(vulkan-memory-allocator 3.0.1 REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)

set(GLSLANG_LIBS
    	glslang
    	HLSL
    	OSDependent
    	OGLCompiler
    	SPIRV
	glslang-default-resource-limits
	)

set(GRAPHICS_SOURCES
	RehtiGraphics.cpp
	RehtiGraphics.hpp
	ShaderManager.cpp
	ShaderManager.hpp
	shaders/shader.vert
	shaders/shader.frag
	)

list(TRANSFORM GRAPHICS_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)

target_sources(rehtiGraphics PRIVATE ${GRAPHICS_SOURCES})

target_link_libraries(rehtiGraphics
	Vulkan::Vulkan
	Vulkan::Headers
	Vulkan::Registry
	glfw
	${GLSLANG_LIBS}
 	vulkan-memory-allocator::vulkan-memory-allocator
	stb::stb
 	glm::glm
	)

target_include_directories(rehtiGraphics PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/shaders/)

target_include_directories(rehtiGraphics INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")