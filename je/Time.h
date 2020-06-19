#pragma once

#include "Platform.h"

namespace je
{
    inline double GetTime()
    {
        return glfwGetTime();
    }
} // namespace je
