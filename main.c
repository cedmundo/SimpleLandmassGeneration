#include <raylib.h>

int main() {
    InitWindow(800, 800, "SimpleLandmassGeneration");
    SetWindowMonitor(0);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawText("Hello world", 100, 100, 28, WHITE);

            DrawRectangle(0, 0, 80, 20, WHITE);
            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
