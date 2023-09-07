add_subdirectory(../../dependencies/glfw ${CMAKE_CURRENT_LIST_DIR}/bin)
include_directories(../../dependencies/glfw/include)

target_sources(rehtiGraphics
	PRIVATE src/base.cpp
		src/base.hpp
	)

target_link_libraries(rehtiGraphics glfw ${GLFW_LIBRARIES})

target_include_directories(rehtiGraphics INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)