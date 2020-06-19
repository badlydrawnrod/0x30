#pragma once
#include "Platform.h"

#include <SDL2/SDL_gamecontroller.h>
#include <functional>

namespace je
{
    class Human
    {
    public:
        using KeyboardEventFn = std::function<void(GLFWwindow* window, int key, int scancode, int action, int mode)>;
        using GamepadButtonEventFn = std::function<void(SDL_JoystickID joystickId, Uint8 button, Uint8 state)>;
        using GamepadAxisEventFn = std::function<void(SDL_JoystickID joystickId, Uint8 axis, Sint16 value)>;

        static Human* Instance();
        ~Human();

        bool HasGamepad() const
        {
            return hasGamepad_;
        }

        void OnKeyboardEvent(KeyboardEventFn keyboardEventFn);
        void OnGamepadButtonEvent(GamepadButtonEventFn gamepadButtonEventFn);
        void OnGamepadAxisEvent(GamepadAxisEventFn gamepadAxisEventFn);

        void Update(double t);

        // This callback is for the underlying keyboard code.
        void KeyboardEventHandler(GLFWwindow* window, int key, int scancode, int action, int mode);

    private:
        Human();

        KeyboardEventFn keyboardEventFn_;
        GamepadButtonEventFn gamepadButtonEventFn_;
        GamepadAxisEventFn gamepadAxisEventFn_;

        SDL_GameController* controller_{nullptr};
        bool hasGamepad_{false};
    };
} // namespace je
