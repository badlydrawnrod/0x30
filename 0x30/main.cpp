#include "Constants.h"
#include "Dedication.h"
#include "Buttons.h"
#include "Menu.h"
#include "Pit.h"
#include "PitRenderer.h"
#include "Playing.h"
#include "Progress.h"
#include "TextRenderer.h"
#include "Textures.h"
#include "Types.h"

#include "je/Batch.h"
#include "je/Context.h"
#include "je/Human.h"
#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Shaders.h"
#include "je/Sound.h"
#include "je/Textures.h"
#include "je/Time.h"
#include "je/Types.h"

#if defined(__EMSCRIPTEN__)
#include "emscripten.h"
#include "emscripten/fetch.h"
#endif

#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#undef APIENTRY
#include <Windows.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <iomanip>
#include <random>
#include <sstream>


#if defined(_WIN32)
struct Console
{
    static inline bool isConsoleHidden{ false };

    static void Show()
    {
        HWND activeWindow = GetActiveWindow();
        ShowWindow(GetConsoleWindow(), 1);
        SetFocus(activeWindow);
        isConsoleHidden = false;
    }

    static void Hide()
    {
        ShowWindow(GetConsoleWindow(), 0);
        isConsoleHidden = true;
    }

    static void Toggle()
    {
        if (isConsoleHidden)
        {
            Show();
        }
        else
        {
            Hide();
        }
    }

    // Show the console and wait for the user to press Enter.
    static void Oops()
    {
        Show();
        SetFocus(GetConsoleWindow());
        std::cout << "\nPress [ENTER] to close this window...\n";
        while (std::getchar() != '\n')
        {
        }
    }
};
#endif

class Game
{
public:
    Game(std::function<int(int, int)>& rnd);
    bool ShouldQuit();
    void Update(double t, double dt);
    void Draw(double t);

private:
    je::Context context;
    je::SoundSystem soundSystem;

    je::Shader shader;
    je::Batch batch;
    Progress progress_;
    Textures textures;
    Sounds sounds;

    Buttons buttons_;

    Playing playing;
    Dedication dedication;
    Menu menu;

    Screens currentScreen{ Screens::Dedication };
};


Game::Game(std::function<int(int, int)>& rnd) :
    context{ je::Context(WIDTH, HEIGHT, TITLE) },
    shader{ je::Shader() },
    batch{ shader.Program() },
    playing{ buttons_, progress_, batch, textures, sounds, rnd },
    dedication{ buttons_, batch, textures, sounds },
    menu{ buttons_, progress_, batch, textures }
{
    LOG("Shader program " << shader.Program());
    LOG("Finished initialising input");
    sounds.Load();
    LOG("Finished loading sounds");

    // Tell the input that we want to know about keyboard events.
    je::Human::Instance()->OnKeyboardEvent([this](GLFWwindow* window, int key, int scancode, int action, int mode) {
        buttons_.OnKeyEvent(window, key, scancode, action, mode);
    });

    // Tell the input that we want to know about gamepad button events.
    je::Human::Instance()->OnGamepadButtonEvent([this](SDL_JoystickID joystickId, Uint8 button, Uint8 state) {
        buttons_.OnGamepadButtonEvent(joystickId, button, state);
    });

    // Tell the input that we want to know about gamepad axis events.
    je::Human::Instance()->OnGamepadAxisEvent([this](SDL_JoystickID joystickId, Uint8 axis, Sint16 value) {
        buttons_.OnGamepadAxisEvent(joystickId, axis, value);
    });
}


bool Game::ShouldQuit()
{
    return context.ShouldQuit();
}


void Game::Update(double t, double dt)
{
    je::Human::Instance()->Update(t);
    buttons_.Update(t);

    if (buttons_.JustPressed(ButtonId::debug))
    {
#if defined(_WIN32)
        Console::Toggle();
#endif
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
        context.SetShouldQuit(true);
        break;
    }
    if (newScreen != currentScreen)
    {
        // TODO: exit the last screen.
        currentScreen = newScreen;
        if (currentScreen == Screens::Playing)
        {
            playing.Start(t, menu.SelectedLevel(), menu.SelectedMode());
        }
        else if (currentScreen == Screens::Menu)
        {
            menu.Start(t);
        }
        // TODO: enter the new screen.
    }
}


void Game::Draw(double t)
{
    context.Clear();
    context.SetViewport(0, 0, WIDTH, HEIGHT);

    batch.Begin(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    switch (currentScreen)
    {
    case Screens::Dedication:
        dedication.Draw(t);
        break;
    case Screens::Menu:
        menu.Draw(t);
        break;
    case Screens::Playing:
        playing.Draw(t);
        break;
    }
    batch.End();

    // Swap buffers.
    context.SwapBuffers();
}


static double t = 0.0;
static double dt = 1.0 / UPDATE_FPS;
static double lastTime = je::GetTime();
static double accumulator = 0.0;

static double lastDrawTime = je::GetTime();
static const double minDrawInterval = 1.0 / (2 * RENDER_FPS);
static Game* theGame;


static void main_loop(void)
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
        theGame->Update(t, dt);
        t += dt;
        accumulator -= dt;
    }

    // Draw. Don't cap the frame rate as the browser is probably doing it for us.
    theGame->Draw(t);
}

#if defined(__EMSCRIPTEN__)
#include <stdio.h>

static int filesDownloaded = 0;

extern "C" int Demo();

int Demo()
{
    printf("Hello, we reached 'Demo'\n");
    FILE* f = fopen("/data/music/adieu.ogg", "r");
    if (f)
    {
        fseek(f, 0, SEEK_END);
        long where = ftell(f);
        fclose(f);
        printf("adieu.ogg is %ld bytes\n", where);
    }
    printf("Goodbye, we finished 'Demo'\n");
    return 0;
}

void DownloadSucceeded(emscripten_fetch_t *fetch) {
    printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    emscripten_fetch_close(fetch); // Free data associated with the fetch.

}

void DownloadFailed(emscripten_fetch_t *fetch) {
    printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}

void WGetSucceeded(const char* filename)
{
    printf("WGet Successfully loaded %s\n", filename);

    ++filesDownloaded;
    if (filesDownloaded == 4)
    {
        // Sync so that we see the files.
        EM_ASM(
            FS.syncfs(false, function(err) {
            ccall('Demo', null, ['number'])
        });
        );
    }
}

void WGetFailed(const char* filename)
{
    printf("WGet Failed to load %s\n", filename);
}

#endif

int main()
{
    // TODO: obviously move this - it's just a test to see if it does _anything_.
#if defined(__EMSCRIPTEN__)
    // Spike: download some relatively large files to memory.
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_REPLACE | EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_PERSIST_FILE;
//    attr.attributes = EMSCRIPTEN_FETCH_REPLACE | EMSCRIPTEN_FETCH_PERSIST_FILE;
    attr.onsuccess = DownloadSucceeded;
    attr.onerror = DownloadFailed;

    emscripten_async_wget("music/adieu.ogg", "/data/music/adieu.ogg", WGetSucceeded, WGetFailed);
    emscripten_async_wget("music/gymnopedie1.ogg", "/data/music/gymnopedie1.ogg", WGetSucceeded, WGetFailed);
    emscripten_async_wget("music/hallelujah.ogg", "/data/music/hallelujah.ogg", WGetSucceeded, WGetFailed);
    emscripten_async_wget("music/minute.ogg", "/data/music/minute.ogg", WGetSucceeded, WGetFailed);

    emscripten_fetch(&attr, "music/adieu.ogg");
    emscripten_fetch(&attr, "music/gymnopedie1.ogg");
    emscripten_fetch(&attr, "music/hallelujah.ogg");
    emscripten_fetch(&attr, "music/minute.ogg");

    std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::function<int(int, int)> Rnd = [&](int lo, int hi) {
            std::uniform_int_distribution<int> distribution(lo, hi);
            return distribution(generator);
        };

    Game game(Rnd);
    theGame = &game;
    emscripten_set_main_loop(main_loop, -1, true);
#else
    try
    {
        // Make a function to create random integers in a closed range.
        std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::function<int(int, int)> Rnd = [&](int lo, int hi) {
            std::uniform_int_distribution<int> distribution(lo, hi);
            return distribution(generator);
        };

#if defined(_WIN32)
        Console::Hide();
#endif

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

                // Make like a gunslinger.
                game.Draw(t);
            }
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        LOG("Failed with exception: " << e.what());
#if defined(_WIN32)
        Console::Oops();
#endif

        return 1;
    }
    catch (...)
    {
        LOG("Failed with unknown exception");
#if defined(_WIN32)
        Console::Oops();
#endif

        return 1;
    }
#endif
}
