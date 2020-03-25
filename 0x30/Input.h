#pragma once

#include "je/Context.h"


namespace input
{
    // Input states.
    extern bool leftPressed;
    extern bool rightPressed;
    extern bool upPressed;
    extern bool downPressed;
    extern bool swapPressed;
    extern bool fillHeld;

    extern bool keyboardFillHeld;

    extern bool wasLeftPressed;
    extern bool wasRightPressed;
    extern bool wasUpPressed;
    extern bool wasDownPressed;
    extern bool wasSwapPressed;

    extern bool leftActivated;
    extern bool rightActivated;
    extern bool upActivated;
    extern bool downActivated;

    extern bool wasLeftActivated;
    extern bool wasRightActivated;
    extern bool wasUpActivated;
    extern bool wasDownActivated;

    extern float joystickX;
    extern float joystickY;
    extern float oldJoystickX;
    extern float oldJoystickY;

    void Initialise(je::Context& context);
    void UpdateInputState();
}
