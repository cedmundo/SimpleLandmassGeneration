#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <jungle/style_jungle.h>
#include <stb_perlin.h>

#define SEED_MIN 0
#define SEED_MAX 1000000

#define SCALE_MIN 1
#define SCALE_DEF 10
#define SCALE_MAX 1000
#define SCALE_FACTOR 10.0f

#define OFFSET_X_MIN (-100)
#define OFFSET_X_DEF 0
#define OFFSET_X_MAX 100

#define OFFSET_Y_MIN (-100)
#define OFFSET_Y_DEF 0
#define OFFSET_Y_MAX 100

#define WIDTH_MIN 100
#define WIDTH_DEF 500
#define WIDTH_MAX 10000

#define HEIGHT_MIN 100
#define HEIGHT_DEF 500
#define HEIGHT_MAX 10000

#define OCTAVES_MIN 1
#define OCTAVES_DEF 6
#define OCTAVES_MAX 10

#define GAIN_MIN 10
#define GAIN_DEF 50
#define GAIN_MAX 100
#define GAIN_FACTOR 100.0f

#define LACUNARITY_MIN 1
#define LACUNARITY_DEF 200
#define LACUNARITY_MAX 1000
#define LACUNARITY_FACTOR 100.0f

typedef struct {
    int seed;
    bool seed_edit_mode;
    bool clicked_random_seed;

    int offset_x;
    bool offset_x_edit_mode;

    int offset_y;
    bool offset_y_edit_mode;

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

typedef struct {
    Image heightmap;
    Texture2D heightmap_texture;
    Vector2 position;
} ProceduralMap;

void DrawGUI(GeneratorOptions *options) {
    GuiPanel((Rectangle) { .x = 0, .y = 0, .width=200, .height=720 }, "Options");

    // Seed
    GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
    GuiSetStyle(SPINNER, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
    float layout_y = 5.0f;

    if (GuiValueBox((Rectangle) { .x = 5, .y = (layout_y+=25), .width=155, .height = 20 }, " Seed", &options->seed, SEED_MIN, SEED_MAX, options->seed_edit_mode)) {
        options->seed_edit_mode = !options->seed_edit_mode;
    }
    options->clicked_random_seed = GuiButton((Rectangle) { .x = 5, .y = (layout_y+=25), .width=190, .height = 20}, "Generate random");

    // Scale
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " S", &options->scale, SCALE_MIN, SCALE_MAX, options->scale_edit_mode)) {
        options->scale_edit_mode = !options->scale_edit_mode;
    }

    // Offset X
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " X", &options->offset_x, OFFSET_X_MIN, OFFSET_X_MAX, options->offset_x_edit_mode)) {
        options->offset_x_edit_mode = !options->offset_x_edit_mode;
    }

    // Offset Y
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " Y", &options->offset_y, OFFSET_Y_MIN, OFFSET_Y_MAX, options->offset_y_edit_mode)) {
        options->offset_y_edit_mode = !options->offset_y_edit_mode;
    }

    // Width
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " W", &options->width, WIDTH_MIN, WIDTH_MAX, options->width_edit_mode)) {
        options->width_edit_mode = !options->width_edit_mode;
    }

    // Height
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " H", &options->height, HEIGHT_MIN, HEIGHT_MAX, options->height_edit_mode)) {
        options->height_edit_mode = !options->height_edit_mode;
    }

    // Octaves
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " O", &options->octaves, OCTAVES_MIN, OCTAVES_MAX, options->octaves_edit_mode)) {
        options->octaves_edit_mode = !options->octaves_edit_mode;
    }

    // Gain
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " G", &options->gain, GAIN_MIN, GAIN_MAX, options->gain_edit_mode)) {
        options->gain_edit_mode = !options->gain_edit_mode;
    }

    // Lacunarity
    if (GuiSpinner((Rectangle) { .x = 5, .y = (layout_y+=25), .width=175, .height = 20 }, " L", &options->lacunarity, LACUNARITY_MIN, LACUNARITY_MAX, options->lacunarity_edit_mode)) {
        options->lacunarity_edit_mode = !options->lacunarity_edit_mode;
    }

    options->generate = GuiButton((Rectangle) { .x = 5, .y = (layout_y+=25), .width=190, .height = 20}, "Generate");
}

ProceduralMap *NewProceduralMap(Vector2 position) {
    ProceduralMap *map = MemAlloc(sizeof(ProceduralMap));
    map->position = position;
    return map;
}

void UnloadProceduralMap(ProceduralMap *map) {
    if (IsTextureReady(map->heightmap_texture)) {
        UnloadTexture(map->heightmap_texture);
    }

    if (IsImageReady(map->heightmap)) {
        UnloadImage(map->heightmap);
    }

    MemFree(map);
}

float PerlinNoise2D(float x, float y, float lacunarity, float gain, int octaves, int seed) {
    // This is basically a copy-paste from raylib, whoever it handles custom seed with octaves
    int i;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float sum = 0.0f;

    for (i = 0; i < octaves; i++) {
        sum += stb_perlin_noise3_seed(x*frequency,y*frequency,1.0f*frequency,0,0,0,(unsigned char)i+seed)*amplitude;
        frequency *= lacunarity;
        amplitude *= gain;
    }
    return sum;
}

void GenerateProceduralMap(ProceduralMap *map, GeneratorOptions options) {
    // This is basically a copy-paste from raylib, whoever it handles custom options
    size_t total_size = options.width * options.height;
    Color *pixels = MemAlloc(sizeof(Color) * total_size);
    int offset_x = options.offset_x;
    int offset_y = options.offset_y;
    int seed = options.seed;
    int width = options.width;
    int height = options.height;
    float scale = (float)options.scale / SCALE_FACTOR;
    float lacunarity = (float)options.lacunarity / LACUNARITY_FACTOR;
    float gain = (float)options.gain / GAIN_FACTOR;
    int octaves = options.octaves;

    for (int y = 0; y < options.height; y++) {
        for (int x = 0;x < options.width; x++) {
            float nx = (float)(x + offset_x)*(scale/(float)width);
            float ny = (float)(y + offset_y)*(scale/(float)height);
            float p = PerlinNoise2D(nx, ny, lacunarity, gain, octaves, seed);
            p = Clamp(p, -1.0f, 1.0f);

            float np = (p + 1.0f)/2.0f;

            int intensity = (int)(np*255.0f);
            pixels[y*width + x] = (Color) { intensity, intensity, intensity, 255 };
        }
    }

    if (IsTextureReady(map->heightmap_texture)) {
        UnloadTexture(map->heightmap_texture);
    }

    if (IsImageReady(map->heightmap)) {
        UnloadImage(map->heightmap);
    }

    map->heightmap = (Image) {
        .data = pixels,
        .width = width,
        .height = height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1,
    };
    map->heightmap_texture = LoadTextureFromImage(map->heightmap);
}

void DrawProceduralMap(ProceduralMap *map) {
    if (IsTextureReady(map->heightmap_texture)) {
        DrawTexture(map->heightmap_texture, (int)map->position.x, (int)map->position.y, WHITE);
    }
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
            .offset_x = OFFSET_X_DEF,
            .offset_x_edit_mode = false,
            .offset_y = OFFSET_Y_DEF,
            .offset_y_edit_mode = false,
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
    ProceduralMap *map = NewProceduralMap((Vector2) { 210, 10 });

    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            // Draw procedural map
            DrawProceduralMap(map);

            // Draw GUI
            DrawGUI(&options);

            // Handle input (from drawing/updating GUI):
            // Generate random value
            if (options.clicked_random_seed) {
                options.seed = (int)GetRandomValue(SEED_MIN, SEED_MAX);
            }

            // Generate map
            if (options.generate) {
                GenerateProceduralMap(map, options);
            }
        }
        EndDrawing();
    }

    UnloadProceduralMap(map);
    CloseWindow();
    return 0;
}
