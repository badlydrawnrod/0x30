#include "Input.h"

#include "je/Human.h"
#include "je/Logger.h"
#include "je/Time.h"

#include <SDL2/SDL.h>

#include <functional>
#include <map>
#include <memory>


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

    double ButtonStates::LastPressed(ButtonId id) const
    {
        size_t i = static_cast<size_t>(id);
        return buttonDownTimes_[i];
    }

    double ButtonStates::LastReleased(ButtonId id) const
    {
        size_t i = static_cast<size_t>(id);
        return buttonUpTimes_[i];
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

    void ButtonStates::Update(double t)
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

        prevButtons_ = buttons_;
    }

    void ButtonStates::OnKeyEvent(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mode*/)
    {
        const bool isPressOrRepeat = (action == GLFW_PRESS || action == GLFW_REPEAT);
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

    void ButtonStates::OnGamepadButtonEvent(SDL_JoystickID joystickId, Uint8 button, Uint8 state)
    {
        const bool isPressOrRepeat = (state == SDL_PRESSED);
        switch (button)
        {
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            // Left.
            UpdateButton(isPressOrRepeat, leftBit);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            // Right.
            UpdateButton(isPressOrRepeat, rightBit);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            // Up.
            UpdateButton(isPressOrRepeat, upBit);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            // Down.
            UpdateButton(isPressOrRepeat, downBit);
            break;
        case SDL_CONTROLLER_BUTTON_A:
            // Button [A].
            UpdateButton(isPressOrRepeat, aBit);
            break;
        case SDL_CONTROLLER_BUTTON_B:
            // Button [B].
            UpdateButton(isPressOrRepeat, bBit);
            break;
        case SDL_CONTROLLER_BUTTON_X:
            // Button [X].
            UpdateButton(isPressOrRepeat, xBit);
            break;
        case SDL_CONTROLLER_BUTTON_BACK:
            // Button [Back].
            UpdateButton(isPressOrRepeat, backBit);
            break;
        default:
            break;
        }
    }

    void ButtonStates::OnGamepadAxisEvent(SDL_JoystickID joystickId, Uint8 axis, Sint16 value)
    {
        const Sint16 threshold = SDL_MAX_SINT16 / 2;
        if (axis == SDL_CONTROLLER_AXIS_LEFTX)
        {
            const Sint16 joystickX = value;
            const bool isLeftActivated = joystickX < -threshold;
            const bool isRightActivated = joystickX > threshold;
            if (isLeftActivated != wasLeftActivated_)
            {
                UpdateButton(isLeftActivated, leftBit);
            }
            if (isRightActivated != wasRightActivated_)
            {
                UpdateButton(isRightActivated, rightBit);
            }
            wasLeftActivated_ = isLeftActivated;
            wasRightActivated_ = isRightActivated;
        }
        else if (axis == SDL_CONTROLLER_AXIS_LEFTY)
        {
            const Sint16 joystickY = value;
            const bool isUpActivated = joystickY < -threshold;
            const bool isDownActivated = joystickY > threshold;

            if (isUpActivated != wasUpActivated_)
            {
                UpdateButton(isUpActivated, upBit);
            }
            if (isDownActivated != wasDownActivated_)
            {
                UpdateButton(isDownActivated, downBit);
            }

            wasUpActivated_ = isUpActivated;
            wasDownActivated_ = isDownActivated;
        }
    }
}

namespace input
{
    Input* Input::Instance()
    {
        static std::unique_ptr<Input> input = nullptr;
        if (!input)
        {
            input.reset(new Input);
        }
        return input.get();
    }

    void Input::Init(je::Context &context)
    {
        // Tell the input that we want to know about keyboard events.
        je::Human::Instance()->OnKeyboardEvent([this](GLFWwindow* window, int key, int scancode, int action, int mode) {
            buttons_.OnKeyEvent(window, key, scancode, action, mode);
        });

        // Tell the input that we want to know about gamepad button events.
        je::Human::Instance()->OnGamepadButtonEvent([this](SDL_JoystickID joystickId, Uint8 button, Uint8 state) {
            buttons_.OnGamepadButtonEvent(joystickId, button, state);
        });

        // Tell the input that we want to know about gamepad axis events.
        je::Human::Instance()->OnGamepadAxisEvent([this](SDL_JoystickID joystickId, Uint8 axis, Sint16 value) {
            buttons_.OnGamepadAxisEvent(joystickId, axis, value);
        });
    }

    void Input::Update(double t)
    {
        je::Human::Instance()->Update(t);
        buttons_.Update(t);
    }

    const ButtonStates& Input::Buttons() const
    {
        return buttons_;
    }
}
