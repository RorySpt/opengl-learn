add_executable(sandbox)
target_compile_features(sandbox PUBLIC cxx_std_23)
target_compile_options(sandbox PUBLIC "/MP" "/Zc:preprocessor")
#set(IMGUI_DIR D:/WorkSpace/MyFile/ThirdParty/imgui)
#set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)


file(GLOB_RECURSE SANDBOX_CPP_FILES 
    *.cpp 
    *.cxx
    *.c
    *.ixx
    )
file(GLOB_RECURSE SANDBOX_HXX_FILES 
    *.h
    *.hpp
)
#target_precompile_headers(sandbox PUBLIC stdafx.h)


#file(GLOB SANDBOX_IMGUI_CPP_FILES 
#    ${IMGUI_DIR}/*.cpp 
#    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
#    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3*.cpp
#    )

#foreach(CPP_FILE ${SANDBOX_CPP_FILES})
#    set_source_files_properties(${CPP_FILE} PROPERTIES COMPILE_FLAGS "/Yu")
#endforeach()
set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Yc")
set(SANDBOX_CPP_FILES ${SANDBOX_CPP_FILES})

message("CMAKE_SOURCE_DIR " ${CMAKE_SOURCE_DIR})


#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SANDBOX_CPP_FILES})
#foreach(CPP_FILE ${SANDBOX_CPP_FILES})
#    message("found " ${CPP_FILE} )
#endforeach()

target_include_directories(sandbox PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/magic_enum
)
target_sources(sandbox PUBLIC ${SANDBOX_CPP_FILES} ${SANDBOX_HXX_FILES})


target_link_libraries(sandbox PUBLIC
    imgui
    opengl32.lib
)


find_package(ASSIMP CONFIG REQUIRED)
if(ASSIMP_FOUND)
    message("found ASSIMP")
    target_include_directories(sandbox PRIVATE ${ASSIMP_INCLUDE_DIR})
    target_link_libraries(sandbox PRIVATE assimp::assimp)
else(ASSIMP_FOUND)
    message(WARNING "found ASSIMP failed!")
endif(ASSIMP_FOUND)


find_package(GLM CONFIG REQUIRED)
if(GLM_FOUND)
    message("found GLM")
    target_include_directories(sandbox PRIVATE ${GLM_INCLUDE_DIR})
    target_link_libraries(sandbox PRIVATE glm::glm)
else(GLM_FOUND)
    message(WARNING "found GLM failed!")
endif(GLM_FOUND)


find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(sandbox PRIVATE glfw)

#if(GLFW_FOUND)
#    message("found glfw3")
#    target_include_directories(sandbox PRIVATE glfw)
#    target_link_libraries(sandbox PRIVATE glfw)
#else(GLFW_FOUND)
#    message(WARNING "found glfw3 failed!")
#endif(GLFW_FOUND)






# 开始组织文件结构

set(SANDBOX_CXX_DIR_FILES ${SANDBOX_CPP_FILES})
set(SANDBOX_HXX_DIR_FILES ${SANDBOX_HXX_FILES})


# utils文件夹
set(SANDBOX_UTILS_HXX_DIR_FILES 
    euler_angle_utils.h
    frame_rate_calc.h
    "flags.h"
)
set(SANDBOX_UTILS_CXX_DIR_FILES 
    frame_rate_calc.cpp
    "flags.cpp"
)

source_group("Header Files/utils" FILES ${SANDBOX_UTILS_HXX_DIR_FILES})
source_group("Source Files/utils" FILES ${SANDBOX_UTILS_CXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_HXX_DIR_FILES ${SANDBOX_UTILS_HXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_CXX_DIR_FILES ${SANDBOX_UTILS_CXX_DIR_FILES})

# data文件夹
set(SANDBOX_DATA_HXX_DIR_FILES 
    vertices.h
)
set(SANDBOX_DATA_CXX_DIR_FILES 
    vertices.cpp
)
source_group("Header Files/data" FILES ${SANDBOX_DATA_HXX_DIR_FILES})
source_group("Source Files/data" FILES ${SANDBOX_DATA_CXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_HXX_DIR_FILES ${SANDBOX_DATA_HXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_CXX_DIR_FILES ${SANDBOX_DATA_CXX_DIR_FILES})



# render文件夹
set(SANDBOX_RENDER_HXX_DIR_FILES 
    igui.h
    demo0_rs.h
    test_render_surface.h
)
set(SANDBOX_RENDER_CXX_DIR_FILES 
    igui.cpp
    demo0_rs.cpp
    test_render_surface.cpp
)
source_group("Header Files/render" FILES ${SANDBOX_RENDER_HXX_DIR_FILES})
source_group("Source Files/render" FILES ${SANDBOX_RENDER_CXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_HXX_DIR_FILES ${SANDBOX_RENDER_HXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_CXX_DIR_FILES ${SANDBOX_RENDER_CXX_DIR_FILES})

# gameplay文件夹
file(GLOB SANDBOX_GAMEPLAY_HXX_DIR_FILES "*_component.h")
file(GLOB SANDBOX_GAMEPLAY_CXX_DIR_FILES "*_component.cpp")

list(PREPEND SANDBOX_GAMEPLAY_HXX_DIR_FILES 
    object.h
    world.h
    actor.h
    player_controller.h
)
list(PREPEND SANDBOX_GAMEPLAY_CXX_DIR_FILES 
    object.cpp
    world.cpp
    actor.cpp
    player_controller.cpp
)
source_group("Header Files/gameplay" FILES ${SANDBOX_GAMEPLAY_HXX_DIR_FILES})
source_group("Source Files/gameplay" FILES ${SANDBOX_GAMEPLAY_CXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_HXX_DIR_FILES ${SANDBOX_GAMEPLAY_HXX_DIR_FILES})
list(APPEND SANDBOX_EXCLUDE_CXX_DIR_FILES ${SANDBOX_GAMEPLAY_CXX_DIR_FILES})


# 过滤出不包含在 SANDBOX_UTILS_DIR_FILES 中的文件
foreach(SANDBOX_EXCLUDE_FILE ${SANDBOX_EXCLUDE_HXX_DIR_FILES})
file(GLOB FULL_DIR_FILES "${SANDBOX_EXCLUDE_FILE}")
#list(FILTER SANDBOX_HXX_DIR_FILES EXCLUDE REGEX "${FULL_DIR_FILES}")
list(REMOVE_ITEM SANDBOX_HXX_DIR_FILES "${FULL_DIR_FILES}")
message("EXCLUDE " ${FULL_DIR_FILES} )
endforeach()

foreach(SANDBOX_EXCLUDE_FILE ${SANDBOX_EXCLUDE_CXX_DIR_FILES})
file(GLOB FULL_DIR_FILES "${SANDBOX_EXCLUDE_FILE}")
#list(FILTER SANDBOX_CXX_DIR_FILES EXCLUDE REGEX "${FULL_DIR_FILES}")
list(REMOVE_ITEM SANDBOX_CXX_DIR_FILES "${FULL_DIR_FILES}")
message("EXCLUDE " ${FULL_DIR_FILES} )
endforeach()



source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${SANDBOX_HXX_DIR_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SANDBOX_CXX_DIR_FILES})


message(${VCPKG_INSTALLED_DIR})

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/configure.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/configure.h"
)