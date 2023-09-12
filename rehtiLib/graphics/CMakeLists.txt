set(DEPENDENCY_DIR "../../dependencies")


find_package(glfw3 3.3.8 REQUIRED)
find_package(spirv-tools REQUIRED)
find_package(spirv-cross REQUIRED)
find_package(glslang REQUIRED)
find_package(shaderc REQUIRED)
find_package(vulkan-memory-allocator REQUIRED)
find_package(stb REQUIRED)
find_package(glm REQUIRED)

target_include_directories(rehtiGraphics PUBLIC ${DEPENDENCY_DIR}/vulkan/include)

target_sources(rehtiGraphics
	PRIVATE src/base.cpp
		src/base.hpp
	)

target_link_libraries(rehtiGraphics ${DEPENDENCY_DIR}/vulkan/lib/vulkan-1)
conan_target_link_libraries(rehtiGraphics glfw)
conan_target_link_libraries(rehtiGraphics spirv-tools::spirv-tools)
conan_target_link_libraries(rehtiGraphics spirv-cross::spirv-cross)
conan_target_link_libraries(rehtiGraphics glslang::glslang)
conan_target_link_libraries(rehtiGraphics shaderc::shaderc)
conan_target_link_libraries(rehtiGraphics stb::stb)
conan_target_link_libraries(rehtiGraphics glm::glm)

target_include_directories(rehtiGraphics INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)