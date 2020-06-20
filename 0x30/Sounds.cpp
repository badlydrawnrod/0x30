#include "Sounds.h"

#include "je/Logger.h"
#include "je/SoundLoader.h"

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
    bool isLoaded_;
};

template<typename T>
void CrtpLoader<T>::OnSuccess(unsigned handle, void* userData, const char* filename)
{
    LOG("WGet2 downloaded: " << filename << " with handle = " << handle);
    auto loader = reinterpret_cast<CrtpLoader<T>*>(userData);
    loader->OnFileDownloaded(filename);
}

template<typename T>
void CrtpLoader<T>::OnFailed(unsigned handle, void*, int status)
{
    LOG("WGet2 failed for handle " << handle << " with status code " << status);
}

template<typename T>
void CrtpLoader<T>::OnProgress(unsigned handle, void*, int percent)
{
    LOG("WGet2 progress for handle " << handle << " = " << percent);
}

template <typename T>
void CrtpLoader<T>::Download(const std::string& url, const std::string& filename)
{
    emscripten_async_wget2(url.c_str(), filename.c_str(), "GET", nullptr, this, OnSuccess, OnFailed, OnProgress);
}

#endif

class SoundsLoader : public CrtpLoader<SoundsLoader>
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

void SoundsLoader::BeginLoad()
{
    Download("sounds/swap.wav", "assets/sounds/swap.wav");
    Download("sounds/marble_click.wav", "assets/sounds/marble_click.wav");
    Download("sounds/pop.wav", "assets/sounds/pop.wav");
    Download("sounds/swap.wav", "assets/sounds/swap.wav");
    Download("music/minute.ogg", "assets/music/minute.ogg");
    Download("music/adieu.ogg", "assets/music/adieu.ogg");
    Download("music/hallelujah.ogg", "assets/music/hallelujah.ogg");
    Download("music/gymnopedie1.ogg", "assets/music/gymnopedie1.ogg");
}

void SoundsLoader::OnLoaded()
{
    blocksSwapping.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
    blocksLanding.TakeOwnership(je::LoadSound("assets/sounds/marble_click.wav"));
    blocksPopping.TakeOwnership(je::LoadSound("assets/sounds/pop.wav"));
    menuSelect.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
    musicMinuteWaltz.TakeOwnership(je::LoadSound("assets/music/minute.ogg"));
    musicLAdieu.TakeOwnership(je::LoadSound("assets/music/adieu.ogg"));
    musicHallelujah.TakeOwnership(je::LoadSound("assets/music/hallelujah.ogg"));
    musicGymnopedie.TakeOwnership(je::LoadSound("assets/music/gymnopedie1.ogg"));
}

#if !defined(__EMSCRIPTEN__)

void Sounds::Load()
{
    LOG("Loading files");
    SoundsLoader soundsLoader;
    soundsLoader.Load();
    while (!soundsLoader.IsLoaded())
    {
    }
    LOG("Files loaded");

    loader_ = std::async(&Sounds::LoaderTask, this);
}

bool Sounds::IsLoaded()
{
    return loader_.wait_for(std::chrono::microseconds(1)) == std::future_status::ready;
}

void Sounds::LoaderTask()
{
    blocksSwapping.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
    blocksLanding.TakeOwnership(je::LoadSound("assets/sounds/marble_click.wav"));
    blocksPopping.TakeOwnership(je::LoadSound("assets/sounds/pop.wav"));
    menuSelect.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
    musicMinuteWaltz.TakeOwnership(je::LoadSound("assets/music/minute.ogg"));
    musicLAdieu.TakeOwnership(je::LoadSound("assets/music/adieu.ogg"));
    musicHallelujah.TakeOwnership(je::LoadSound("assets/music/hallelujah.ogg"));
    musicGymnopedie.TakeOwnership(je::LoadSound("assets/music/gymnopedie1.ogg"));
}

#else

#include "emscripten.h"

void Sounds::OnFileDownloaded(const char* filename)
{
    LOG("OnFileDownloaded " << filename);
    ++downloaded_;
    if (downloaded_ == 8) // TODO: obviously do better than this!
    {
        LOG("All sounds downloaded");
        blocksSwapping.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
        blocksLanding.TakeOwnership(je::LoadSound("assets/sounds/marble_click.wav"));
        blocksPopping.TakeOwnership(je::LoadSound("assets/sounds/pop.wav"));
        menuSelect.TakeOwnership(je::LoadSound("assets/sounds/swap.wav"));
        musicMinuteWaltz.TakeOwnership(je::LoadSound("assets/music/minute.ogg"));
        musicLAdieu.TakeOwnership(je::LoadSound("assets/music/adieu.ogg"));
        musicHallelujah.TakeOwnership(je::LoadSound("assets/music/hallelujah.ogg"));
        musicGymnopedie.TakeOwnership(je::LoadSound("assets/music/gymnopedie1.ogg"));
        isLoaded_ = true;
    }
}

void Sounds::OnSuccess(unsigned handle, void* userData, const char* filename)
{
    LOG("WGet2 downloaded: " << filename << " with handle = " << handle);
    Sounds* sounds = reinterpret_cast<Sounds*>(userData);
    sounds->OnFileDownloaded(filename);
}

void Sounds::OnFailed(unsigned handle, void*, int status)
{
    LOG("WGet2 failed for handle " << handle << " with status code " << status);
}

void Sounds::OnProgress(unsigned handle, void*, int percent)
{
    LOG("WGet2 progress for handle " << handle << " = " << percent);
}

void Sounds::Download(const char* url, const char* filename)
{
    emscripten_async_wget2(url, filename, "GET", nullptr, this, OnSuccess, OnFailed, OnProgress);
}

void Sounds::Load()
{
    Download("sounds/swap.wav", "assets/sounds/swap.wav");
    Download("sounds/marble_click.wav", "assets/sounds/marble_click.wav");
    Download("sounds/pop.wav", "assets/sounds/pop.wav");
    Download("sounds/swap.wav", "assets/sounds/swap.wav");
    Download("music/minute.ogg", "assets/music/minute.ogg");
    Download("music/adieu.ogg", "assets/music/adieu.ogg");
    Download("music/hallelujah.ogg", "assets/music/hallelujah.ogg");
    Download("music/gymnopedie1.ogg", "assets/music/gymnopedie1.ogg");
}

bool Sounds::IsLoaded()
{
    return isLoaded_;
}

#endif
