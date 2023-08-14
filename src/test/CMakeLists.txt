add_executable(test)
target_compile_features(test PUBLIC cxx_std_23)
target_compile_options(test PUBLIC "/MP" "/Zc:preprocessor")
#set(IMGUI_DIR D:/WorkSpace/MyFile/ThirdParty/imgui)
#set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)


file(GLOB_RECURSE TEST_CPP_FILES 
    *.cpp 
    *.cxx
    *.c
    *.ixx
    )
file(GLOB_RECURSE TEST_HXX_FILES 
    *.h
    *.hpp
)

set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Yc")
set(TEST_CPP_FILES ${TEST_CPP_FILES})



target_include_directories(test PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/magic_enum
)
target_sources(test PUBLIC ${TEST_CPP_FILES} ${TEST_HXX_FILES})


target_link_libraries(test PUBLIC
    imgui
    opengl32.lib
)

if(STB_PATH)
    message("found stb")
    target_include_directories(test PRIVATE ${STB_PATH})
else(STB_PATH)
    message(FATAL_ERROR "found stb failed! Check whether the stb submodule exists.")
endif(STB_PATH)

find_package(ASSIMP CONFIG REQUIRED)
if(ASSIMP_FOUND)
    message("found ASSIMP")
    target_include_directories(test PRIVATE ${ASSIMP_INCLUDE_DIR})
    target_link_libraries(test PRIVATE assimp::assimp)
else(ASSIMP_FOUND)
    message(WARNING "found ASSIMP failed!")
endif(ASSIMP_FOUND)


find_package(GLM CONFIG REQUIRED)
if(GLM_FOUND)
    message("found GLM")
    target_include_directories(test PRIVATE ${GLM_INCLUDE_DIR})
    target_link_libraries(test PRIVATE glm::glm)
else(GLM_FOUND)
    message(WARNING "found GLM failed!")
endif(GLM_FOUND)


find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(test PRIVATE glfw)

#if(GLFW_FOUND)
#    message("found glfw3")
#    target_include_directories(test PRIVATE glfw)
#    target_link_libraries(test PRIVATE glfw)
#else(GLFW_FOUND)
#    message(WARNING "found glfw3 failed!")
#endif(GLFW_FOUND)






# 开始组织文件结构

set(TEST_CXX_DIR_FILES ${TEST_CPP_FILES})
set(TEST_HXX_DIR_FILES ${TEST_HXX_FILES})


# utils文件夹


# 过滤出不包含在 TEST_UTILS_DIR_FILES 中的文件
foreach(TEST_EXCLUDE_FILE ${TEST_EXCLUDE_HXX_DIR_FILES})
file(GLOB FULL_DIR_FILES "${TEST_EXCLUDE_FILE}")
#list(FILTER TEST_HXX_DIR_FILES EXCLUDE REGEX "${FULL_DIR_FILES}")
list(REMOVE_ITEM TEST_HXX_DIR_FILES "${FULL_DIR_FILES}")
message("EXCLUDE " ${FULL_DIR_FILES} )
endforeach()

foreach(TEST_EXCLUDE_FILE ${TEST_EXCLUDE_CXX_DIR_FILES})
file(GLOB FULL_DIR_FILES "${TEST_EXCLUDE_FILE}")
#list(FILTER TEST_CXX_DIR_FILES EXCLUDE REGEX "${FULL_DIR_FILES}")
list(REMOVE_ITEM TEST_CXX_DIR_FILES "${FULL_DIR_FILES}")
message("EXCLUDE " ${FULL_DIR_FILES} )
endforeach()



source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${TEST_HXX_DIR_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${TEST_CXX_DIR_FILES})


message(${VCPKG_INSTALLED_DIR})

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/configure.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/configure.h"
)