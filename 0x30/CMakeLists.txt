cmake_minimum_required(VERSION 3.16)

project(0x30 VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    set(TARGET_NAME game)

    find_package(glfw3 CONFIG REQUIRED)
    find_package(glad CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-image CONFIG REQUIRED)
    find_package(OpenAL CONFIG REQUIRED)

    set(LIBS glfw glad::glad SDL2::SDL2 SDL2::SDL2_image OpenAL::OpenAL)
elseif (EMSCRIPTEN)
    set(TARGET_NAME index)

    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_definitions(-D GLFW_INCLUDE_ES3)
endif ()

add_executable(${TARGET_NAME}
        Colours.cpp
        Colours.h
        Constants.h
        Dedication.cpp
        Dedication.h
        Flyup.cpp
        Flyup.h
        FlyupRenderer.cpp
        FlyupRenderer.h
        Buttons.cpp
        Buttons.h
        LevelRenderer.cpp
        LevelRenderer.h
        main.cpp
        Menu.cpp
        Menu.h
        Pit.cpp
        Pit.h
        PitRenderer.cpp
        PitRenderer.h
        Playing.cpp
        Playing.h
        Progress.cpp
        Progress.h
        ScoreRenderer.cpp
        ScoreRenderer.h
        Sounds.cpp
        Sounds.h
        TextRenderer.cpp
        TextRenderer.h
        Textures.cpp
        Textures.h
        TimeRenderer.cpp
        TimeRenderer.h
        Types.h
        )

target_link_libraries(${TARGET_NAME} PRIVATE je)

if (${TARGET_NAME} STREQUAL "index")
    # Currently I have a slightly different directory structure for the Emscripten build.
    add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD

            COMMAND ${CMAKE_COMMAND} -E echo Copying assets...

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/music
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/music

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets/sounds
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sounds
    )
else ()
    add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD

            COMMAND ${CMAKE_COMMAND} -E echo Copying assets...

            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    )
endif ()
