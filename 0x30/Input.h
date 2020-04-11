#pragma once

#include "je/Context.h"


namespace input
{
    // Input states. Names refer to XBox controller names, even if keys are mapped to them.
    bool IsBackPressed();
    bool WasBackPressed();

    bool IsStartPressed();
    bool WasStartPressed();

    bool IsLeftPressed();
    bool WasLeftPressed();

    bool IsRightPressed();
    bool WasRightPressed();

    bool IsUpPressed();
    bool WasUpPressed();

    bool IsDownPressed();
    bool WasDownPressed();

    bool IsAPressed();
    bool WasAPressed();

    bool IsXHeld();

    void Initialise(je::Context& context);
    void UpdateInputState();
}
