CMAKE_MINIMUM_REQUIRED(VERSION 4.0.0)
PROJECT(moonstone LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(GLFWPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # disable building GLFWPP examples
SET(IMGUI_BUILD_GLFW_BINDING ON)
add_subdirectory(libs/glfwpp)
add_subdirectory(libs/glad)
add_subdirectory(libs/angelscript/sdk/angelscript/projects/cmake/)
add_subdirectory(libs/doctest)
add_subdirectory(libs/glm)

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
    ./src/Error.cpp
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
    ./src/renderer/TextureArray.cpp
    ./src/renderer/Call.cpp
    # Engine Module
    ./src/engine/quad.cpp
    # Scenes Module
    ./scenes/SceneTexture.cpp
    ./scenes/SceneClearColor.cpp
    )

# ImGui Setup library
set(IMGUI_SOURCES
    ./libs/imgui/backends/imgui_impl_glfw.cpp
    ./libs/imgui/backends/imgui_impl_opengl3.cpp
    ./libs/imgui/imgui.cpp
    ./libs/imgui/imgui_demo.cpp
    ./libs/imgui/imgui_draw.cpp
    ./libs/imgui/imgui_tables.cpp
    ./libs/imgui/imgui_widgets.cpp
    )
add_library(imgui ${IMGUI_SOURCES})
target_link_libraries(imgui PRIVATE glad)
target_include_directories(imgui PRIVATE 
    ${CMAKE_SOURCE_DIR}/libs/imgui 
    ${CMAKE_SOURCE_DIR}/libs/imgui/backends
    ${CMAKE_SOURCE_DIR}/libs/glfwpp/external/glfw/include
)

# Main game engine library
add_library(moonstone)
target_compile_features(moonstone PRIVATE cxx_std_23)
target_sources(moonstone PUBLIC FILE_SET CXX_MODULES FILES ${MOONSTONE_SOURCES})
target_link_libraries(moonstone PRIVATE GLFWPP doctest imgui glad angelscript ${glfw3_LIBRARIES})
target_compile_definitions(moonstone PRIVATE $<$<CONFIG:Debug>:_DEBUG> _ROOTDIR="${CMAKE_SOURCE_DIR}")
target_compile_options(moonstone PRIVATE $<$<CONFIG:Debug>:-fno-omit-frame-pointer -fsanitize=address>)
target_link_options(moonstone PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)

target_include_directories(moonstone SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/doctest/include
        ${CMAKE_SOURCE_DIR}/libs/imgui
        ${CMAKE_SOURCE_DIR}/libs/imgui/backends
        ${CMAKE_SOURCE_DIR}/libs/glad/include 
        ${CMAKE_SOURCE_DIR}/libs/glfwpp/include
        ${CMAKE_SOURCE_DIR}/libs/angelscript/sdk/angelscript/include
        ${CMAKE_SOURCE_DIR}/libs/glm
        ${CMAKE_SOURCE_DIR}/libs/stb
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include 
    )

# Main executable
add_executable(game)
target_sources(game PRIVATE ${SOURCES})
target_compile_features(game PRIVATE cxx_std_23)
target_link_libraries(game PRIVATE GLFWPP imgui moonstone)
target_compile_options(game PRIVATE $<$<CONFIG:Debug>:-fno-omit-frame-pointer -fsanitize=address>)
target_link_options(game PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
target_include_directories(game SYSTEM PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/imgui
        ${CMAKE_SOURCE_DIR}/libs/glfwpp/include
        ${CMAKE_SOURCE_DIR}/libs/glm
    )