#include "Input.h"

#include "je/Logger.h"


namespace
{
    using GameTime = double;

    enum class ButtonId : uint32_t { debug, back, start, left, right, up, down, a, b, x };

    using ButtonBit = uint32_t;
    using Buttons = uint32_t;


    constexpr ButtonBit debugBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit backBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit startBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit leftBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit rightBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit upBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit downBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit aBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit bBit = 1 << static_cast<uint32_t>(ButtonId::debug);
    constexpr ButtonBit xBit = 1 << static_cast<uint32_t>(ButtonId::debug);

    class ButtonStates
    {
    public:
        void Update();
        void DetectTransitions();

        bool JustPressed(ButtonId id);

        void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode);
        void OnJoystickEvent(int joystickId, int event);

    private:
        Buttons prevButtons_{ 0 };
        Buttons buttons_{ 0 };
        Buttons buttonDowns_{ 0 };
        Buttons buttonUps_{ 0 };
    };

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

    // Input states.
    bool isDebugPressed = false;
    bool isBackPressed = false;
    bool isStartPressed = false;
    bool isLeftPressed = false;
    bool isRightPressed = false;
    bool isUpPressed = false;
    bool isDownPressed = false;
    bool isAPressed = false;
    bool isBPressed = false;
    bool isXPressed = false;

    bool wasDebugPressed = false;
    bool wasBackPressed = false;
    bool wasStartPressed = false;
    bool wasLeftPressed = false;
    bool wasRightPressed = false;
    bool wasUpPressed = false;
    bool wasDownPressed = false;
    bool wasAPressed = false;
    bool wasBPressed = false;
    bool wasXPressed = false;

    bool isXHeld = false;
    bool isKeyboardXHeld = false;

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
        buttons.OnKeyEvent(window, key, scancode, action, mode);

        bool isPress = (action == GLFW_PRESS);
        bool isPressOrRepeat = (action == GLFW_PRESS || action == GLFW_REPEAT);
        switch (key)
        {
        case GLFW_KEY_LEFT:
            // Left.
            isLeftPressed = isPress;
            break;
        case GLFW_KEY_RIGHT:
            // Right.
            isRightPressed = isPress;
            break;
        case GLFW_KEY_UP:
            // Up.
            isUpPressed = isPress;
            break;
        case GLFW_KEY_DOWN:
            // Down.
            isDownPressed = isPress;
            break;
        case GLFW_KEY_SPACE:
        case GLFW_KEY_X:
            // Button [A].
            isAPressed = isPress;
            break;
        case GLFW_KEY_ESCAPE:
            // Button [B].
            isBPressed = isPress;
            break;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
        case GLFW_KEY_C:
            // Button [X].
            isXPressed = isPress;
            isKeyboardXHeld = isPressOrRepeat;
            break;
        case GLFW_KEY_P:
            // Button [Back].
            isBackPressed = isPress;
            break;
        case GLFW_KEY_F12:
            // Debug.
            isDebugPressed = isPress;
            break;
        default:
            break;
        }
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
    bool IsDebugPressed() { return isDebugPressed; }
    bool WasDebugPressed() { return wasDebugPressed; }

    bool IsBackPressed() { return isBackPressed; }
    bool WasBackPressed() { return wasBackPressed; }

    bool IsStartPressed() { return isStartPressed; }
    bool WasStartPressed() { return wasStartPressed; }

    bool IsLeftPressed() { return isLeftPressed; }
    bool WasLeftPressed() { return wasLeftPressed; }

    bool IsRightPressed() { return isRightPressed; }
    bool WasRightPressed() { return wasRightPressed; }

    bool IsUpPressed() { return isUpPressed; }
    bool WasUpPressed() { return wasUpPressed; }

    bool IsDownPressed() { return isDownPressed; }
    bool WasDownPressed() { return wasDownPressed; }

    bool IsAPressed() { return isAPressed; }
    bool WasAPressed() { return wasAPressed; }

    bool IsBPressed() { return isBPressed; }
    bool WasBPressed() { return wasBPressed; }

    bool IsXPressed() { return isXPressed; }
    bool WasXPressed() { return wasXPressed; }

    bool IsXHeld() { return isXHeld; }


    void UpdateInputState()
    {
        // Copy the current state.
        wasDebugPressed = isDebugPressed;
        wasBackPressed = isBackPressed;
        wasStartPressed = isStartPressed;
        wasLeftPressed = isLeftPressed;
        wasRightPressed = isRightPressed;
        wasUpPressed = isUpPressed;
        wasDownPressed = isDownPressed;
        wasAPressed = isAPressed;
        wasBPressed = isBPressed;
        wasXPressed = isXPressed;

        // Copy the current joystick state.
        oldJoystickX = joystickX;
        oldJoystickY = joystickY;
        wasLeftActivated = leftActivated;
        wasRightActivated = rightActivated;
        wasUpActivated = upActivated;
        wasDownActivated = downActivated;

        // Reset the state.
        isDebugPressed = false;
        isBackPressed = false;
        isStartPressed = false;
        isLeftPressed = false;
        isRightPressed = false;
        isUpPressed = false;
        isDownPressed = false;
        isAPressed = false;
        isBPressed = false;
        isXPressed = false;

        // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
        buttons.Update();
        glfwPollEvents();

        isXHeld = isKeyboardXHeld;

        // Poll the first gamepad.
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            isBackPressed = isBackPressed || state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
            isStartPressed = isStartPressed || state.buttons[GLFW_GAMEPAD_BUTTON_START];
            isLeftPressed = isLeftPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
            isRightPressed = isRightPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
            isUpPressed = isUpPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
            isDownPressed = isDownPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
            isAPressed = isAPressed || state.buttons[GLFW_GAMEPAD_BUTTON_A];
            isBPressed = isBPressed || state.buttons[GLFW_GAMEPAD_BUTTON_B];
            isXPressed = isXPressed || state.buttons[GLFW_GAMEPAD_BUTTON_X];
            isXHeld = isKeyboardXHeld || state.buttons[GLFW_GAMEPAD_BUTTON_X];

            joystickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            joystickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

            // Binary activations on the stick.
            const float threshold = 0.5f;
            leftActivated = joystickX < -threshold;
            rightActivated = joystickX > threshold;
            upActivated = joystickY < -threshold;
            downActivated = joystickY > threshold;

            isLeftPressed = isLeftPressed || (leftActivated && !wasLeftActivated);
            isRightPressed = isRightPressed || (rightActivated && !wasRightActivated);
            isUpPressed = isUpPressed || (upActivated && !wasUpActivated);
            isDownPressed = isDownPressed || (downActivated && !wasDownActivated);
        }

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
