cmake_minimum_required(VERSION 3.28)
project(ShaderPlayground)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;NoSanitize" CACHE STRING "" FORCE)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

FetchContent_Declare(imgui_external
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.tar.gz
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

add_library(imgui
        ${imgui_external_SOURCE_DIR}/imgui.cpp
        ${imgui_external_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_external_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
        src/Entities/Particle.cpp
        src/Entities/Particle.h
)

target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR})
include_directories(${imgui_external_SOURCE_DIR})
include_directories(${imgui_external_SOURCE_DIR}/backends)
include_directories("/usr/include/stb")

if (CMAKE_BUILD_TYPE STREQUAL "NoSanitize")
    add_compile_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized)
    add_link_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized)
else ()
    add_compile_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized -fsanitize=address,undefined -fsanitize-trap)
    add_link_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized -fsanitize=address,undefined -fsanitize-trap)
endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Compiling Debug version.")
    add_compile_options(-g3)
else ()
    add_compile_options(-O2)
endif ()


add_executable(ShaderPlayground src/main.cpp
        src/Window.cpp
        src/Shader.cpp
        src/Shader.h
        src/Camera.cpp
        src/Input/Keyboard.cpp
        src/Input/Keyboard.h
        src/Mesh.cpp
        src/Mesh.h
        src/Model.cpp
        src/Model.h
        src/Input/Mouse.cpp
        src/Input/Mouse.h
        src/GlobalDefines.h
        src/ResourceManager.cpp
        src/ResourceManager.h
        src/ParticleInstancer.cpp
        src/ParticleInstancer.h
)

message(STATUS "Copying shader files.")
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(ShaderPlayground PRIVATE OpenGL)
target_link_libraries(ShaderPlayground PRIVATE GLEW::GLEW)
target_link_libraries(ShaderPlayground PRIVATE glfw)
target_link_libraries(ShaderPlayground PRIVATE glm::glm)
target_link_libraries(ShaderPlayground PRIVATE assimp::assimp)
target_link_libraries(ShaderPlayground PRIVATE imgui)