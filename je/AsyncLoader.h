#pragma once

#include "Logger.h"

#include <string>

#if !defined(__EMSCRIPTEN__)

#include <chrono>
#include <future>

namespace je
{
    template<typename T>
    class AsyncLoader
    {
    public:
        bool IsLoaded();
        void Load();
        void Download(const std::string& url, const std::string& filename);

    private:
        std::future<void> loader_;
    };

    template<typename T>
    bool AsyncLoader<T>::IsLoaded()
    {
        return loader_.wait_for(std::chrono::microseconds(1)) == std::future_status::ready;
    }

    template<typename T>
    void AsyncLoader<T>::Load()
    {
        static_cast<T*>(this)->OnLoaderStarted();
        loader_ = std::async(&T::OnLoaderFinished, static_cast<T*>(this));
    }

    template<typename T>
    void AsyncLoader<T>::Download(const std::string& /*url*/, const std::string& /*filename*/)
    {
    }
} // namespace je

#else

#include "emscripten.h"

namespace je
{
    template<typename T>
    class AsyncLoader
    {
    public:
        bool IsLoaded();
        void Load();
        void Download(const std::string& url, const std::string& filename);

    private:
        void OnFileDownloaded(const char* filename);

        static void OnSuccess(unsigned handle, void* userData, const char* filename);
        static void OnFailed(unsigned handle, void*, int status);
        static void OnProgress(unsigned handle, void*, int percent);

    private:
        int needed_{0};
        int downloads_{0};
    };

    template<typename T>
    bool AsyncLoader<T>::IsLoaded()
    {
        return downloads_ >= needed_;
    }

    template<typename T>
    void AsyncLoader<T>::Load()
    {
        static_cast<T*>(this)->OnLoaderStarted();
    }

    template<typename T>
    void AsyncLoader<T>::Download(const std::string& url, const std::string& filename)
    {
        ++needed_;
        emscripten_async_wget2(url.c_str(), filename.c_str(), "GET", nullptr, this, OnSuccess, OnFailed, OnProgress);
    }

    template<typename T>
    void AsyncLoader<T>::OnFileDownloaded(const char* /*filename*/)
    {
        ++downloads_;
        if (downloads_ == needed_)
        {
            static_cast<T*>(this)->OnLoaderFinished();
        }
    }

    template<typename T>
    void AsyncLoader<T>::OnSuccess(unsigned handle, void* userData, const char* filename)
    {
        LOG("Downloaded: " << filename << " with handle = " << handle);
        auto loader = reinterpret_cast<AsyncLoader<T>*>(userData);
        loader->OnFileDownloaded(filename);
    }

    template<typename T>
    void AsyncLoader<T>::OnFailed(unsigned handle, void*, int status)
    {
        LOG("Download failed for handle " << handle << " with status code " << status);
    }

    template<typename T>
    void AsyncLoader<T>::OnProgress(unsigned handle, void*, int percent)
    {
        LOG("Download progress for handle " << handle << " = " << percent);
    }
} // namespace je

#endif
