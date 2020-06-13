#pragma once

#include "je/Sound.h"

#include <future>


struct Sounds
{
    void Load();
    bool IsLoaded();

    je::SoundBuffer blocksSwapping;
    je::SoundBuffer blocksLanding;
    je::SoundBuffer blocksPopping;
    je::SoundBuffer menuSelect;

    je::SoundBuffer musicMinuteWaltz;
    je::SoundBuffer musicLAdieu;
    je::SoundBuffer musicHallelujah;
    je::SoundBuffer musicGymnopedie;

private:
    void OnFileDownloaded(const char* filename);
    void Download(const char* url, const char* filename);

    static void OnSuccess(unsigned handle, void* userData, const char* filename);
    static void OnFailed(unsigned handle, void*, int status);
    static void OnProgress(unsigned handle, void*, int percent);

#if !defined(__EMSCRIPTEN__)
    void LoaderTask();
    std::future<void> loader_;
#else
    int downloaded_{ 0 };
    bool isLoaded_{ false };
#endif
};
