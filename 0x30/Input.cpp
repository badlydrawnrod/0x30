#include "Input.h"

#include "je/Logger.h"


namespace input
{
    using ButtonBit = uint32_t;

    constexpr ButtonBit debugBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit backBit = 1 << static_cast<uint32_t>(ButtonId::back);
    constexpr ButtonBit startBit = 1 << static_cast<uint32_t>(ButtonId::start);
    constexpr ButtonBit leftBit = 1 << static_cast<uint32_t>(ButtonId::left);
    constexpr ButtonBit rightBit = 1 << static_cast<uint32_t>(ButtonId::right);
    constexpr ButtonBit upBit = 1 << static_cast<uint32_t>(ButtonId::up);
    constexpr ButtonBit downBit = 1 << static_cast<uint32_t>(ButtonId::down);
    constexpr ButtonBit aBit = 1 << static_cast<uint32_t>(ButtonId::a);
    constexpr ButtonBit bBit = 1 << static_cast<uint32_t>(ButtonId::b);
    constexpr ButtonBit xBit = 1 << static_cast<uint32_t>(ButtonId::x);

    void ButtonStates::Reset()
    {
        buttons_ = 0;
        prevButtons_ = 0;
        buttonUps_ = 0;
        buttonDowns_ = 0;
    }

    bool ButtonStates::IsPressed(ButtonId id)
    {
        const ButtonBit idBit = 1 << static_cast<uint32_t>(id);
        return (buttons_ & idBit) == idBit;
    }

    bool ButtonStates::JustPressed(ButtonId id)
    {
        const ButtonBit idBit = 1 << static_cast<uint32_t>(id);
        return (buttonDowns_ & idBit) == idBit;
    }

    void ButtonStates::Update()
    {
        prevButtons_ = buttons_;
    }

    void ButtonStates::DetectTransitions()
    {
        Buttons changes = prevButtons_ ^ buttons_;

        buttonDowns_ = changes & buttons_;
        buttonUps_ = changes & (~buttons_);

        if (buttonDowns_)
        {
            LOG("Button downs: 0x" << std::hex << buttonDowns_ << std::dec);
        }
        if (buttonUps_)
        {
            LOG("Button ups: 0x" << std::hex << buttonUps_ << std::dec);
        }
    }

    void ButtonStates::OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        bool isPress = (action == GLFW_PRESS);
        bool isPressOrRepeat = (action == GLFW_PRESS || action == GLFW_REPEAT);
        switch (key)
        {
        case GLFW_KEY_LEFT:
            // Left.
            if (isPressOrRepeat)
            {
                buttons_ |= leftBit;
            }
            else
            {
                buttons_ &= ~leftBit;
            }
            break;
        case GLFW_KEY_RIGHT:
            // Right.
            if (isPressOrRepeat)
            {
                buttons_ |= rightBit;
            }
            else
            {
                buttons_ &= ~rightBit;
            }
            break;
        case GLFW_KEY_UP:
            // Up.
            if (isPressOrRepeat)
            {
                buttons_ |= upBit;
            }
            else
            {
                buttons_ &= ~upBit;
            }
            break;
        case GLFW_KEY_DOWN:
            // Down.
            if (isPressOrRepeat)
            {
                buttons_ |= downBit;
            }
            else
            {
                buttons_ &= ~downBit;
            }
            break;
        case GLFW_KEY_SPACE:
        case GLFW_KEY_X:
            // Button [A].
            if (isPressOrRepeat)
            {
                buttons_ |= aBit;
            }
            else
            {
                buttons_ &= ~aBit;
            }
            break;
        case GLFW_KEY_ESCAPE:
            // Button [B].
            if (isPressOrRepeat)
            {
                buttons_ |= bBit;
            }
            else
            {
                buttons_ &= ~bBit;
            }
            break;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
        case GLFW_KEY_C:
            // Button [X].
            if (isPressOrRepeat)
            {
                buttons_ |= xBit;
            }
            else
            {
                buttons_ &= ~xBit;
            }
            break;
        case GLFW_KEY_P:
            // Button [Back].
            if (isPressOrRepeat)
            {
                buttons_ |= backBit;
            }
            else
            {
                buttons_ &= ~backBit;
            }
            break;
        case GLFW_KEY_F12:
            // Debug.
            if (isPressOrRepeat)
            {
                buttons_ |= debugBit;
            }
            else
            {
                buttons_ &= ~debugBit;
            }
            break;
        default:
            break;
        }
    }

    void ButtonStates::OnJoystickEvent(int joystickId, int event)
    {
    }

    ButtonStates buttons;
}


namespace
{
    // Left stick binary states.
    bool leftActivated = false;
    bool rightActivated = false;
    bool upActivated = false;
    bool downActivated = false;
    bool wasLeftActivated = false;
    bool wasRightActivated = false;
    bool wasUpActivated = false;
    bool wasDownActivated = false;

    // Left stick positions.
    float joystickX = 0.0f;
    float joystickY = 0.0f;
    float oldJoystickX = 0.0f;
    float oldJoystickY = 0.0f;

    // Called by GLFW whenever a key is pressed or released.
    void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        input::buttons.OnKeyEvent(window, key, scancode, action, mode);
    }

    // Called by GLFW whenever a joystick / gamepad is connected or disconnected.
    void OnJoystickEvent(int joystickId, int event)
    {
        // We only care if the joystick in question is a gamepad.
        if (glfwJoystickIsGamepad(joystickId))
        {
            if (event == GLFW_CONNECTED)
            {
                LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - connected");
            }
            else if (event == GLFW_DISCONNECTED)
            {
                LOG("Gamepad " << joystickId << " - " << glfwGetGamepadName(joystickId) << " - disconnected");
            }
        }
    }
}

namespace input
{
    void UpdateInputState()
    {
        // Copy the current joystick state.
        oldJoystickX = joystickX;
        oldJoystickY = joystickY;
        wasLeftActivated = leftActivated;
        wasRightActivated = rightActivated;
        wasUpActivated = upActivated;
        wasDownActivated = downActivated;

        // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
        buttons.Update();
        glfwPollEvents();

        // TODO: I've forgotten to pack the blueberry pie, er, raise a ticket.
        //
        // Poll the first gamepad.
        //GLFWgamepadstate state;
        //if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        //{
        //    isBackPressed = isBackPressed || state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
        //    isStartPressed = isStartPressed || state.buttons[GLFW_GAMEPAD_BUTTON_START];
        //    isLeftPressed = isLeftPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
        //    isRightPressed = isRightPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
        //    isUpPressed = isUpPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
        //    isDownPressed = isDownPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
        //    isAPressed = isAPressed || state.buttons[GLFW_GAMEPAD_BUTTON_A];
        //    isBPressed = isBPressed || state.buttons[GLFW_GAMEPAD_BUTTON_B];
        //    isXPressed = isXPressed || state.buttons[GLFW_GAMEPAD_BUTTON_X];
        //    isXHeld = isKeyboardXHeld || state.buttons[GLFW_GAMEPAD_BUTTON_X];

        //    joystickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        //    joystickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

        //    // Binary activations on the stick.
        //    const float threshold = 0.5f;
        //    leftActivated = joystickX < -threshold;
        //    rightActivated = joystickX > threshold;
        //    upActivated = joystickY < -threshold;
        //    downActivated = joystickY > threshold;

        //    isLeftPressed = isLeftPressed || (leftActivated && !wasLeftActivated);
        //    isRightPressed = isRightPressed || (rightActivated && !wasRightActivated);
        //    isUpPressed = isUpPressed || (upActivated && !wasUpActivated);
        //    isDownPressed = isDownPressed || (downActivated && !wasDownActivated);
        //}

        buttons.DetectTransitions();
    }

    void Initialise(je::Context& context)
    {
        // Wire up some GLFW callbacks.
        glfwSetKeyCallback(context.Window(), OnKeyEvent);
        glfwSetJoystickCallback(OnJoystickEvent);

        // Enumerate the gamepads.
        for (int joystickId = GLFW_JOYSTICK_1; joystickId <= GLFW_JOYSTICK_LAST; joystickId++)
        {
            if (glfwJoystickIsGamepad(joystickId))
            {
                LOG("Gamepad " << joystickId << " [" << glfwGetGamepadName(joystickId) << "] found");
            }
        }
    }
}
