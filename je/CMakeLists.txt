cmake_minimum_required(VERSION 3.16)

project(je VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    find_package(glfw3 CONFIG REQUIRED)
    find_package(glad CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-image CONFIG REQUIRED)
    find_package(OpenAL CONFIG REQUIRED)

    set(LIBS glfw glad::glad SDL2::SDL2 SDL2::SDL2_image OpenAL::OpenAL)
elseif (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_definitions(-D GLFW_INCLUDE_ES3)
endif ()

if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_FREETYPE=1 -s USE_GLFW=3 -s USE_WEBGL2=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[\"png\"]")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif ()

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE
        Batch.cpp
        Batch.h
        Context.cpp
        Context.h
        Human.h
        Human.cpp
        Keyboard.cpp
        Keyboard.h
        Logger.h
        QuadHelpers.h
        Shaders.cpp
        Shaders.h
        Sound.cpp
        Sound.h
        Textures.cpp
        Textures.h
        MyTime.h
        Transforms.h
        Types.h
        WavDecoder.h
        WavDecoder.cpp
        WavLoader.cpp
        WavLoader.h
        VorbisLoader.cpp
        VorbisLoader.h
        SoundLoader.cpp
        SoundLoader.h
        Shell.h
        Platform.h
        AsyncLoader.h
        AsyncPersistence.cpp
        AsyncPersistence.h
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBS})
