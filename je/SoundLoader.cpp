#include "SoundLoader.h"

#include "Logger.h"
#include "VorbisLoader.h"
#include "WavLoader.h"

#include <AL/al.h>
#include <string>

namespace je
{
    ALuint LoadSound(const std::string& filename)
    {
        LOG("Loading " << filename);

        if (auto buffer = LoadWav(filename); buffer != 0)
        {
            return buffer;
        }

        if (auto buffer = LoadVorbis(filename); buffer != 0)
        {
            return buffer;
        }

        return 0;
    }
} // namespace je
