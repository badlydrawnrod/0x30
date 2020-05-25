cmake_minimum_required(VERSION 3.16)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


# Is this Emscripten?
if (CMAKE_CXX_COMPILER MATCHES "/emsdk.*em\\+\\+")
    message(STATUS "Emscripten detected")
    set(CMAKE_CXX_COMPILER_ID "Emscripten")
    set(TARGET_NAME index)
else ()
    set(TARGET_NAME game)
endif ()

message(STATUS "Build type: CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})
message(STATUS "Compiler: CMAKE_CXX_COMPILER=" ${CMAKE_CXX_COMPILER})
message(STATUS "Compiler id: CMAKE_CXX_COMPILER_ID=" ${CMAKE_CXX_COMPILER_ID})
message(STATUS "Compiler flags: CMAKE_CXX_COMPILE_FLAGS=" ${CMAKE_CXX_COMPILE_FLAGS})
message(STATUS "Compiler cxx debug flags: CMAKE_CXX_FLAGS_DEBUG=" ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "Compiler cxx release flags: CMAKE_CXX_FLAGS_RELEASE=" ${CMAKE_CXX_FLAGS_RELEASE})
message(STATUS "Compiler cxx min size flags: CMAKE_CXX_FLAGS_MINSIZEREL" ${CMAKE_CXX_FLAGS_MINSIZEREL})
message(STATUS "Compiler cxx flags: CMAKE_CXX_FLAGS" ${CMAKE_CXX_FLAGS})
message(STATUS "Library Type: LIB_TYPE" ${LIB_TYPE})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glad CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-image CONFIG REQUIRED)
    find_package(OpenAL CONFIG REQUIRED)
    find_package(LibSndFile CONFIG REQUIRED)
    find_package(jsoncpp CONFIG REQUIRED)
    set(LIBS glfw glad::glad SDL2::SDL2 SDL2::SDL2_image OpenAL::OpenAL sndfile-shared jsoncpp_lib)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_definitions(-D GLFW_INCLUDE_ES3)
endif ()


if (CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/index)
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
endif ()

include_directories(${CMAKE_SOURCE_DIR})

add_executable(${TARGET_NAME}
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
        je/Types.h je/Keyboard.cpp je/Keyboard.h)

target_link_libraries(${TARGET_NAME} PRIVATE ${LIBS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Emscripten")
    #    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "-Wall -pedantic -Wshadow -Wpointer-arith -Wcast-qual \
    #-Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wconversion -lopenal -O2 \
    #-s USE_FREETYPE=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"]")
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "-lopenal -O2 -s USE_FREETYPE=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"]")

    # The @ is used to remap ${CMAKE_SOURCE_DIR}/assets to /assets.
    # Here we're explicitly _not_ loading music as it causes everything to crash.
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS " -s ASSERTIONS=1 -O2 --shell-file ${CMAKE_SOURCE_DIR}/html/minimal.html -s USE_FREETYPE=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"] --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets/ --exclude-file ${CMAKE_SOURCE_DIR}/assets/music")
    #    set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-O2 --shell-file ${CMAKE_SOURCE_DIR}/html/minimal.html \
    #    -s USE_FREETYPE=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"] \
    #     --preload-file ${CMAKE_SOURCE_DIR}/assets/sprite_tiles.png@assets/sprite_tiles.png \
    #     --preload-file ${CMAKE_SOURCE_DIR}/assets/backdrops@assets/backdrops \
    #     --preload-file ${CMAKE_SOURCE_DIR}/assets/sounds@assets/sounds")
endif ()
