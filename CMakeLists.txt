cmake_minimum_required(VERSION 4.0)
project(OpenGLPlayground)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wno-nontrivial-memcall")

if (NOT UNIX)
    message(STATUS "Compiling Windows version")
    add_compile_definitions(-DWIN32)
endif ()

if (VCPKG_TOOLCHAIN)
    message(STATUS "Using vcpkg Package Manager")
    find_package(imgui CONFIG REQUIRED)
else ()
    add_library(imgui
            ${imgui_external_SOURCE_DIR}/imgui.cpp
            ${imgui_external_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_external_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_external_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )

    target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR})
    include_directories(${imgui_external_SOURCE_DIR})
    include_directories(${imgui_external_SOURCE_DIR}/backends)
    include_directories("/usr/include/stb")
endif ()

add_subdirectory(AzxEngineGL)

add_compile_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized)
add_link_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Compiling Debug version.")
    add_compile_options(-O0 -gfull -DDEBUG -fno-inline -fstack-protector-all -fsanitize=address,undefined -fsanitize-trap)
    add_link_options(-O0 -gfull -DDEBUG -fno-inline -fstack-protector-all -fsanitize=address,undefined -fsanitize-trap)
else ()
    add_compile_options(-O3)
endif ()

add_executable(OpenGLPlayground main.cpp)

message(STATUS "Copying shader files.")
file(COPY shaders DESTINATION ${PROJECT_BINARY_DIR})
message(STATUS "Copying assets files.")
file(COPY assets DESTINATION ${PROJECT_BINARY_DIR})

target_link_libraries(OpenGLPlayground PRIVATE AzxEngineGL)
if (VCPKG_TOOLCHAIN)
    target_link_libraries(OpenGLPlayground PRIVATE imgui::imgui)
else ()
    target_link_libraries(OpenGLPlayground PRIVATE imgui)
endif ()

