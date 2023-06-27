add_executable(sandbox)
target_compile_features(sandbox PUBLIC cxx_std_23)

file(GLOB_RECURSE SANDBOX_CPP_FILES *.cpp)

message("CMAKE_SOURCE_DIR " ${CMAKE_SOURCE_DIR})
message("found " ${SANDBOX_CPP_FILES} )

target_sources(sandbox PUBLIC ${SANDBOX_CPP_FILES})
target_include_directories(sandbox PUBLIC
    C:/WorkSpace/MyFile/ThirdParty/glm
    C:/WorkSpace/MyFile/ThirdParty/Deps/Include
    D:/WorkSpace/MyFile/ThirdParty/imgui
    D:/WorkSpace/MyFile/ThirdParty/imgui/backends
    C:/WorkSpace/MyProgram/Install/Assimp/include
)
target_link_directories(sandbox PUBLIC
    C:/WorkSpace/MyFile/ThirdParty/Deps/Libs    
    C:/WorkSpace/MyProgram/Install/Assimp/lib
)
target_link_libraries(sandbox PUBLIC
    glfw3.lib
    assimp-vc143-mt.lib
    opengl32.lib
#    kernel32.lib
#    user32.lib
#    gdi32.lib
#    shell32.lib
)
