#include "AsyncPersistence.h"

#if !defined(__EMSCRIPTEN__)

#else

#include "emscripten.h"

#include <memory>
#include <utility>

namespace je
{
    struct InflightLoad
    {
        InflightLoad(std::string filename, AsyncPersistenceLoader::OnLoaded onLoaded, AsyncPersistenceLoader::OnError onError) : filename(std::move(filename)), onLoaded(std::move(onLoaded)), onError(std::move(onError)) {}

        std::string filename;
        AsyncPersistenceLoader::OnLoaded onLoaded;
        AsyncPersistenceLoader::OnError onError;
    };

    constexpr char dbName[] = "JePersistence"; // TODO: parameterise this?

    void AsyncPersistenceLoader::Load(const std::string& filename, const OnLoaded& onLoaded, const OnError& onError)
    {
        // This will leak if neither of the callbacks are invoked.
        auto inflight = std::make_unique<InflightLoad>(filename, onLoaded, onError);
        emscripten_idb_async_load(dbName, filename.c_str(), inflight.release(), OnLoadedShim, OnErrorShim);
    }

    void AsyncPersistenceLoader::OnLoadedShim(void* arg, void* buffer, int length)
    {
        auto handler = std::unique_ptr<InflightLoad>(reinterpret_cast<InflightLoad*>(arg));
        if (handler->onLoaded)
        {
            handler->onLoaded(handler->filename, buffer, length);
        }
    }

    void AsyncPersistenceLoader::OnErrorShim(void* arg)
    {
        auto handler = std::unique_ptr<InflightLoad>(reinterpret_cast<InflightLoad*>(arg));
        if (handler->onError)
        {
            handler->onError(handler->filename);
        }
    }

    struct InflightSave
    {
        InflightSave(std::string filename, AsyncPersistenceSaver::OnSaved onSaved, AsyncPersistenceSaver::OnError onError) : filename(std::move(filename)), onSaved(std::move(onSaved)), onError(std::move(onError)) {}

        std::string filename;
        AsyncPersistenceSaver::OnSaved onSaved;
        AsyncPersistenceSaver::OnError onError;
    };

    void AsyncPersistenceSaver::Save(const std::string& filename, void* buffer, int length, const OnSaved& onSaved, const OnError& onError)
    {
        // This will leak if neither of the callbacks are invoked.
        auto inflight = std::make_unique<InflightSave>(filename, onSaved, onError);
        emscripten_idb_async_store(dbName, filename.c_str(), buffer, length, inflight.release(), OnSavedShim, OnErrorShim);
    }

    void AsyncPersistenceSaver::OnSavedShim(void* arg)
    {
        auto handler = std::unique_ptr<InflightSave>(reinterpret_cast<InflightSave*>(arg));
        if (handler->onSaved)
        {
            handler->onSaved(handler->filename);
        }
    }

    void AsyncPersistenceSaver::OnErrorShim(void* arg)
    {
        auto handler = std::unique_ptr<InflightSave>(reinterpret_cast<InflightSave*>(arg));
        if (handler->onError)
        {
            handler->onError(handler->filename);
        }
    }
} // namespace je

#endif
