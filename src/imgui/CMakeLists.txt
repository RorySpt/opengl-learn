set(_target "imgui")

add_library(${_target})

file(GLOB SANDBOX_IMGUI_CPP_FILES 
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_opengl3*.cpp
    )
#message("search " SANDBOX_IMGUI_CPP_FILES )
#foreach(CPP_FILE ${SANDBOX_IMGUI_CPP_FILES})
#    message("found " ${CPP_FILE} )
#endforeach()

target_sources(
    ${_target} PRIVATE
    imconfig.h
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    ${SANDBOX_IMGUI_CPP_FILES}
)


target_include_directories(${_target} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/backends)

#find_package(Freetype REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
#find_package(glm CONFIG REQUIRED)

#target_include_directories(${_target} PRIVATE ${VCPKH})
target_link_libraries(${_target} PRIVATE glfw)
