#include "MeowEngine/MeowEngine.hpp"

int main() {
    Application application;
    application.createWindow(800, 600, "MeowEngine Window");
    Graphics graphics(application);
    EventSystem event(application);
    event.addModule(ModuleQuitOnESC);

    while (application.running) {
        event.runEvents();
        graphics.clearBackground(WHITE);

        graphics.update();
    }
    return 0;
}