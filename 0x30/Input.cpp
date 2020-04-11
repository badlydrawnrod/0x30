#include "Input.h"

#include "je/Logger.h"


namespace
{
    // Input states.
    bool backPressed = false;
    bool startPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    bool upPressed = false;
    bool downPressed = false;
    bool aPressed = false;
    bool fillHeld = false;

    bool keyboardFillHeld = false;

    bool wasBackPressed = false;
    bool wasStartPressed = false;
    bool wasLeftPressed = false;
    bool wasRightPressed = false;
    bool wasUpPressed = false;
    bool wasDownPressed = false;
    bool wasAPressed = false;

    bool leftActivated = false;
    bool rightActivated = false;
    bool upActivated = false;
    bool downActivated = false;

    bool wasLeftActivated = false;
    bool wasRightActivated = false;
    bool wasUpActivated = false;
    bool wasDownActivated = false;

    float joystickX = 0.0f;
    float joystickY = 0.0f;
    float oldJoystickX = 0.0f;
    float oldJoystickY = 0.0f;


    // Called by GLFW whenever a key is pressed or released.
    void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        bool isPress = (action == GLFW_PRESS);
        bool isPressOrRepeat = (action == GLFW_PRESS || action == GLFW_REPEAT);
        switch (key)
        {
        case GLFW_KEY_LEFT:
        case GLFW_KEY_A:
            leftPressed = isPress;
            break;
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_D:
            rightPressed = isPress;
            break;
        case GLFW_KEY_UP:
        case GLFW_KEY_W:
            upPressed = isPress;
            break;
        case GLFW_KEY_DOWN:
        case GLFW_KEY_S:
            downPressed = isPress;
            break;
        case GLFW_KEY_SPACE:
        case GLFW_KEY_ENTER:
        case GLFW_KEY_Z:
        case GLFW_KEY_X:
        case GLFW_KEY_C:
            aPressed = isPress;
            break;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            keyboardFillHeld = isPressOrRepeat;
            break;
        case GLFW_KEY_F12:
            backPressed = isPress;
            break;
        case GLFW_KEY_ESCAPE:
            startPressed = isPress;
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

    bool IsBackPressed() { return backPressed; }
    bool WasBackPressed() { return wasBackPressed; }

    bool IsStartPressed() { return startPressed; }
    bool WasStartPressed() { return wasStartPressed; }

    bool IsLeftPressed() { return leftPressed; }
    bool WasLeftPressed() { return wasLeftPressed; }

    bool IsRightPressed() { return rightPressed; }
    bool WasRightPressed() { return wasRightPressed; }

    bool IsUpPressed() { return upPressed; }
    bool WasUpPressed() { return wasUpPressed; }

    bool IsDownPressed() { return downPressed; }
    bool WasDownPressed() { return wasDownPressed; }

    bool IsAPressed() { return aPressed; }
    bool WasAPressed() { return wasAPressed; }

    bool IsXHeld() { return fillHeld; }


    void UpdateInputState()
    {
        // Copy the current state.
        wasBackPressed = backPressed;
        wasStartPressed = startPressed;
        wasLeftPressed = leftPressed;
        wasRightPressed = rightPressed;
        wasUpPressed = upPressed;
        wasDownPressed = downPressed;
        wasAPressed = aPressed;
        oldJoystickX = joystickX;
        oldJoystickY = joystickY;
        wasLeftActivated = leftActivated;
        wasRightActivated = rightActivated;
        wasUpActivated = upActivated;
        wasDownActivated = downActivated;

        // Reset the state.
        backPressed = false;
        startPressed = false;
        leftPressed = false;
        rightPressed = false;
        upPressed = false;
        downPressed = false;
        aPressed = false;

        // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
        glfwPollEvents();

        fillHeld = keyboardFillHeld;

        // Poll the first gamepad.
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            backPressed = backPressed || state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
            startPressed = startPressed || state.buttons[GLFW_GAMEPAD_BUTTON_START];
            leftPressed = leftPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
            rightPressed = rightPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
            upPressed = upPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
            downPressed = downPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
            aPressed = aPressed || state.buttons[GLFW_GAMEPAD_BUTTON_A];
            fillHeld = keyboardFillHeld || state.buttons[GLFW_GAMEPAD_BUTTON_X];

            joystickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            joystickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

            const float threshold = 0.5f;
            leftActivated = joystickX < -threshold;
            rightActivated = joystickX > threshold;
            upActivated = joystickY < -threshold;
            downActivated = joystickY > threshold;

            leftPressed = leftPressed || (leftActivated && !wasLeftActivated);
            rightPressed = rightPressed || (rightActivated && !wasRightActivated);
            upPressed = upPressed || (upActivated && !wasUpActivated);
            downPressed = downPressed || (downActivated && !wasDownActivated);
        }
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
