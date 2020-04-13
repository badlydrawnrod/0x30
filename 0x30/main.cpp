#include "Constants.h"
#include "Dedication.h"
#include "Input.h"
#include "Menu.h"
#include "Pit.h"
#include "PitRenderer.h"
#include "Playing.h"
#include "TextRenderer.h"
#include "Textures.h"

#include "je/Batch.h"
#include "je/Context.h"
#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Shaders.h"
#include "je/Sound.h"
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


class Game
{
public:
    Game(std::function<int(int, int)>& rnd);
    bool ShouldQuit();
    void Update(double t, double dt);
    void Draw();

private:
    je::Context context;
    je::SoundSystem soundSystem;

    je::Shader shader;
    je::Batch batch;
    Textures textures;
    Sounds sounds;
    Playing playing;
    Dedication dedication;
    Menu menu;

    Screens currentScreen{ Screens::Dedication };
};


Game::Game(std::function<int(int, int)>& rnd) :
    context{ je::Context(WIDTH, HEIGHT, TITLE) },
    shader{ je::Shader() },
    batch{ shader.Program() },
    playing{ batch, textures, sounds, rnd },
    dedication{ batch, textures, sounds },
    menu{ batch, textures }
{
    LOG("Shader program " << shader.Program());
    input::Initialise(context);
}


bool Game::ShouldQuit()
{
    return glfwWindowShouldClose(context.Window());
}


void Game::Update(double t, double dt)
{
    input::UpdateInputState(t);
    if (input::buttons.JustPressed(input::ButtonId::debug))
    {
        ToggleConsole();
    }

    Screens newScreen = currentScreen;
    switch (currentScreen)
    {
    case Screens::Dedication:
        newScreen = dedication.Update(t, dt);
        break;
    case Screens::Menu:
        newScreen = menu.Update(t, dt);
        break;
    case Screens::Playing:
        newScreen = playing.Update(t, dt);
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
            playing.Start(t, menu.SelectedLevel());
        }
        else if (currentScreen == Screens::Menu)
        {
            menu.Start(t, playing.MaxLevel(), playing.Scores());
        }
        // TODO: enter the new screen.
    }
}


void Game::Draw()
{
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
    // TODO: does this belong here?
    glfwSwapBuffers(context.Window());
}


int main()
{
    // Make a function to create random integers in a closed range.
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::function<int(int, int)> Rnd = [&](int lo, int hi) {
        std::uniform_int_distribution<int> distribution(lo, hi);
        return distribution(generator);
    };

    Game game(Rnd);

    double t = 0.0;
    double dt = 1.0 / UPDATE_FPS;
    double lastTime = je::GetTime();
    double accumulator = 0.0;

    double lastDrawTime = je::GetTime();
    const double minDrawInterval = 1.0 / RENDER_FPS;

    while (!game.ShouldQuit())
    {
        // Update using: https://gafferongames.com/post/fix_your_timestep/
        double now = je::GetTime();
        double delta = now - lastTime;
        if (delta >= 0.1)
        {
            delta = 0.1;
        }
        lastTime = now;
        accumulator += delta;
        while (accumulator >= dt)
        {
            game.Update(t, dt);
            t += dt;
            accumulator -= dt;
        }

        // Draw, potentially capping the frame rate.
        now = je::GetTime();
        double drawInterval = now - lastDrawTime;
        if (drawInterval >= minDrawInterval)
        {
            lastDrawTime = now;
            // Clear the colour buffer.
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Set the viewport position and size.
            glViewport(0, 0, WIDTH, HEIGHT);

            // Make like a gunslinger.
            game.Draw();
        }
    }

    return 0;
}
