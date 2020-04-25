#pragma once

#include "je/Types.h"


struct Colours
{
    static constexpr je::Rgba4b title{ 0xff, 0x00, 0x7f, 0xff };
    static constexpr je::Rgba4b titleShadow{ 0x7f, 0x00, 0x3f, 0xff };

    static constexpr je::Rgba4b levelText{ 0x1f, 0xff, 0xff, 0xff };
    static constexpr je::Rgba4b levelNumber{ 0xff, 0x1f, 0x1f, 0xff };

    static constexpr je::Rgba4b scoreText{ 0x1f, 0xff, 0xff, 0xff };
    static constexpr je::Rgba4b scoreNumber{ 0xff, 0x1f, 0x1f, 0xff };

    static constexpr je::Rgba4b timeText{ 0x1f, 0xff, 0xff, 0xff };
    static constexpr je::Rgba4b timeNumber{ 0xff, 0x1f, 0x1f, 0xff };

    static constexpr je::Rgba4b black{ 0x00, 0x00, 0x00, 0xff };
    static constexpr je::Rgba4b white{ 0xff, 0xff, 0xff, 0xff };

    static constexpr je::Rgba4b mode{ 0xff, 0x00, 0x00, 0xff };

    static constexpr je::Rgba4b cursorBackground{ 0x00, 0x7f, 0x7f, 0xff };

    static constexpr je::Rgba4b selectableLevel{ 0xff, 0xff, 0xff, 0xff };
    static constexpr je::Rgba4b unselectableLevel{ 0x7f, 0x7f, 0x7f, 0xff };

};