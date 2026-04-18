//
// Created by Stěpán Toman on 17.04.2026.
//

#ifndef MEOWENGINE_APPLICATION_HPP
#define MEOWENGINE_APPLICATION_HPP

#include <filesystem>
#include <vector>
#include <utility>

#include "Core.hpp"

class Application{

public:
    SDL_Renderer* renderer = nullptr;
    SDL_Window* window = nullptr;
    TTF_TextEngine* textEngine = nullptr;
    bool running = true;
    float deltaTime = 0.0f;
    int screenWidth, screenHeight;
    TTF_Font* font = nullptr;

    Application() = default;
    ~Application() {
        closeWindow();
    }

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&& other) noexcept {
        moveFrom(std::move(other));
    }

    Application& operator=(Application&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        closeWindow();
        moveFrom(std::move(other));
        return *this;
    }

    void createWindow(int width, int height, const string& title) {
        closeWindow();

        running = true;
        windowTitle = title;
        screenHeight = height;
        screenWidth = width;

        if (!SDL_Init(initFlags)) {
            running = false;
            return;
        }
        sdlInitialized = true;

        if (!TTF_Init()) {
            running = false;
            closeWindow();
            return;
        }
        ttfInitialized = true;

        window = SDL_CreateWindow(windowTitle.c_str(), screenWidth, screenHeight, windowFlags);
        if (!window) {
            running = false;
            closeWindow();
            return;
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            running = false;
            closeWindow();
            return;
        }

        // Keep presentation synchronized with the display to reduce tearing/stutter.
        SDL_SetRenderVSync(renderer, 1);

        textEngine = TTF_CreateRendererTextEngine(renderer);
        if (!textEngine) {
            running = false;
            closeWindow();
            return;
        }

        const string resolvedFontPath = resolveFontPath(fontPath);
        if (resolvedFontPath.empty()) {
            running = false;
            closeWindow();
            return;
        }

        font = TTF_OpenFont(resolvedFontPath.c_str(), fontSize);
        if (!font) {
            running = false;
            closeWindow();
        }
    }

    void closeWindow() {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        if (textEngine) {
            TTF_DestroyRendererTextEngine(textEngine);
            textEngine = nullptr;
        }
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        if (ttfInitialized) {
            TTF_Quit();
            ttfInitialized = false;
        }
        if (sdlInitialized) {
            SDL_Quit();
            sdlInitialized = false;
        }

        running = false;
    }
    static float getDeltaTime() {
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

    void setFontSize(float argFontSize) {
        fontSize = argFontSize;
    }

    void setFontPath(std::string argFontPath) {
        fontPath = std::move(argFontPath);
    }

private:
    string windowTitle;
    SDL_WindowFlags windowFlags = 0;
    SDL_InitFlags initFlags = SDL_INIT_VIDEO;
    string fontPath = "default";
    float fontSize = 30;
    bool sdlInitialized = false;
    bool ttfInitialized = false;

    static bool fileExists(const string& path) {
        if (path.empty()) {
            return false;
        }

        std::error_code ec;
        return std::filesystem::exists(path, ec) && !ec;
    }

    static string getDefaultFontPathForCurrentOS() {
#if defined(_WIN32)
        const std::vector<string> candidates = {
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/arial.ttf"
        };
#elif defined(__APPLE__)
        const std::vector<string> candidates = {
                "/System/Library/Fonts/Supplemental/Helvetica.ttc",
                "/System/Library/Fonts/Supplemental/Arial.ttf",
                "/Library/Fonts/Arial.ttf"
        };
#else
        const std::vector<string> candidates = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
        };
#endif

        for (const auto& candidate : candidates) {
            if (fileExists(candidate)) {
                return candidate;
            }
        }

        return "";
    }

    static string resolveFontPath(const string& configuredPath) {
        if (configuredPath == "default") {
            return getDefaultFontPathForCurrentOS();
        }

        if (fileExists(configuredPath)) {
            return configuredPath;
        }

        return "";
    }

    void moveFrom(Application&& other) {
        renderer = other.renderer;
        window = other.window;
        textEngine = other.textEngine;
        running = other.running;
        deltaTime = other.deltaTime;
        screenWidth = other.screenWidth;
        screenHeight = other.screenHeight;
        font = other.font;
        windowTitle = std::move(other.windowTitle);
        windowFlags = other.windowFlags;
        initFlags = other.initFlags;
        fontPath = std::move(other.fontPath);
        fontSize = other.fontSize;
        sdlInitialized = other.sdlInitialized;
        ttfInitialized = other.ttfInitialized;

        other.renderer = nullptr;
        other.window = nullptr;
        other.textEngine = nullptr;
        other.font = nullptr;
        other.sdlInitialized = false;
        other.ttfInitialized = false;
        other.running = false;
    }

};

#endif //MEOWENGINE_APPLICATION_HPP
