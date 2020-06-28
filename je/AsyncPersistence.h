#pragma once

#include <functional>
#include <string>

namespace je
{
    class AsyncPersistenceLoader
    {
    public:
        using OnLoaded = std::function<void(const std::string&, void*, int)>;
        using OnError = std::function<void(const std::string&)>;

        static void Load(const std::string& filename, const OnLoaded& onLoaded, const OnError& onError);
    };

    class AsyncPersistenceSaver
    {
    public:
        using OnSaved = std::function<void(const std::string&)>;
        using OnError = std::function<void(const std::string&)>;

        static void Save(const std::string& filename, void* buffer, int length, const OnSaved& onSaved, const OnError& onError);
    };
} // namespace je
