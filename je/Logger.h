#pragma once

#include <iostream>

#define LOG(...)                                                                  \
    {                                                                             \
        std::clog << __FILE__ << "(" << __LINE__ << "): " << __VA_ARGS__ << '\n'; \
    }
