//
// Created by Stěpán Toman on 17.04.2026.
//

#ifndef MEOWENGINE_APPLICATION_HPP
#define MEOWENGINE_APPLICATION_HPP

#include "Core.hpp"

class Application{

public:
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    TTF_TextEngine* textEngine = nullptr;
    bool running = true;
    float deltaTime = 0.0f;
    int screenWidth, screenHeight;

    void createWindow(int width, int height, const string& title) {
        windowTitle = title;
        screenHeight = height;
        screenWidth = width;

        if (!SDL_Init(initFlags)) {
            running = false;
            return;
        }

        if (!TTF_Init()) {
            running = false;
            SDL_Quit();
            return;
        }

        window = SDL_CreateWindow(windowTitle.c_str(), screenWidth, screenHeight, windowFlags);
        if (!window) {
            running = false;
            TTF_Quit();
            SDL_Quit();
            return;
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            running = false;
            SDL_DestroyWindow(window);
            window = nullptr;
            TTF_Quit();
            SDL_Quit();
            return;
        }

        textEngine = TTF_CreateRendererTextEngine(renderer);
    }

    void closeWindow() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        TTF_Quit();
        SDL_Quit();
    }
    float getDeltaTime() {
        static Uint64 lastTime = 0;
        Uint64 currentTime = SDL_GetTicksNS();

        // Initialize lastTime on the first call to avoid a massive spike
        if (lastTime == 0) {
            lastTime = currentTime;
        }

        Uint64 diff = currentTime - lastTime;
        lastTime = currentTime;

        // Convert nanoseconds to seconds (1s = 1,000,000,000ns)

        return static_cast<float>(diff) / 1000000000.0f;
    }
private:
    string windowTitle;
    SDL_WindowFlags windowFlags = 0;
    SDL_InitFlags initFlags = SDL_INIT_VIDEO;

};

#endif //MEOWENGINE_APPLICATION_HPP
