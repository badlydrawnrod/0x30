#pragma once

#include "je/Logger.h"
#include "je/Sound.h"

#if !defined(__EMSCRIPTEN__)

#include <chrono>
#include <future>

template<typename T>
class CrtpLoader
{
public:
    void Download(const std::string& url, const std::string& filename);
    bool IsLoaded();
    void Load();

private:
    std::future<void> loader_;
};

template<typename T>
void CrtpLoader<T>::Download(const std::string& /*url*/, const std::string& /*filename*/)
{
}

template<typename T>
bool CrtpLoader<T>::IsLoaded()
{
    return loader_.wait_for(std::chrono::microseconds(1)) == std::future_status::ready;
}

template<typename T>
void CrtpLoader<T>::Load()
{
    static_cast<T*>(this)->BeginLoad();
    loader_ = std::async(&T::OnLoaded, static_cast<T*>(this));
}

#else

#include "emscripten.h"

template<typename T>
class CrtpLoader
{
public:
    void OnFileDownloaded(const char* filename);
    void Download(const std::string& url, const std::string& filename);
    bool IsLoaded();
    void Load();

    static void OnSuccess(unsigned handle, void* userData, const char* filename);
    static void OnFailed(unsigned handle, void*, int status);
    static void OnProgress(unsigned handle, void*, int percent);

private:
    int needed_{0};
    int downloads_{0};
};

template<typename T>
bool CrtpLoader<T>::IsLoaded()
{
    return downloads_ >= needed_;
}

template<typename T>
void CrtpLoader<T>::OnFileDownloaded(const char* /*filename*/)
{
    ++downloads_;
    if (downloads_ == needed_)
    {
        static_cast<T*>(this)->OnLoaded();
    }
}

template<typename T>
void CrtpLoader<T>::OnSuccess(unsigned handle, void* userData, const char* filename)
{
    LOG("Downloaded: " << filename << " with handle = " << handle);
    auto loader = reinterpret_cast<CrtpLoader<T>*>(userData);
    loader->OnFileDownloaded(filename);
}

template<typename T>
void CrtpLoader<T>::OnFailed(unsigned handle, void*, int status)
{
    LOG("Download failed for handle " << handle << " with status code " << status);
}

template<typename T>
void CrtpLoader<T>::OnProgress(unsigned handle, void*, int percent)
{
    LOG("Download progress for handle " << handle << " = " << percent);
}

template<typename T>
void CrtpLoader<T>::Download(const std::string& url, const std::string& filename)
{
    ++needed_;
    emscripten_async_wget2(url.c_str(), filename.c_str(), "GET", nullptr, this, OnSuccess, OnFailed, OnProgress);
}

template<typename T>
void CrtpLoader<T>::Load()
{
    static_cast<T*>(this)->BeginLoad();
}

#endif

class Sounds : public CrtpLoader<Sounds>
{
public:
    void BeginLoad();
    void OnLoaded();

    je::SoundBuffer blocksSwapping;
    je::SoundBuffer blocksLanding;
    je::SoundBuffer blocksPopping;
    je::SoundBuffer menuSelect;

    je::SoundBuffer musicMinuteWaltz;
    je::SoundBuffer musicLAdieu;
    je::SoundBuffer musicHallelujah;
    je::SoundBuffer musicGymnopedie;
};
