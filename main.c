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

#define OFFSET_X_MIN (-10000)
#define OFFSET_X_DEF 0
#define OFFSET_X_MAX 10000

#define OFFSET_Y_MIN (-10000)
#define OFFSET_Y_DEF 0
#define OFFSET_Y_MAX 10000

#define WIDTH_MIN 10
#define WIDTH_DEF 100
#define WIDTH_MAX 10000

#define HEIGHT_MIN 10
#define HEIGHT_DEF 100
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

#define MESH_SCALE_MIN 1
#define MESH_SCALE_DEF 100
#define MESH_SCALE_MAX 10000
#define MESH_SCALE_FACTOR 10.0f

#define MESH_HEIGHT_MIN 0
#define MESH_HEIGHT_DEF 10
#define MESH_HEIGHT_MAX 10000
#define MESH_HEIGHT_FACTOR 10.0f

#define GENERATE_MAP_SECS 0.03f

#define REGION_COUNT 9

typedef struct {
    bool auto_generate;
    int active_view;

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

    int mesh_scale;
    bool mesh_scale_edit_mode;

    int mesh_height;
    bool mesh_height_edit_mode;

    bool generate_map;
} ProceduralMapOptions;

typedef struct {
    float *noise_map;
    int height;
    int width;
    Texture2D noise_tex;
    Texture2D color_tex;
    Vector2 position;
    Model model;
} ProceduralMap;

enum ViewMode {
    VIEW_MODE_HEIGHT_MAP,
    VIEW_MODE_COLOR_MAP,
    VIEW_MODE_MESH_WIRED,
    VIEW_MODE_MESH_UNLIT,
};

void DrawGUI(ProceduralMapOptions *options) {
    GuiPanel((Rectangle) {.x = 0, .y = 0, .width=200, .height=720}, "Options");
    float layout_y = 5.0f;

    GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
    GuiSetStyle(SPINNER, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);

    // Preview mode
    GuiToggleGroup((Rectangle) {.x = 5, .y = (layout_y += 25), .width=46, .height = 20}, "HMAP;CMAP;WIR;MESH",
                   &options->active_view);

    // Seed
    if (GuiValueBox((Rectangle) {.x = 5, .y = (layout_y += 25), .width=155, .height = 20}, " Seed", &options->seed,
                    SEED_MIN, SEED_MAX, options->seed_edit_mode)) {
        options->seed_edit_mode = !options->seed_edit_mode;
    }
    options->clicked_random_seed = GuiButton((Rectangle) {.x = 5, .y = (layout_y += 25), .width=190, .height = 20},
                                             "Generate random");

    // Scale
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " S", &options->scale,
                   SCALE_MIN, SCALE_MAX, options->scale_edit_mode)) {
        options->scale_edit_mode = !options->scale_edit_mode;
    }

    // Offset X
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " X", &options->offset_x,
                   OFFSET_X_MIN, OFFSET_X_MAX, options->offset_x_edit_mode)) {
        options->offset_x_edit_mode = !options->offset_x_edit_mode;
    }

    // Offset Y
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " Y", &options->offset_y,
                   OFFSET_Y_MIN, OFFSET_Y_MAX, options->offset_y_edit_mode)) {
        options->offset_y_edit_mode = !options->offset_y_edit_mode;
    }

    // Width
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " W", &options->width,
                   WIDTH_MIN, WIDTH_MAX, options->width_edit_mode)) {
        options->width_edit_mode = !options->width_edit_mode;
    }

    // Height
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " H", &options->height,
                   HEIGHT_MIN, HEIGHT_MAX, options->height_edit_mode)) {
        options->height_edit_mode = !options->height_edit_mode;
    }

    // Octaves
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " O", &options->octaves,
                   OCTAVES_MIN, OCTAVES_MAX, options->octaves_edit_mode)) {
        options->octaves_edit_mode = !options->octaves_edit_mode;
    }

    // Gain
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " G", &options->gain,
                   GAIN_MIN, GAIN_MAX, options->gain_edit_mode)) {
        options->gain_edit_mode = !options->gain_edit_mode;
    }

    // Lacunarity
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=175, .height = 20}, " L", &options->lacunarity,
                   LACUNARITY_MIN, LACUNARITY_MAX, options->lacunarity_edit_mode)) {
        options->lacunarity_edit_mode = !options->lacunarity_edit_mode;
    }

    GuiLine((Rectangle) {.x = 5, .y = (layout_y += 25), .width=190, .height = 20}, "Mesh options");
    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=172, .height = 20}, " MS", &options->mesh_scale,
                   MESH_SCALE_MIN, MESH_SCALE_MAX, options->mesh_scale_edit_mode)) {
        options->mesh_scale_edit_mode = !options->mesh_scale_edit_mode;
    }

    if (GuiSpinner((Rectangle) {.x = 5, .y = (layout_y += 25), .width=172, .height = 20}, " MH", &options->mesh_height,
                   MESH_HEIGHT_MIN, MESH_HEIGHT_MAX, options->mesh_height_edit_mode)) {
        options->mesh_height_edit_mode = !options->mesh_height_edit_mode;
    }

    options->generate_map = GuiButton((Rectangle) {.x = 5, .y = (layout_y += 25), .width=190, .height = 20},
                                      "Generate map");

    GuiStatusBar((Rectangle) {.x=0, .y=(float) GetScreenHeight() - 20, .width=(float) GetScreenWidth(), .height = 20},
                 TextFormat("Idle | %d FPS", GetFPS()));
}

ProceduralMap *NewProceduralMap(Vector2 position) {
    ProceduralMap *map = MemAlloc(sizeof(ProceduralMap));
    map->position = position;
    return map;
}

void UnloadProceduralMap(ProceduralMap *map) {
    if (map->noise_map != NULL) {
        MemFree(map->noise_map);
    }

    if (IsTextureReady(map->noise_tex)) {
        UnloadTexture(map->noise_tex);
    }

    if (IsTextureReady(map->color_tex)) {
        UnloadTexture(map->color_tex);
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
        sum += stb_perlin_noise3_seed(x * frequency, y * frequency, 1.0f * frequency, 0, 0, 0,
                                      (unsigned char) i + seed) * amplitude;
        frequency *= lacunarity;
        amplitude *= gain;
    }
    return sum;
}

void GenerateNoiseMap(ProceduralMap *map, ProceduralMapOptions options) {
    size_t total_size = options.width * options.height;
    float *noise_values = MemAlloc(sizeof(Color) * total_size);
    int offset_x = options.offset_x;
    int offset_y = options.offset_y;
    int seed = options.seed;
    int width = options.width;
    int height = options.height;
    float scale = (float) options.scale / SCALE_FACTOR;
    float lacunarity = (float) options.lacunarity / LACUNARITY_FACTOR;
    float gain = (float) options.gain / GAIN_FACTOR;
    int octaves = options.octaves;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = (float) (x + offset_x) * (scale / (float) width);
            float ny = (float) (y + offset_y) * (scale / (float) height);
            float p = PerlinNoise2D(nx, ny, lacunarity, gain, octaves, seed);
            noise_values[y * width + x] = Clamp(p, -1.0f, 1.0f);
        }
    }

    map->noise_map = noise_values;
    map->height = height;
    map->width = width;
}

void GenerateHeightTexture(ProceduralMap *map) {
    int width = map->width;
    int height = map->height;
    int total_size = width * height;
    Color *heightmap_pixels = MemAlloc(sizeof(Color) * total_size);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float p = map->noise_map[y * width + x];
            float np = (p + 1.0f) / 2.0f;
            int intensity = (int) (np * 255.0f);
            heightmap_pixels[y * width + x] = (Color) {intensity, intensity, intensity, 255};
        }
    }

    Image image = {0};
    image.height = height;
    image.width = width;
    image.data = heightmap_pixels;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.mipmaps = 1;

    if (IsTextureReady(map->noise_tex) && map->noise_tex.width == width && map->noise_tex.height == height) {
        UpdateTexture(map->noise_tex, image.data);
    } else {
        if (IsTextureReady(map->noise_tex)) {
            UnloadTexture(map->noise_tex);
        }
        map->noise_tex = LoadTextureFromImage(image);
    }

    UnloadImage(image);
}

void GenerateColorTexture(ProceduralMap *map) {
    int width = map->width;
    int height = map->height;
    int total_size = width * height;
    Color *colormap_pixels = MemAlloc(sizeof(Color) * total_size);

    float region_brakes[REGION_COUNT] = {
            0.1f,
            0.2f,
            0.22f,
            0.25f,
            0.4f,
            0.5f,
            0.7f,
            0.8f,
            1.0f,
    };
    Color region_color[REGION_COUNT] = {
            ColorFromHSV(235.0f, 0.70f, 0.10f),
            ColorFromHSV(235.0f, 0.70f, 0.50f),
            ColorFromHSV(235.0f, 0.70f, 0.80f),
            ColorFromHSV(54.0f, 0.23f, 1.00f),
            ColorFromHSV(88.0f, 0.39f, 0.84f),
            ColorFromHSV(97.0f, 0.53f, 0.73f),
            ColorFromHSV(114.0f, 0.78f, 0.42f),
            ColorFromHSV(16.0f, 0.40f, 0.47f),
            ColorFromHSV(20.0f, 0.03f, 0.94f),
    };

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float p = map->noise_map[y * width + x];
            for (int r = 0; r < REGION_COUNT; r++) {
                if (p < region_brakes[r] || FloatEquals(p, region_brakes[r])) {
                    colormap_pixels[y * width + x] = region_color[r];
                    break;
                }
            }
        }
    }

    Image image = {0};
    image.height = height;
    image.width = width;
    image.data = colormap_pixels;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.mipmaps = 1;

    if (IsTextureReady(map->color_tex) && map->color_tex.width == width && map->color_tex.height == height) {
        UpdateTexture(map->color_tex, image.data);
    } else {
        if (IsTextureReady(map->color_tex)) {
            UnloadTexture(map->color_tex);
        }
        map->color_tex = LoadTextureFromImage(image);
    }

    UnloadImage(image);
}

void GenerateTerrainMesh(ProceduralMap *map, ProceduralMapOptions options) {
    int res_z = map->height;
    int res_x = map->width;
    float scale = (float) options.mesh_scale / MESH_SCALE_FACTOR;
    float overall_height = (float) options.mesh_height / MESH_HEIGHT_FACTOR;
    int vertex_count = res_x * res_z;

    Vector3 *vertices = MemAlloc(sizeof(Vector3) * vertex_count);
    for (int z = 0; z < res_z; z++) {
        float pos_z = ((float) z / (float) (res_z - 1) - 0.5f) * scale;
        for (int x = 0; x < res_x; x++) {
            float pos_x = ((float) x / (float) (res_x - 1) - 0.5f) * scale;
            float p = map->noise_map[x + z * res_x];
            vertices[x + z * res_x] = (Vector3) {pos_x, p * overall_height, pos_z};
        }
    }

    Vector3 *normals = MemAlloc(sizeof(Vector3) * vertex_count);
    for (int n = 0; n < vertex_count; n++) {
        normals[n] = (Vector3) {0.0f, 1.0f, 0.0f};
    }

    Vector2 *uvs = MemAlloc(sizeof(Vector2) * vertex_count);
    for (int v = 0; v < res_z; v++) {
        for (int u = 0; u < res_x; u++) {
            uvs[u + v * res_x] = (Vector2) {(float) u / (float) (res_x - 1), (float) v / (float) (res_z - 1)};
        }
    }

    int faces = (res_x - 1) * (res_z - 1);
    int *triangles = MemAlloc(sizeof(int) * faces * 6);
    int t = 0;
    for (int face = 0; face < faces; face++) {
        int i = face + face / (res_x - 1);
        triangles[t++] = i + res_x;
        triangles[t++] = i + 1;
        triangles[t++] = i;

        triangles[t++] = i + res_x;
        triangles[t++] = i + res_x + 1;
        triangles[t++] = i + 1;
    }

    Mesh mesh = {0};
    mesh.vertexCount = vertex_count;
    mesh.triangleCount = faces * 2;
    mesh.vertices = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.normals = MemAlloc(sizeof(float) * 3 * mesh.vertexCount);
    mesh.texcoords = MemAlloc(sizeof(float) * 2 * mesh.vertexCount);
    mesh.indices = MemAlloc(sizeof(unsigned short) * 3 * mesh.triangleCount);

    for (int i = 0; i < vertex_count; i++) {
        mesh.vertices[3 * i + 0] = vertices[i].x;
        mesh.vertices[3 * i + 1] = vertices[i].y;
        mesh.vertices[3 * i + 2] = vertices[i].z;
    }

    for (int i = 0; i < vertex_count; i++) {
        mesh.normals[3 * i + 0] = normals[i].x;
        mesh.normals[3 * i + 1] = normals[i].y;
        mesh.normals[3 * i + 2] = normals[i].z;
    }

    for (int i = 0; i < vertex_count; i++) {
        mesh.texcoords[2 * i + 0] = uvs[i].x;
        mesh.texcoords[2 * i + 1] = uvs[i].y;
    }

    for (int i = 0; i < mesh.triangleCount * 3; i++) {
        mesh.indices[i] = triangles[i];
    }

    if (IsModelReady(map->model)) {
        UnloadModel(map->model);
    }

    UploadMesh(&mesh, false);
    map->model = LoadModelFromMesh(mesh);
    SetMaterialTexture(map->model.materials, MATERIAL_MAP_DIFFUSE, map->color_tex);

    MemFree(vertices);
    MemFree(normals);
    MemFree(uvs);
    MemFree(triangles);
}

void GenerateProceduralMap(ProceduralMap *map, ProceduralMapOptions options) {
    GenerateNoiseMap(map, options);
    GenerateHeightTexture(map);
    GenerateColorTexture(map);
    if (!options.auto_generate) {
        GenerateTerrainMesh(map, options);
    }
}

void DrawProceduralMap(ProceduralMap *map, ProceduralMapOptions options, Camera3D camera) {
    if (options.active_view == VIEW_MODE_HEIGHT_MAP && IsTextureReady(map->noise_tex)) {
        DrawTexture(map->noise_tex, (int) map->position.x, (int) map->position.y, WHITE);
    } else if (options.active_view == VIEW_MODE_COLOR_MAP && IsTextureReady(map->color_tex)) {
        DrawTexture(map->color_tex, (int) map->position.x, (int) map->position.y, WHITE);
    } else if (options.active_view == VIEW_MODE_MESH_WIRED) {
        BeginMode3D(camera);
        if (IsModelReady(map->model)) {
            DrawModelWires(map->model, Vector3Zero(), 1.0f, WHITE);
        }
        DrawGrid(10, 1.0f);
        EndMode3D();
    } else if (options.active_view == VIEW_MODE_MESH_UNLIT) {
        BeginMode3D(camera);
        if (IsModelReady(map->model)) {
            DrawModel(map->model, Vector3Zero(), 1.0f, WHITE);
        }
        DrawGrid(10, 1.0f);
        EndMode3D();
    }
}

int main() {
    InitWindow(1280, 720, "SimpleLandmassGeneration");
    SetWindowMonitor(0);
    SetTargetFPS(60);
    GuiLoadStyleJungle();
    SetExitKey(KEY_NULL);

    Camera3D camera = {0};
    camera.position = (Vector3) {10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = (Vector3) {0.0f, 0.0f, 0.0f};      // Camera looking at point
    camera.up = (Vector3) {0.0f, 1.0f, 0.0f};          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    ProceduralMapOptions options = {
            .active_view = 0,
            .auto_generate = false,
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
            .mesh_scale = MESH_SCALE_DEF,
            .mesh_scale_edit_mode = false,
            .mesh_height = MESH_HEIGHT_DEF,
            .mesh_height_edit_mode = false,
    };
    ProceduralMap *map = NewProceduralMap((Vector2) {210, 10});

    float generate_map_timer = 0.0f;
    bool captured = false;
    while (!WindowShouldClose()) {
        BeginDrawing();
        {
            ClearBackground(BLACK);
            // Draw procedural map
            DrawProceduralMap(map, options, camera);

            // Draw GUI
            options.auto_generate = false;
            DrawGUI(&options);
            Rectangle drawing_area = {.x = 200, .y = 0, .width = (float) GetScreenWidth() -
                                                                 200, .height = (float) GetScreenHeight()};
            if (options.active_view == VIEW_MODE_MESH_WIRED || options.active_view == VIEW_MODE_MESH_UNLIT) {
                if (captured) {
                    UpdateCamera(&camera, CAMERA_FREE);
                }
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                    CheckCollisionPointRec(GetMousePosition(), drawing_area)) {
                    captured = true;
                    DisableCursor();
                }

                if (IsKeyPressed(KEY_ESCAPE)) {
                    captured = false;
                    EnableCursor();
                }
            } else {
                if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) &&
                    CheckCollisionPointRec(GetMousePosition(), drawing_area)) {
                    options.offset_x -= (int) GetMouseDelta().x;
                    options.offset_y -= (int) GetMouseDelta().y;
                    options.auto_generate = true;
                }

                if (GetMouseWheelMove() != 0.0f && CheckCollisionPointRec(GetMousePosition(), drawing_area)) {
                    options.scale -= (int) (GetMouseWheelMove() * 5);
                    options.auto_generate = true;
                }

            }

            // Handle input (from drawing/updating GUI):
            // Generate random value
            if (options.clicked_random_seed) {
                options.seed = (int) GetRandomValue(SEED_MIN, SEED_MAX);
                options.auto_generate = true;
            }

            // Generate map with button
            generate_map_timer += GetFrameTime();
            if (options.generate_map || options.auto_generate && generate_map_timer >= GENERATE_MAP_SECS) {
                generate_map_timer = 0.0f;
                GenerateProceduralMap(map, options);
            }
        }
        EndDrawing();
    }

    UnloadProceduralMap(map);
    CloseWindow();
    return 0;
}
