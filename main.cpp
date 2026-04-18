#include "MeowEngine/MeowEngine.hpp"

int main() {
   Application application;
   application.createWindow(1000, 800, "MeowEngine");

   Graphics graphics(application);
   EventSystem eventSystem(application);
   eventSystem.addModule(ModuleQuitOnESC);

   Curve curve;
   curve.startPoint = {100, 100};
   curve.endPoint = {900, 700};
   curve.controlPoint1 = {100, 200};
   curve.controlPoint2 = {450, 100};

    while (application.running) {
        eventSystem.runEvents();
        graphics.clearBackground(SKYBLUE);

        if(Input::IsKeyDown(KEY_ONE)) curve.controlPoint1 = Input::getMousePos();
        if(Input::IsKeyDown(KEY_TWO)) curve.controlPoint2 = Input::getMousePos();

        graphics.drawCircle(curve.startPoint, 10, BLACK);
        graphics.drawCircle(curve.endPoint, 10, BLACK);
        graphics.drawCircle(curve.controlPoint1, 10, DARKGRAY);
        graphics.drawCircle(curve.controlPoint2, 10, BLACK);

        graphics.drawBezier(curve, MAROON, 10);

        graphics.update();
    }
    return 0;
}
