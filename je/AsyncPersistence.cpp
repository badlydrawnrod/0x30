#include "AsyncPersistence.h"

#if !defined(__EMSCRIPTEN__)

#else

#include "emscripten.h"

namespace je
{
    constexpr char dbName[] = "JePersistence"; // TODO: parameterise this?

    void AsyncPersistenceLoader::Load(const std::string& filename)
    {
        emscripten_idb_async_load(dbName, filename.c_str(), this, OnLoadedShim, OnErrorShim);
    }

    void AsyncPersistenceLoader::OnLoadedShim(void* arg, void* buffer, int length)
    {
        auto handler = reinterpret_cast<AsyncPersistenceLoader*>(arg);
        if (handler->onLoaded_)
        {
            handler->onLoaded_(buffer, length);
        }
    }

    void AsyncPersistenceLoader::OnErrorShim(void* arg)
    {
        auto handler = reinterpret_cast<AsyncPersistenceLoader*>(arg);
        if (handler->onError_)
        {
            handler->onError_();
        }
    }

    void AsyncPersistenceSaver::Save(const std::string& filename, void* buffer, int length)
    {
        emscripten_idb_async_store(dbName, filename.c_str(), buffer, length, this, OnSavedShim, OnErrorShim);
    }

    void AsyncPersistenceSaver::OnSavedShim(void* arg)
    {
        auto handler = reinterpret_cast<AsyncPersistenceSaver*>(arg);
        if (handler->onSaved_)
        {
            handler->onSaved_();
        }
    }

    void AsyncPersistenceSaver::OnErrorShim(void* arg)
    {
        auto handler = reinterpret_cast<AsyncPersistenceSaver*>(arg);
        if (handler->onError_)
        {
            handler->onError_();
        }
    }
} // namespace je

#endif
