set(DEPENDENCY_DIR "../../dependencies")

add_subdirectory(${DEPENDENCY_DIR}/glfw ${CMAKE_CURRENT_LIST_DIR}/bin)

include_directories(${DEPENDENCY_DIR}/glfw/include)

target_include_directories(rehtiGraphics PUBLIC ${DEPENDENCY_DIR}/stb/include)
target_include_directories(rehtiGraphics PUBLIC ${DEPENDENCY_DIR}/vulkan/include)

target_sources(rehtiGraphics
	PRIVATE src/base.cpp
		src/base.hpp
	)

target_link_libraries(rehtiGraphics ${DEPENDENCY_DIR}/vulkan/lib/vulkan-1)
target_link_libraries(rehtiGraphics 
		optimized ${DEPENDENCY_DIR}/vulkan/lib/shaderc_combined 
		debug 	  ${DEPENDENCY_DIR}/vulkan/lib/shaderc_combinedd
		)

target_link_libraries(rehtiGraphics glfw ${GLFW_LIBRARIES})

target_include_directories(rehtiGraphics INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)