CMAKE_MINIMUM_REQUIRED(VERSION 4.0.0)
PROJECT(moonstone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -I${CMAKE_SOURCE_DIR}/libs/include -fdiagnostics-color=always")
set(CMAKE_COLOR_DIAGNOSTICS ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()

add_compile_definitions(_SOLUTIONDIR="${CMAKE_SOURCE_DIR}")

find_package(imgui)
find_package(glm)
set(GLFWPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # disable building GLFWPP examples
SET(IMGUI_BUILD_GLFW_BINDING ON)
add_subdirectory(libs/glfwpp)
add_subdirectory(libs/glad)
add_subdirectory(libs/angelscript/sdk/angelscript/projects/cmake/)

set(SOURCES
    ./src/Main.cpp
    )

SET(MOONSTONE_SOURCES
    ./src/Moonstone.c++
    ./scenes/Scenes.c++
    ./src/external/External.c++
    ./src/Scene.cpp
    ./src/Window.cpp
    ./src/Renderer.cpp
    ./src/Shader.cpp
    ./src/Utility.cpp
    ./src/Texture.cpp
    ./src/Logging.cpp
    ./src/VertexBuffer.cpp
    ./src/VertexArray.cpp
    ./src/BufferLayout.cpp
    ./src/IndexBuffer.cpp
    ./src/external/ImGui.cpp
    ./scenes/SceneTexture.cpp
    ./scenes/SceneClearColor.cpp
    )

set(IMGUI_IMPL_SOURCES
    ./libs/imgui/imgui_impl_glfw.cpp
    ./libs/imgui/imgui_impl_opengl3.cpp
    )

add_library(imgui_impl ${IMGUI_IMPL_SOURCES})
target_link_libraries(imgui_impl PRIVATE imgui glad)
target_include_directories(imgui_impl PRIVATE ${CMAKE_SOURCE_DIR}/libs/imgui)

add_library(moonstone)
target_compile_features(moonstone PRIVATE cxx_std_23)
target_sources(moonstone PUBLIC FILE_SET CXX_MODULES FILES ${MOONSTONE_SOURCES})
target_include_directories(moonstone PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/imgui
        ${CMAKE_SOURCE_DIR}/libs/glad/include 
        ${CMAKE_SOURCE_DIR}/libs/glfwpp/include
        ${CMAKE_SOURCE_DIR}/libs/angelscript/sdk/angelscript/include
        ${CMAKE_SOURCE_DIR}/include 
    )
target_link_libraries(moonstone PRIVATE GLFWPP imgui_impl imgui glad angelscript ${glfw3_LIBRARIES} ${glm_LIBRARIES})

add_executable(game)
target_sources(game PRIVATE ${SOURCES})
target_compile_features(game PRIVATE cxx_std_23)
target_include_directories(game PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/imgui
        ${CMAKE_SOURCE_DIR}/libs/glfwpp/include
    )
target_link_libraries(game PRIVATE GLFWPP imgui moonstone)
