#include "MeowEngine/MeowEngine.hpp"

int main() {
   Application application;
   application.createWindow(800, 600, "MeowEngine");

   Graphics graphics(application);
   EventSystem eventSystem(application);
   eventSystem.addModule(ModuleQuitOnESC);
   eventSystem.addModule(Input::ProcessEvent);

   Player player(application);
   player.rect = {{100, 100}, {50, 50}};
   player.color = MAROON;

   Rect groundPos = {{0, 550}, {600, 600}};
   Collider groundCollider{};
   groundCollider.rect = groundPos;

   Collider obstacle1{};
   obstacle1.rect = {{500, 470}, {50, 50}};

   std::vector<Collider> worldColliders = {groundCollider, obstacle1};

    while (application.running) {
        eventSystem.runEvents();


        player.update(application.deltaTime, worldColliders);
        graphics.clearBackground(SKYBLUE);
        graphics.drawRect(groundPos, DARKGRAY);
        graphics.drawRect(obstacle1.rect, BLACK);
        graphics.drawPlayer(player);

        graphics.update();
    }

    application.closeWindow();
    return 0;
}
