cmake_minimum_required(VERSION 3.16)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(LibSndFile CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(game
        0x30/Colours.cpp
        0x30/Colours.h
        0x30/Constants.h
        0x30/Dedication.cpp
        0x30/Dedication.h
        0x30/Flyup.cpp
        0x30/Flyup.h
        0x30/FlyupRenderer.cpp
        0x30/FlyupRenderer.h
        0x30/Input.cpp
        0x30/Input.h
        0x30/LevelRenderer.cpp
        0x30/LevelRenderer.h
        0x30/main.cpp
        0x30/Menu.cpp
        0x30/Menu.h
        0x30/Pit.cpp
        0x30/Pit.h
        0x30/PitRenderer.cpp
        0x30/PitRenderer.h
        0x30/Playing.cpp
        0x30/Playing.h
        0x30/Progress.cpp
        0x30/Progress.h
        0x30/ScoreRenderer.cpp
        0x30/ScoreRenderer.h
        0x30/Sounds.cpp
        0x30/Sounds.h
        0x30/TextRenderer.cpp
        0x30/TextRenderer.h
        0x30/Textures.cpp
        0x30/Textures.h
        0x30/TimeRenderer.cpp
        0x30/TimeRenderer.h
        0x30/Types.h
        je/Batch.cpp
        je/Batch.h
        je/Context.cpp
        je/Context.h
        je/Logger.h
        je/QuadHelpers.h
        je/Shaders.cpp
        je/Shaders.h
        je/Sound.cpp
        je/Sound.h
        je/Textures.cpp
        je/Textures.h
        je/Time.h
        je/Transforms.h
        je/Types.h)

target_link_libraries(game PRIVATE glfw glad::glad SDL2::SDL2 SDL2::SDL2_image OpenAL::OpenAL sndfile-shared jsoncpp_lib)
