#pragma once

#include "Time.h"

#if defined(__EMSCRIPTEN__)
#include "emscripten.h"
#endif

#include <memory>

namespace je
{
    const double UPDATE_FPS = 60.0;

    template<typename TGame>
    class Shell
    {
    public:
        explicit Shell(TGame&& game) : theGame{std::move(game)}
        {
        }

        void Update();
        void Draw();
        void Refresh();
        void RunMainLoop();
        static void EmRefresh(void* arg);

    private:
        double t = 0.0;
        double dt = 1.0 / UPDATE_FPS;
        double lastTime = je::GetTime();
        double accumulator = 0.0;
        double lastDrawTime = je::GetTime();

        TGame theGame;
    };

    template<typename TGame>
    void Shell<TGame>::Update()
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
    }

#if defined(__EMSCRIPTEN__)

    template<typename TGame>
    void Shell<TGame>::Draw()
    {
        // Draw. Don't cap the frame rate as the browser is probably doing it for us.
        theGame->Draw(t);
    }

#else

    template<typename TGame>
    void Shell<TGame>::Draw()
    {
        const double minDrawInterval = 1.0 / RENDER_FPS;

        // Draw, potentially capping the frame rate.
        double now = je::GetTime();
        double drawInterval = now - lastDrawTime;
        if (drawInterval >= minDrawInterval)
        {
            lastDrawTime = now;

            // Make like a gunslinger.
            theGame->Draw(t);
        }
    }

#endif

    template<typename TGame>
    void Shell<TGame>::EmRefresh(void* arg)
    {
        Shell* shell = reinterpret_cast<Shell*>(arg);
        shell->Refresh();
    }

    template<typename TGame>
    void Shell<TGame>::Refresh()
    {
        Update();
        Draw();
    }

#if defined(__EMSCRIPTEN__)

    template<typename TGame>
    void Shell<TGame>::RunMainLoop()
    {
        emscripten_set_main_loop_arg(EmRefresh, this, -1, true);
    }

#else

    template<typename TGame>
    void Shell<TGame>::RunMainLoop()
    {
        while (!theGame->ShouldQuit())
        {
            Refresh();
        }
    }

#endif
} // namespace je
