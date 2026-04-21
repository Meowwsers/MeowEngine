//
// Created by Stěpán Toman on 17.04.2026.
//

#ifndef MEOWENGINE_EVENTSYSTEM_HPP
#define MEOWENGINE_EVENTSYSTEM_HPP

#include "Core.hpp"
#include "Application.hpp"
#include "Input.hpp"
#include <functional>
#include <utility>
#include <vector>

using EventModule = std::function<bool(Application&, const SDL_Event&)>;

inline bool ModuleQuitOnESC(Application& application, const SDL_Event& event) {

    if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
        application.running = false;
        return true;
    }

    return false;
}

class EventSystem {
public:
    explicit EventSystem(Application& application) : application(application), event{} {}

    SDL_Event event;


    void runEvents() {
        application.deltaTime = application.getDeltaTime();
        Input::NewFrame();

        while (SDL_PollEvent(&event)) {
            // Quit is always handled by the engine, independent of loaded modules.
            if (event.type == SDL_EVENT_QUIT) {
                application.running = false;
                continue;
            }

            Input::ProcessEvent(application, event);

            for (const auto& module : modules) {
                if (module(application, event)) {
                    break;
                }
            }
        }
    }

    void addModule(EventModule module) {
        modules.emplace_back(std::move(module));
    }

    void clearModules() {
        modules.clear();
    }

private:
    Application& application;
    std::vector<EventModule> modules;

};

#endif //MEOWENGINE_EVENTSYSTEM_HPP
