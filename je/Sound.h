#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>


namespace je
{
    void InitOpenAl();
    ALuint LoadSound(const std::string& filename);
    void TearDownOpenAl();
}
