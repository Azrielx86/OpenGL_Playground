cmake_minimum_required(VERSION 3.28)
project(ShaderPlayground)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;NoSanitize" CACHE STRING "" FORCE)

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

    add_compile_definitions(-DWIN32)
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

if (UNIX)
    if (CMAKE_BUILD_TYPE STREQUAL "NoSanitize")
        add_compile_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized)
        add_link_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized)
    else ()
        add_compile_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized -fsanitize=address,undefined -fsanitize-trap)
        add_link_options(-Wall -Wextra -Wconversion -Wdouble-promotion -Wno-sign-conversion -Wno-unknown-pragmas -Wuninitialized -fsanitize=address,undefined -fsanitize-trap)
    endif ()
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Compiling Debug version.")
    add_compile_options(-O0 -gfull -DDEBUG)
else ()
    add_compile_options(-O3)
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
        src/Resources/ResourceManager.cpp
        src/Resources/ResourceManager.h
        src/ParticleInstancer.cpp
        src/ParticleInstancer.h
        src/Resources/Material.cpp
        src/Resources/Material.h
        src/Skybox.cpp
        src/Skybox.h
        src/Resources/Texture.cpp
        src/Resources/Texture.h
        src/Lights/Light.cpp
        src/Lights/Light.h
        src/Framebuffer.cpp
        src/Framebuffer.h
        src/Resources/Primitive.cpp
        src/Resources/Primitive.h
)

message(STATUS "Copying shader files.")
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

if (VCPKG_TOOLCHAIN)
    target_link_libraries(ShaderPlayground PRIVATE OpenGL::GL)
    target_include_directories(ShaderPlayground PRIVATE ${Stb_INCLUDE_DIR})
    target_link_libraries(ShaderPlayground PRIVATE imgui::imgui)
else ()
    target_link_libraries(ShaderPlayground PRIVATE OpenGL)
    target_link_libraries(ShaderPlayground PRIVATE imgui)
endif ()
target_link_libraries(ShaderPlayground PRIVATE glfw)
target_link_libraries(ShaderPlayground PRIVATE GLEW::GLEW)
target_link_libraries(ShaderPlayground PRIVATE glm::glm)
target_link_libraries(ShaderPlayground PRIVATE assimp::assimp)
