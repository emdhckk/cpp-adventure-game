cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

# nlohmann/json
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(json)

add_executable(main
    src/main.cpp
    src/Chunk.cpp
    src/Tile.cpp
    src/TileRegistry.cpp
    src/NoiseGen.cpp
    src/World.cpp
    src/Entity.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Neutral.cpp
    src/QuestManager.cpp
    src/Boat.cpp
    src/RelicRender.cpp
    src/TileFinder.cpp
    src/Compass.cpp
    src/ScoreSave.cpp
    src/GameOver.cpp
    src/TitleScreen.cpp
)

target_compile_features(main PRIVATE cxx_std_17)

target_compile_definitions(main PRIVATE 
    ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/"
)

target_link_libraries(main PRIVATE
    SFML::Graphics
    SFML::Audio
    nlohmann_json::nlohmann_json
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)

target_include_directories(main PRIVATE external/FastNoiseLite)

# Copie les assets à côté de l'exécutable
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:main>/assets
)