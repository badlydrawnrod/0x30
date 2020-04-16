#include "Input.h"

#include "je/Logger.h"
#include "je/Time.h"


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

    bool ButtonStates::IsPressed(ButtonId id) const
    {
        const ButtonBit idBit = 1 << static_cast<uint32_t>(id);
        return (buttons_ & idBit) == idBit;
    }

    bool ButtonStates::IsReleased(ButtonId id) const
    {
        const ButtonBit idBit = 1 << static_cast<uint32_t>(id);
        return (buttons_ & idBit) == 0;
    }

    bool ButtonStates::JustPressed(ButtonId id) const
    {
        const ButtonBit idBit = 1 << static_cast<uint32_t>(id);
        return (buttonDowns_ & idBit) == idBit;
    }

    bool ButtonStates::JustReleased(ButtonId id) const
    {
        const ButtonBit idBit = 1 << static_cast<uint32_t>(id);
        return (buttonUps_ & idBit) == idBit;
    }

    double ButtonStates::LastPressed(ButtonId id)
    {
        size_t i = static_cast<size_t>(id);
        return buttonDownTimes_[i];
    }

    double ButtonStates::LastReleased(ButtonId id)
    {
        size_t i = static_cast<size_t>(id);
        return buttonUpTimes_[i];
    }

    void ButtonStates::Update()
    {
        prevButtons_ = buttons_;
    }

    void ButtonStates::UpdateButton(bool isPressOrRepeat, uint32_t bit)
    {
        if (isPressOrRepeat)
        {
            buttons_ |= bit;
        }
        else
        {
            buttons_ &= ~bit;
        }
    }

    void ButtonStates::PollGamepadButton(const GLFWgamepadstate& state, int button, uint32_t bit)
    {
        if (state.buttons[button] && !prevGamepadState_.buttons[button])
        {
            buttons_ |= bit;
        }
        else if (!state.buttons[button] && prevGamepadState_.buttons[button])
        {
            buttons_ &= ~bit;
        }
    }

    void ButtonStates::PollGamepad()
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_BACK, backBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_START, startBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_DPAD_LEFT, leftBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, rightBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_DPAD_UP, upBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, downBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_A, aBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_B, bBit);
            PollGamepadButton(state, GLFW_GAMEPAD_BUTTON_X, xBit);
            prevGamepadState_ = state;

            const float joystickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            const float joystickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

            const float threshold = 0.5f;
            const bool isLeftActivated = joystickX < -threshold;
            const bool isRightActivated = joystickX > threshold;
            const bool isUpActivated = joystickY < -threshold;
            const bool isDownActivated = joystickY > threshold;

            if (isLeftActivated != wasLeftActivated_)
            {
                UpdateButton(isLeftActivated, leftBit);
            }
            if (isRightActivated != wasRightActivated_)
            {
                UpdateButton(isRightActivated, rightBit);
            }
            if (isUpActivated != wasUpActivated_)
            {
                UpdateButton(isUpActivated, upBit);
            }
            if (isDownActivated != wasDownActivated_)
            {
                UpdateButton(isDownActivated, downBit);
            }

            wasLeftActivated_ = isLeftActivated;
            wasRightActivated_ = isRightActivated;
            wasUpActivated_ = isUpActivated;
            wasDownActivated_ = isDownActivated;
        }
    }

    void ButtonStates::DetectTransitions(double t)
    {
        Buttons changes = prevButtons_ ^ buttons_;

        buttonDowns_ = changes & buttons_;
        buttonUps_ = changes & (~buttons_);

        if (buttonDowns_)
        {
            uint32_t mask = 1;
            for (size_t id = 0; id < numButtons; id++)
            {
                if ((buttonDowns_ & mask) == mask)
                {
                    buttonDownTimes_[id] = t;
                }
                mask = mask << 1;
            }
        }
        if (buttonUps_)
        {
            uint32_t mask = 1;
            for (size_t id = 0; id < numButtons; id++)
            {
                if ((buttonUps_ & mask) == mask)
                {
                    buttonUpTimes_[id] = t;
                }
                mask = mask << 1;
            }
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
            UpdateButton(isPressOrRepeat, leftBit);
            break;
        case GLFW_KEY_RIGHT:
            // Right.
            UpdateButton(isPressOrRepeat, rightBit);
            break;
        case GLFW_KEY_UP:
            // Up.
            UpdateButton(isPressOrRepeat, upBit);
            break;
        case GLFW_KEY_DOWN:
            // Down.
            UpdateButton(isPressOrRepeat, downBit);
            break;
        case GLFW_KEY_SPACE:
        case GLFW_KEY_X:
            // Button [A].
            UpdateButton(isPressOrRepeat, aBit);
            break;
        case GLFW_KEY_ESCAPE:
            // Button [B].
            UpdateButton(isPressOrRepeat, bBit);
            break;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
        case GLFW_KEY_C:
            // Button [X].
            UpdateButton(isPressOrRepeat, xBit);
            break;
        case GLFW_KEY_P:
            // Button [Back].
            UpdateButton(isPressOrRepeat, backBit);
            break;
        case GLFW_KEY_F12:
            // Debug.
            UpdateButton(isPressOrRepeat, debugBit);
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
    void UpdateInputState(double t)
    {
        // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
        buttons.Update();
        glfwPollEvents();
        buttons.PollGamepad();
        buttons.DetectTransitions(t);
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
