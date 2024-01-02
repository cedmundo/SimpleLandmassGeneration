#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <jungle/style_jungle.h>

typedef struct {
    int i;
} AppGuiState;

AppGuiState DrawGUI(AppGuiState state) {
    GuiPanel((Rectangle) { .x = 0, .y = 0, .width=200, .height=720 }, "Options");
    return state;
}

int main() {
    InitWindow(1280, 720, "SimpleLandmassGeneration");
    SetWindowMonitor(0);
    SetTargetFPS(60);
    GuiLoadStyleJungle();

    AppGuiState state;
    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            state = DrawGUI(state);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
