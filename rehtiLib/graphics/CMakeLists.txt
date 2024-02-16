add_library(rehtiGraphics "")

set(DEPENDENCY_DIR "${CMAKE_CURRENT_LIST_DIR}/../../third_party")
set(VALIDATION_LAYER_DIR "${vulkan-validationlayers_INCLUDE_DIR}/../${CMAKE_INSTALL_BINDIR}")
set($ENV{VK_LAYER_PATH} VALIDATION_LAYER_DIR)
set($ENV{VK_LOADER_LAYERS_ENABLE} "*validation")

find_package(glfw3 3.3.8 REQUIRED)
find_package(Vulkan 1.3.239.0 REQUIRED)
find_package(SPIRV-Tools 1.3.239.0 REQUIRED)
find_package(vulkan-memory-allocator 3.0.1 REQUIRED)
find_package(vulkan-validationlayers 1.3.239.0 REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)

set(ENABLE_CTEST OFF CACHE BOOL "" FORCE)
set(ALLOW_EXTERNAL_SPIRV_TOOLS ON CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEPENDENCY_DIR}/glslang ${DEPENDENCY_DIR}/glslang)

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
	GraphicsObjectManager.cpp
	GraphicsObjectManager.hpp
	DescriptorBuilder.hpp
	DescriptorBuilder.cpp
	Camera.hpp
	Camera.cpp
	BasicTypes.hpp
	GraphicsTypes.hpp
	GraphicsTypes.cpp
	AABB.hpp
	AABB.cpp
	RehtiGui.hpp
	RehtiGui.cpp
	TimerCallbackSystem.hpp
	TimerCallbackSystem.cpp
	shaders/testShader.vert
	shaders/testShader.frag
	shaders/objectShader.vert
	shaders/objectShader.frag
	shaders/characterShader.vert
	shaders/characterShader.frag
	shaders/mapShader.vert
	shaders/mapShader.frag
	../bindings/imgui_impl_glfw.cpp
	../bindings/imgui_impl_glfw.h
	../bindings/imgui_impl_vulkan.cpp
	../bindings/imgui_impl_vulkan.h
	)

list(TRANSFORM GRAPHICS_SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/src/)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../assets/generated/)

target_sources(rehtiGraphics PRIVATE ${GRAPHICS_SOURCES})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../utils ${CMAKE_CURRENT_BINARY_DIR}/build/rehtiutils)

find_package(assimp REQUIRED)
target_link_libraries(rehtiGraphics
	Vulkan::Vulkan
	Vulkan::Headers
	Vulkan::Registry
	glfw
	spirv-tools::spirv-tools
	${GLSLANG_LIBS}
 	vulkan-memory-allocator::vulkan-memory-allocator
	vulkan-validationlayers::vulkan-validationlayers
	stb::stb
 	glm::glm
	rehtiUtils
	imgui::imgui
	assimp::assimp
	)

target_include_directories(rehtiGraphics PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/shaders/)
target_include_directories(rehtiGraphics PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/)

target_include_directories(rehtiGraphics INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")