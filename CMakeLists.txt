CMAKE_MINIMUM_REQUIRED(VERSION 4.0.0)
PROJECT(moonstone LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

string(CONCAT CMAKE_CXX_FLAGS "-stdlib=libc++")

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
    ./src/external/External.c++
    ./scenes/Scenes.c++
    ./src/Scene.cpp
    ./src/Window.cpp
    ./src/Utility.cpp
    ./src/Logging.cpp
    # External Module
    ./src/external/ImGui.cpp
    # Renderer Module
    ./src/renderer/Shader.cpp
    ./src/renderer/VertexElement.cpp
    ./src/renderer/VertexBuffer.cpp
    ./src/renderer/VertexArray.cpp
    ./src/renderer/BufferLayout.cpp
    ./src/renderer/IndexBuffer.cpp
    ./src/renderer/Texture.cpp
    ./src/renderer/Renderer.cpp
    # Engine Module
    ./src/engine/quad.cpp
    # Scenes Module
    ./scenes/SceneTexture.cpp
    ./scenes/SceneClearColor.cpp
    )

# ImGui Setup library
set(IMGUI_IMPL_SOURCES
    ./libs/imgui/imgui_impl_glfw.cpp
    ./libs/imgui/imgui_impl_opengl3.cpp
    )
add_library(imgui_impl ${IMGUI_IMPL_SOURCES})
target_link_libraries(imgui_impl PRIVATE imgui glad)
target_include_directories(imgui_impl PRIVATE ${CMAKE_SOURCE_DIR}/libs/imgui)

# Main game engine library
add_library(moonstone)
target_compile_features(moonstone PRIVATE cxx_std_23)
target_sources(moonstone PUBLIC FILE_SET CXX_MODULES FILES ${MOONSTONE_SOURCES})
target_link_libraries(moonstone PRIVATE GLFWPP imgui_impl imgui glad angelscript ${glfw3_LIBRARIES} ${glm_LIBRARIES})
target_compile_definitions(moonstone PRIVATE $<$<CONFIG:Debug>:_DEBUG> _ROOTDIR="${CMAKE_SOURCE_DIR}")
target_compile_options(moonstone PRIVATE $<$<CONFIG:Debug>:-fno-omit-frame-pointer -fsanitize=address>)
target_link_options(moonstone PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_include_directories(moonstone SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/imgui
        ${CMAKE_SOURCE_DIR}/libs/glad/include 
        ${CMAKE_SOURCE_DIR}/libs/glfwpp/include
        ${CMAKE_SOURCE_DIR}/libs/angelscript/sdk/angelscript/include
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include 
    )

# Main executable
add_executable(game)
target_sources(game PRIVATE ${SOURCES})
target_compile_features(game PRIVATE cxx_std_23)
target_compile_options(game PRIVATE $<$<CONFIG:Debug>:-fno-omit-frame-pointer -fsanitize=address>)
target_link_options(game PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_link_libraries(game PRIVATE GLFWPP imgui moonstone)
target_include_directories(game SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/imgui
        ${CMAKE_SOURCE_DIR}/libs/glfwpp/include
    )
