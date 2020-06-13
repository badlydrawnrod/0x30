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

    static void WGet2Succeeded(unsigned handle, void* userData, const char* filename);
    static void WGet2Failed(unsigned handle, void*, int status);
    static void WGet2Progress(unsigned handle, void*, int percent);

#if !defined(__EMSCRIPTEN__)
    void LoaderTask();
    std::future<void> loader_;
#endif
};
