cmake_minimum_required(VERSION 3.16)

project(0x30 VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    set(TARGET_NAME game)
elseif (EMSCRIPTEN)
    set(TARGET_NAME index)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_definitions(-D GLFW_INCLUDE_ES3)
endif ()

if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_FREETYPE=1 -s USE_GLFW=3 -s USE_WEBGL2=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"]")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_FUNCTIONS=\"['_main']\"")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_FREETYPE=1 -s USE_GLFW=3 -s USE_WEBGL2=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"]")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -l openal --no-heap-copy")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --shell-file ${CMAKE_SOURCE_DIR}/html/minimal.html")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets/")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --exclude-file ${CMAKE_SOURCE_DIR}/assets/music")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --exclude-file ${CMAKE_SOURCE_DIR}/assets/sounds")

#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/web-${CMAKE_BUILD_TYPE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/web-${CMAKE_BUILD_TYPE})
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/desktop-${CMAKE_BUILD_TYPE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/desktop-${CMAKE_BUILD_TYPE})
endif ()

add_executable(${TARGET_NAME})
target_sources(${TARGET_NAME} PRIVATE
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

target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} PUBLIC je)

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
