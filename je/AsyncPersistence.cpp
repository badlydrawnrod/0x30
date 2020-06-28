#include "AsyncPersistence.h"

#if !defined(__EMSCRIPTEN__)

#include <fstream>
#include <future>

namespace je
{
    static void DoLoad(const std::string& filename, const AsyncPersistenceLoader::OnLoaded& onLoaded, const AsyncPersistenceLoader::OnError& onError)
    {
        std::ifstream infile(filename);
        if (infile)
        {
            infile.seekg(0, std::ifstream::end);
            auto length = infile.tellg();
            infile.seekg(0, std::ifstream::beg);
            auto buffer = std::make_unique<char[]>(length);
            infile.read(buffer.get(), length);
            infile.close();
            onLoaded(filename, buffer.get(), static_cast<int>(length));
        }
        else
        {
            onError(filename);
        }
    }

    void AsyncPersistenceLoader::Load(const std::string& filename, const OnLoaded& onLoaded, const OnError& onError)
    {
        std::thread(&DoLoad, filename, onLoaded, onError).detach();
    }

    static void DoSave(const std::string& filename, void* buffer, int length, const AsyncPersistenceSaver::OnSaved& onSaved, const AsyncPersistenceSaver::OnError& onError)
    {
        std::ofstream outfile(filename);
        if (outfile)
        {
            outfile.write(static_cast<char*>(buffer), length);
            outfile.close();
            onSaved(filename);
        }
        else
        {
            onError(filename);
        }
    }

    void AsyncPersistenceSaver::Save(const std::string& filename, void* buffer, int length, const OnSaved& onSaved, const OnError& onError)
    {
        std::thread(DoSave, filename, buffer, length, onSaved, onError).detach();
    }
} // namespace je

#else

#include "emscripten.h"

#include <memory>
#include <utility>

namespace je
{
    constexpr char dbName[] = "JePersistence"; // TODO: parameterise this?

    struct InflightLoad
    {
        InflightLoad(std::string filename, AsyncPersistenceLoader::OnLoaded onLoaded, AsyncPersistenceLoader::OnError onError)
            : filename(std::move(filename)), onLoaded(std::move(onLoaded)), onError(std::move(onError))
        {
        }

        std::string filename;
        AsyncPersistenceLoader::OnLoaded onLoaded;
        AsyncPersistenceLoader::OnError onError;
    };

    static void OnLoadedShim(void* arg, void* buffer, int length)
    {
        auto handler = std::unique_ptr<InflightLoad>(reinterpret_cast<InflightLoad*>(arg));
        if (handler->onLoaded)
        {
            handler->onLoaded(handler->filename, buffer, length);
        }
    }

    static void OnLoadErrorShim(void* arg)
    {
        auto handler = std::unique_ptr<InflightLoad>(reinterpret_cast<InflightLoad*>(arg));
        if (handler->onError)
        {
            handler->onError(handler->filename);
        }
    }

    void AsyncPersistenceLoader::Load(const std::string& filename, const OnLoaded& onLoaded, const OnError& onError)
    {
        // This will leak if neither of the callbacks are invoked.
        auto inflight = std::make_unique<InflightLoad>(filename, onLoaded, onError);
        emscripten_idb_async_load(dbName, filename.c_str(), inflight.release(), OnLoadedShim, OnLoadErrorShim);
    }

    struct InflightSave
    {
        InflightSave(std::string filename, AsyncPersistenceSaver::OnSaved onSaved, AsyncPersistenceSaver::OnError onError)
            : filename(std::move(filename)), onSaved(std::move(onSaved)), onError(std::move(onError))
        {
        }

        std::string filename;
        AsyncPersistenceSaver::OnSaved onSaved;
        AsyncPersistenceSaver::OnError onError;
    };

    static void OnSavedShim(void* arg)
    {
        auto handler = std::unique_ptr<InflightSave>(reinterpret_cast<InflightSave*>(arg));
        if (handler->onSaved)
        {
            handler->onSaved(handler->filename);
        }
    }

    static void OnSaveErrorShim(void* arg)
    {
        auto handler = std::unique_ptr<InflightSave>(reinterpret_cast<InflightSave*>(arg));
        if (handler->onError)
        {
            handler->onError(handler->filename);
        }
    }

    void AsyncPersistenceSaver::Save(const std::string& filename, void* buffer, int length, const OnSaved& onSaved, const OnError& onError)
    {
        // This will leak if neither of the callbacks are invoked.
        auto inflight = std::make_unique<InflightSave>(filename, onSaved, onError);
        emscripten_idb_async_store(dbName, filename.c_str(), buffer, length, inflight.release(), OnSavedShim, OnSaveErrorShim);
    }
} // namespace je

#endif
