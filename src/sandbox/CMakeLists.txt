add_executable(sandbox)
target_compile_features(sandbox PUBLIC cxx_std_23)
target_compile_options(sandbox PUBLIC "/MP")
#set(IMGUI_DIR D:/WorkSpace/MyFile/ThirdParty/imgui)
#set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

file(GLOB_RECURSE SANDBOX_CPP_FILES 
    *.h
    *.cpp 
    *.cxx
    *.c
    *.ixx
    )
#file(GLOB SANDBOX_IMGUI_CPP_FILES 
#    ${IMGUI_DIR}/*.cpp 
#    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
#    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3*.cpp
#    )
set(SANDBOX_CPP_FILES ${SANDBOX_CPP_FILES})

#list(REMOVE_ITEM SANDBOX_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/IlluminationWidget.cpp)



message("CMAKE_SOURCE_DIR " ${CMAKE_SOURCE_DIR})

#foreach(CPP_FILE ${SANDBOX_CPP_FILES})
#    message("found " ${CPP_FILE} )
#endforeach()



#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SANDBOX_CPP_FILES})
#foreach(CPP_FILE ${SANDBOX_CPP_FILES})
#    message("found " ${CPP_FILE} )
#endforeach()

target_include_directories(sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third-party)
target_sources(sandbox PUBLIC ${SANDBOX_CPP_FILES})


target_link_libraries(sandbox PUBLIC
    imgui
    #assimp-vc143-mt.lib
    opengl32.lib
#    kernel32.lib
#    user32.lib
#    gdi32.lib
#    shell32.lib
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


