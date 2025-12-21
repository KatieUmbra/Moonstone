CMAKE_MINIMUM_REQUIRED(VERSION 4.0.0)
PROJECT(Moonstone LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(GLFWPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # disable building GLFWPP examples
SET(IMGUI_BUILD_GLFW_BINDING ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Angelscript CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(imgui CONFIG REQUIRED)

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
    ./src/renderer/AsyncBuffer.cpp
    ./src/renderer/AsyncBufferLock.cpp
    ./src/renderer/AsyncBufferConnection.cpp
    # Engine Module
    ./src/engine/quad.cpp
    # Scenes Module
    ./scenes/SceneTexture.cpp
    ./scenes/SceneClearColor.cpp
    )

# Main game engine library
add_library(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_sources(${PROJECT_NAME} PUBLIC FILE_SET CXX_MODULES FILES ${MOONSTONE_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Angelscript::angelscript
    doctest::doctest
    glad::glad
    glfw
    glm::glm
    imgui::imgui
)
target_include_directories(${PROJECT_NAME}
    SYSTEM PRIVATE
    Angelscript_INCLUDE_DIRS
    doctest_INCLUDE_DIRS
    glad_INCLUDE_DIRS
    glfw3_INCLUDE_DIRS
    glm_INCLUDE_DIRS
    imgui_INCLUDE_DIRS
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include 
    )

# Main executable
add_executable(game)
target_sources(game PRIVATE ${SOURCES})
target_compile_features(game PRIVATE cxx_std_23)
target_link_libraries(game PRIVATE
    glfw
    imgui::imgui
    ${PROJECT_NAME}
)
target_include_directories(game SYSTEM PRIVATE
    glfw3_INCLUDE_DIRS
    glm_INCLUDE_DIRS
    imgui_INCLUDE_DIRS
    )
