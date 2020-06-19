#include "VorbisLoader.h"

#include "Logger.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4138 4244 4245 4456 4457 4701)
#endif

#include "stb_vorbis.c"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <AL/al.h>
#include <string>

namespace je
{
    ALuint LoadVorbis(const std::string& filename)
    {
        ALuint buffer = 0;

        short* decoded;
        int channels;
        int sampleRate;
        int len = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &decoded);
        if (len <= 0)
        {
            LOG("vorbis decode failed on " << filename);
            return buffer;
        }

        LOG("vorbis decoded " << len << " bytes for " << filename);
        LOG("   channels: " << channels);
        LOG("sample rate: " << sampleRate);
        alGenBuffers(1, &buffer);
        ALenum format = channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        ALsizei numBytes = len;
        alBufferData(buffer, format, decoded, numBytes, sampleRate);
        if (auto error = alGetError(); error != AL_NO_ERROR)
        {
            LOG("alBufferData failed with OpenAL Error " << alGetString(error));
        }

        if (decoded)
        {
            free(decoded);
        }

        return buffer;
    }
} // namespace je
