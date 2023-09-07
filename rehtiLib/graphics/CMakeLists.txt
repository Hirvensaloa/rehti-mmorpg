add_subdirectory(../../dependencies/glfw ${CMAKE_CURRENT_LIST_DIR}/bin)

include_directories(../../dependencies/glfw/include)

target_include_directories(rehtiGraphics PUBLIC ../../dependencies/vma/include)
target_include_directories(rehtiGraphics PUBLIC ../../dependencies/stb/include)
target_include_directories(rehtiGraphics PUBLIC ../../dependencies/vulkan/include)

target_sources(rehtiGraphics
	PRIVATE src/base.cpp
		src/base.hpp
	)

target_link_libraries(rehtiGraphics ../../dependencies/vulkan/vulkan-1)
target_link_libraries(rehtiGraphics ../../dependencies/vulkan/shaderc_combined)

target_link_libraries(rehtiGraphics glfw ${GLFW_LIBRARIES})

target_include_directories(rehtiGraphics INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)