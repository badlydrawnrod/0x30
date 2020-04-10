#include "Input.h"

#include "je/Logger.h"


namespace input
{
    // Input states.
    bool viewPressed = false;
    bool menuPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    bool upPressed = false;
    bool downPressed = false;
    bool swapPressed = false;
    bool fillHeld = false;

    bool keyboardFillHeld = false;

    bool wasViewPressed = false;
    bool wasMenuPressed = false;
    bool wasLeftPressed = false;
    bool wasRightPressed = false;
    bool wasUpPressed = false;
    bool wasDownPressed = false;
    bool wasSwapPressed = false;

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
            swapPressed = isPress;
            break;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            keyboardFillHeld = isPressOrRepeat;
            break;
        case GLFW_KEY_F12:
            viewPressed = isPress;
            break;
        case GLFW_KEY_ESCAPE:
            menuPressed = isPress;
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


    void UpdateInputState()
    {
        // Copy the current state.
        wasViewPressed = viewPressed;
        wasMenuPressed = menuPressed;
        wasLeftPressed = leftPressed;
        wasRightPressed = rightPressed;
        wasUpPressed = upPressed;
        wasDownPressed = downPressed;
        wasSwapPressed = swapPressed;
        oldJoystickX = joystickX;
        oldJoystickY = joystickY;
        wasLeftActivated = leftActivated;
        wasRightActivated = rightActivated;
        wasUpActivated = upActivated;
        wasDownActivated = downActivated;

        // Reset the state.
        viewPressed = false;
        menuPressed = false;
        leftPressed = false;
        rightPressed = false;
        upPressed = false;
        downPressed = false;
        swapPressed = false;

        // Check if any events have been activated (key pressed, mouse moved etc.) and invoke the relevant callbacks.
        glfwPollEvents();

        fillHeld = keyboardFillHeld;

        // Poll the first gamepad.
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            viewPressed = viewPressed || state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
            menuPressed = menuPressed || state.buttons[GLFW_GAMEPAD_BUTTON_START];
            leftPressed = leftPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
            rightPressed = rightPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
            upPressed = upPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
            downPressed = downPressed || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
            swapPressed = swapPressed || state.buttons[GLFW_GAMEPAD_BUTTON_A];
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
