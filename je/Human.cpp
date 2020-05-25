#include "Human.h"

#include <SDL2/SDL.h>


namespace je
{
    Human* Human::Instance()
    {
        static std::unique_ptr <Human> input = nullptr;
        if (!input)
        {
            input.reset(new Human);
        }

        return input.get();
    }

    Human::Human()
    {
        // Use SDL for gamepads.
        SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    }

    Human::~Human()
    {
        if (controller_)
        {
            SDL_GameControllerClose(controller_);
            controller_ = nullptr;
        }
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    }

    void Human::OnKeyboardEvent(KeyboardEventFn keyboardEventFn)
    {
        keyboardEventFn_ = keyboardEventFn;
    }

    void Human::OnGamepadButtonEvent(GamepadButtonEventFn gamepadButtonEventFn)
    {
        gamepadButtonEventFn_ = gamepadButtonEventFn;
    }

    void Human::OnGamepadAxisEvent(GamepadAxisEventFn gamepadAxisEventFn)
    {
        gamepadAxisEventFn_ = gamepadAxisEventFn;
    }

    void Human::Update(double t)
    {
        // ASk SDL to poll for events. We specifically want to know about controllers, i.e., gamepads.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_CONTROLLERDEVICEADDED:
                // We only care about the first controller we see.
                if (!controller_)
                {
                    controller_ = SDL_GameControllerOpen(event.cdevice.which);
                    hasGamepad_ = true;
                }
                break;
            case SDL_CONTROLLERAXISMOTION:
                if (gamepadAxisEventFn_)
                {
                    gamepadAxisEventFn_(event.caxis.which, event.caxis.axis, event.caxis.value);
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                if (gamepadButtonEventFn_)
                {
                    gamepadButtonEventFn_(event.cbutton.which, event.cbutton.button, event.cbutton.state);
                }
                break;
            }
        }

        // Tell GLFW to poll so that we can get keyboard events.
        glfwPollEvents();
    }

    void Human::KeyboardEventHandler(GLFWwindow* window, int key, int scancode, int action, int mode)
    {
        if (keyboardEventFn_)
        {
            keyboardEventFn_(window, key, scancode, action, mode);
        }
    }
}
