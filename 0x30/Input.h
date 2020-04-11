#pragma once

#include "je/Context.h"


namespace input
{
    // Input states.
    bool IsDebugPressed();
    bool WasDebugPressed();

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

    bool IsBPressed();
    bool WasBPressed();

    bool IsXPressed();
    bool WasXPressed();
    bool IsXHeld();

    void Initialise(je::Context& context);
    void UpdateInputState();
}
