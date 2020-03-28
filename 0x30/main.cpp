#include "Assets.h"
#include "Constants.h"
#include "Dedication.h"
#include "Input.h"
#include "Menu.h"
#include "Pit.h"
#include "PitRenderer.h"
#include "Playing.h"
#include "TextRenderer.h"

#include "je/Batch.h"
#include "je/Context.h"
#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Shaders.h"
#include "je/Textures.h"
#include "je/Time.h"
#include "je/Types.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <random>
#include <sstream>


// Show / hide the Windows console.
void ToggleConsole()
{
    static bool isConsoleHidden = false;

    if (isConsoleHidden)
    {
        HWND activeWindow = GetActiveWindow();
        ShowWindow(GetConsoleWindow(), 1);
        SetFocus(activeWindow);
    }
    else
    {
        ShowWindow(GetConsoleWindow(), 0);
    }
    isConsoleHidden = !isConsoleHidden;
}


int main()
{
    je::Context context(WIDTH, HEIGHT, TITLE);
    if (!context.Window())
    {
        return -1;
    }

    // Create a shader program.
    je::Shader shader;
    LOG("Shader program " << shader.Program());

    input::Initialise(context);

    je::Batch batch(shader.Program());

    // Make a function to create random integers in a closed range.
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::function<int(int, int)> Rnd = [&](int lo, int hi) {
        std::uniform_int_distribution<int> distribution(lo, hi);
        return distribution(generator);
    };

    Textures textures;
    Playing playing(batch, textures, Rnd);
    Dedication dedication(batch, textures);
    Menu menu(batch, textures);

    // Loop.
    Screens currentScreen{ Screens::Dedication };
    while (!glfwWindowShouldClose(context.Window()))
    {
        input::UpdateInputState();
        if (input::wasViewPressed && !input::viewPressed)
        {
            ToggleConsole();
        }

        Screens newScreen = currentScreen;
        switch (currentScreen)
        {
        case Screens::Dedication:
            newScreen = dedication.Update();
            break;
        case Screens::Menu:
            newScreen = menu.Update();
            break;
        case Screens::Playing:
            newScreen = playing.Update();
            break;
        case Screens::Quit:
            glfwSetWindowShouldClose(context.Window(), GL_TRUE);
            break;
        }
        if (newScreen != currentScreen)
        {
            // TODO: exit the last screen.
            currentScreen = newScreen;
            if (currentScreen == Screens::Playing)
            {
                playing.Start();
            }
            // TODO: enter the new screen.
        }

        // Clear the colour buffer.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Set the viewport position and size.
        glViewport(0, 0, WIDTH, HEIGHT);

        // Draw the batch.
        batch.Begin(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        switch (currentScreen)
        {
        case Screens::Dedication:
            dedication.Draw();
            break;
        case Screens::Menu:
            menu.Draw();
            break;
        case Screens::Playing:
            playing.Draw();
            break;
        }
        batch.End();

        // Swap buffers.
        glfwSwapBuffers(context.Window());
    }

    return 0;
}
