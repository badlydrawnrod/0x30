#pragma once

#include "je/Context.h"


namespace input
{
    enum class ButtonId : uint32_t { debug, back, start, left, right, up, down, a, b, x };

    class ButtonStates
    {
    public:
        void Reset();
        void Update();
        bool IsPressed(ButtonId id);
        bool JustPressed(ButtonId id);

        void DetectTransitions();
        void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode);
        void OnJoystickEvent(int joystickId, int event);

    private:
        using Buttons = uint32_t;

        Buttons prevButtons_{ 0 };
        Buttons buttons_{ 0 };
        Buttons buttonDowns_{ 0 };
        Buttons buttonUps_{ 0 };
    };

    extern ButtonStates buttons;

    void Initialise(je::Context& context);
    void UpdateInputState();
}
