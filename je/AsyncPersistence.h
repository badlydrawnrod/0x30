#pragma once

#include <functional>
#include <string>

namespace je
{
    class AsyncPersistenceLoader
    {
    public:
        using OnLoaded = std::function<void(void*, int)>;
        using OnError = std::function<void()>;

        AsyncPersistenceLoader(OnLoaded onLoaded, OnError onError) : onLoaded_{onLoaded}, onError_{onError}
        {
        }

        void Load(const std::string& filename);

    private:
        static void OnLoadedShim(void* arg, void* buffer, int length);
        static void OnErrorShim(void* arg);

        OnLoaded onLoaded_;
        OnError onError_;
    };

    class AsyncPersistenceSaver
    {
    public:
        using OnSaved = std::function<void()>;
        using OnError = std::function<void()>;

        AsyncPersistenceSaver(OnSaved onSaved, OnError onError) : onSaved_{onSaved}, onError_{onError}
        {
        }

        void Save(const std::string& filename, void* buffer, int length);

    private:
        static void OnSavedShim(void* arg);
        static void OnErrorShim(void* arg);

        OnSaved onSaved_;
        OnError onError_;
    };
} // namespace je
