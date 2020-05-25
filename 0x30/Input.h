#pragma once

#include "je/Context.h"

#include <SDL2/SDL_gamecontroller.h>

#include <array>


namespace input
{
    enum class ButtonId : uint32_t { debug, back, start, left, right, up, down, a, b, x, last = x };

    class ButtonStates
    {
    public:
        void Update();
        bool IsPressed(ButtonId id) const;
        bool IsReleased(ButtonId id) const;
        bool JustPressed(ButtonId id) const;
        bool JustReleased(ButtonId id) const;
        double LastPressed(ButtonId id) const;
        double LastReleased(ButtonId id) const;

        void DetectTransitions(double t);
        void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode);
        void OnGamepadButtonEvent(SDL_JoystickID joystickId, Uint8 button, Uint8 state);
        void OnGamepadAxisEvent(SDL_JoystickID joystickId, Uint8 axis, Sint16 value);

    private:
        void UpdateButton(bool isPressOrRepeat, uint32_t bit);

        using Buttons = uint32_t;
        static constexpr size_t numButtons = static_cast<size_t>(ButtonId::last) + 1;

        Buttons prevButtons_{ 0 };
        Buttons buttons_{ 0 };
        Buttons buttonDowns_{ 0 };
        Buttons buttonUps_{ 0 };
        std::array<double, numButtons> buttonDownTimes_;
        std::array<double, numButtons> buttonUpTimes_;
        bool wasLeftActivated_{};
        bool wasRightActivated_{};
        bool wasUpActivated_{};
        bool wasDownActivated_{};
    };

    class Input
    {
    public:
        static Input* Instance();

        void Init(je::Context& context);
        ~Input();

        void Update(double t);

        const ButtonStates& Buttons() const;
        bool HasGamepad() const;

    private:
        Input() = default;

        void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode);

        static void OnKeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

        SDL_GameController *controller_ { nullptr };
        bool hasGamepad_{ false };
        ButtonStates buttons_{};
    };

    GLFWkeyfun GetKeyboardHandler();
    void SetKeyboardHandler(GLFWkeyfun handler);
}
