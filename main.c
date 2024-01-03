#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <jungle/style_jungle.h>
#include <stb_perlin.h>

#define SEED_MIN 0
#define SEED_MAX 1000000
#define SCALE_MIN 1
#define SCALE_DEF 10
#define SCALE_MAX 1000
#define WIDTH_MIN 100
#define WIDTH_DEF 100
#define WIDTH_MAX 10000
#define HEIGHT_MIN 100
#define HEIGHT_DEF 100
#define HEIGHT_MAX 10000
#define OCTAVES_MIN 1
#define OCTAVES_DEF 6
#define OCTAVES_MAX 10
#define GAIN_MIN 1
#define GAIN_DEF 50
#define GAIN_MAX 100
#define LACUNARITY_MIN 1
#define LACUNARITY_DEF 200
#define LACUNARITY_MAX 1000

typedef struct {
    int seed;
    bool seed_edit_mode;
    bool clicked_random_seed;

    int width;
    bool width_edit_mode;

    int height;
    bool height_edit_mode;

    int scale;
    bool scale_edit_mode;

    int octaves;
    bool octaves_edit_mode;

    int gain;
    bool gain_edit_mode;

    int lacunarity;
    bool lacunarity_edit_mode;

    bool generate;
} GeneratorOptions;

void DrawGUI(GeneratorOptions *options) {
    GuiPanel((Rectangle) { .x = 0, .y = 0, .width=200, .height=720 }, "Options");

    // Seed
    GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
    if (GuiValueBox((Rectangle) { .x = 5, .y = 30, .width=155, .height = 20 }, " Seed", &options->seed, SEED_MIN, SEED_MAX, options->seed_edit_mode)) {
        options->seed_edit_mode = !options->seed_edit_mode;
    }
    options->clicked_random_seed = GuiButton((Rectangle) { .x = 5, .y = 55, .width=190, .height = 20}, "Generate random");

    // Scale
    if (GuiValueBox((Rectangle) { .x = 5, .y = 80, .width=175, .height = 20 }, " S", &options->scale, SCALE_MIN, SCALE_MAX, options->scale_edit_mode)) {
        options->scale_edit_mode = !options->scale_edit_mode;
    }

    // Width
    if (GuiValueBox((Rectangle) { .x = 5, .y = 105, .width=175, .height = 20 }, " W", &options->width, WIDTH_MIN, WIDTH_MAX, options->width_edit_mode)) {
        options->width_edit_mode = !options->width_edit_mode;
    }

    // Height
    if (GuiValueBox((Rectangle) { .x = 5, .y = 130, .width=175, .height = 20 }, " H", &options->height, HEIGHT_MIN, HEIGHT_MAX, options->height_edit_mode)) {
        options->height_edit_mode = !options->height_edit_mode;
    }

    // Octaves
    if (GuiValueBox((Rectangle) { .x = 5, .y = 155, .width=175, .height = 20 }, " O", &options->octaves, OCTAVES_MIN, OCTAVES_MAX, options->octaves_edit_mode)) {
        options->octaves_edit_mode = !options->octaves_edit_mode;
    }

    // Gain
    if (GuiValueBox((Rectangle) { .x = 5, .y = 185, .width=175, .height = 20 }, " G", &options->gain, GAIN_MIN, GAIN_MAX, options->gain_edit_mode)) {
        options->gain_edit_mode = !options->gain_edit_mode;
    }

    // Lacunarity
    if (GuiValueBox((Rectangle) { .x = 5, .y = 210, .width=175, .height = 20 }, " L", &options->lacunarity, LACUNARITY_MIN, LACUNARITY_MAX, options->lacunarity_edit_mode)) {
        options->lacunarity_edit_mode = !options->lacunarity_edit_mode;
    }

    options->generate = GuiButton((Rectangle) { .x = 5, .y = 235, .width=190, .height = 20}, "Generate");
}

int main() {
    InitWindow(1280, 720, "SimpleLandmassGeneration");
    SetWindowMonitor(0);
    SetTargetFPS(60);
    GuiLoadStyleJungle();

    GeneratorOptions options = {
            .seed = 0,
            .seed_edit_mode = false,
            .clicked_random_seed = false,
            .scale = SCALE_DEF,
            .scale_edit_mode = false,
            .width = WIDTH_DEF,
            .width_edit_mode = false,
            .height = HEIGHT_DEF,
            .height_edit_mode = false,
            .octaves = OCTAVES_DEF,
            .octaves_edit_mode = false,
            .gain = GAIN_DEF,
            .gain_edit_mode = false,
            .lacunarity = LACUNARITY_DEF,
            .lacunarity_edit_mode = false,
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawGUI(&options);

            // Generate random value
            if (options.clicked_random_seed) {
                options.seed = (int)GetRandomValue(SEED_MIN, SEED_MAX);
            }

            // Generate map
            if (options.generate) {
                TraceLog(LOG_INFO, "Generating a map ...");
            }
        }
        EndDrawing();
    }


    CloseWindow();
    return 0;
}
