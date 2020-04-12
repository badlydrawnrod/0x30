#pragma once

#include "je/Context.h"

#include <array>


namespace input
{
    enum class ButtonId : uint32_t { debug, back, start, left, right, up, down, a, b, x, last = x };

    class ButtonStates
    {
    public:
        void Update();
        bool IsPressed(ButtonId id);
        bool IsReleased(ButtonId id);
        bool JustPressed(ButtonId id);
        bool JustReleased(ButtonId id);
        double LastPressed(ButtonId id);
        double LastReleased(ButtonId id);

        void DetectTransitions(double t);
        void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode);
        void OnJoystickEvent(int joystickId, int event);

    private:
        using Buttons = uint32_t;
        static constexpr size_t numButtons = static_cast<size_t>(ButtonId::last) + 1;

        Buttons prevButtons_{ 0 };
        Buttons buttons_{ 0 };
        Buttons buttonDowns_{ 0 };
        Buttons buttonUps_{ 0 };
        std::array<double, numButtons> buttonDownTimes_;
        std::array<double, numButtons> buttonUpTimes_;
    };

    extern ButtonStates buttons;

    void Initialise(je::Context& context);
    void UpdateInputState(double t);
}
