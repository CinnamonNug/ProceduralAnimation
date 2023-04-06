#include <iostream>


#include <raylib.h>
#include <BodySim.h>


int main() {
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Verlet Particle Simulation");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    BodySim bsm;

    while(!WindowShouldClose()) {
        bsm.TimeStep(GetFrameTime());

        BeginDrawing();
            bsm.Draw();
        EndDrawing();
    }
}






