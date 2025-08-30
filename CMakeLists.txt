cmake_minimum_required(VERSION 4.0)
project(OpenGLPlayground)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wno-nontrivial-memcall")

if (NOT UNIX)
    message(STATUS "Compiling Windows version")
    add_compile_definitions(-DWIN32)
endif ()

add_subdirectory(AzxEngineGL)

if (VCPKG_TOOLCHAIN)
    message(STATUS "Using vcpkg Package Manager")

    set(OpenGL_GL_PREFERENCE GLVND)

    find_package(glm CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(assimp CONFIG REQUIRED)
    find_package(Stb REQUIRED)
    find_package(imgui CONFIG REQUIRED)

    include_directories(${Stb_INCLUDE_DIR})
else ()
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(glm REQUIRED)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # assimp
        FetchContent_Declare(assimp
                GIT_REPOSITORY https://github.com/assimp/assimp.git
                GIT_TAG v5.4.3)
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
        set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(assimp)
    else ()
        find_package(assimp REQUIRED)
    endif ()

    # imgui
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
    )

    target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR})
    include_directories(${imgui_external_SOURCE_DIR})
    include_directories(${imgui_external_SOURCE_DIR}/backends)
    include_directories("/usr/include/stb")
endif ()

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

target_link_libraries(OpenGLPlayground AzxEngineGL)
