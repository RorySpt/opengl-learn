cmake_minimum_required(VERSION 3.20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(opengl-learn
    VERSION 1.0
    DESCRIPTION "Individual learning project of opengl"
    LANGUAGES C CXX
)

set(PROJECT_SHADER_PATH "${CMAKE_SOURCE_DIR}/data/shader")
set(STB_PATH "${CMAKE_SOURCE_DIR}/extern/stb")
set(GLM_PATH "${CMAKE_SOURCE_DIR}/extern/glm")
set(JSON_PATH "${CMAKE_SOURCE_DIR}/extern/json")
add_subdirectory(src)