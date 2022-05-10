#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MY_PINK CLITERAL(Color){ 200, 109, 194, 240 }

static int picture_count = 0;

#define QUANDALE_SIZE 100

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Texture2D texture;
    int current_texture;
} Quandale;

typedef struct {
    Image image;
    Texture2D texture;
} Asset;

void load_pictures(char **files, Asset *assets)
{
    for (size_t i = 0; i < picture_count; ++i)
    {
        assets[i].image = LoadImage(files[i]);
    }
}

void resize_images(Asset *assets)
{
    for (size_t i = 0; i < picture_count; ++i)
    {
        ImageResize(&assets[i].image, QUANDALE_SIZE, QUANDALE_SIZE);
    }
}

void unload_pictures(Asset *assets)
{
    for (size_t i = 0; i < picture_count; ++i)
    {
        UnloadImage(assets[i].image);
    }
}

void load_textures(Asset *assets)
{
    for (size_t i = 0; i < picture_count; ++i)
    {
        assets[i].texture = LoadTextureFromImage(assets[i].image);
    }
}

void unload_textures(Asset *assets)
{
    for (size_t i = 0; i < picture_count; ++i)
    {
        UnloadTexture(assets[i].texture);
    }
}

void update_current_texture(Quandale *quandale, Asset *assets)
{
    if (quandale == NULL) {
        return;
    }

    if (quandale->current_texture < picture_count - 1) {
        quandale->current_texture++;
    } else if (quandale->current_texture == picture_count - 1) {
        quandale->current_texture = 0;
    }

    quandale->texture = assets[quandale->current_texture].texture;
}

#define WIDTH  640
#define HEIGHT 480

void move_quandale(Quandale *quandale, Asset *assets)
{
    if (quandale->pos.y + QUANDALE_SIZE >= HEIGHT) {
        quandale->vel.y *= -1;
        update_current_texture(quandale, assets);
    } else if (quandale->pos.y <= 0) {
        quandale->vel.y *= -1;
        update_current_texture(quandale, assets);
    } else if (quandale->pos.x + QUANDALE_SIZE >= WIDTH) {
        quandale->vel.x *= -1;
        update_current_texture(quandale, assets);
    } else if (quandale->pos.x <= 0) {
        quandale->vel.x *= -1;
        update_current_texture(quandale, assets);
    }
    quandale->pos = Vector2Add(quandale->pos, quandale->vel);
}

void prepare_assets(Asset *assets, char **files)
{
    load_pictures(files, assets);
    resize_images(assets);
    load_textures(assets);
}

void unload_assets(Asset *assets)
{
    unload_pictures(assets);
    unload_textures(assets);
}

#define ASSET_DIR "assets/"

// removes the '..' and '.' from the directory files and prepends 'assets/' to all the files
// returns the new array pointer, must be free'd
char **fix_dir_files(char **dir_files)
{
    int index = 0;
    char **new_mem = (char **) malloc(sizeof(char *) * (picture_count - 2));
    for (size_t i = 0; i < picture_count - 2; ++i) new_mem[i] = (char *) malloc(256 * sizeof(char));

    for (size_t i = 0; i < picture_count; ++i) {
        if (!strcmp("..", dir_files[i]) || !strcmp(".", dir_files[i])) {
            continue;
        }

        char temp_str[256];
        snprintf(temp_str, 256, "assets/%s", dir_files[i]);
        strncpy(new_mem[index], temp_str, 256);
        index++;
    }

    for (size_t i = 0; i < picture_count; ++i) free(dir_files[i]);
    free(dir_files);

    picture_count -= 2;

    return new_mem;
}

Asset *gen_asset_array()
{
    Asset *assets = malloc(sizeof(Asset) * picture_count);

    for (size_t i = 0; i < picture_count; ++i) assets[i] = (Asset) {0};

    return assets;
}

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "make it bounce");
    SetTargetFPS(30);

    char **dir_files = fix_dir_files(GetDirectoryFiles(ASSET_DIR, &picture_count));
    Asset *assets = gen_asset_array();
    prepare_assets(assets, dir_files);

    Quandale quandale = { .pos = (Vector2) {100, 100}, .vel = (Vector2) {3, 3}, .texture = assets[0].texture, .current_texture = 0 };

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(MY_PINK);

            move_quandale(&quandale, assets);
            DrawTextureV(quandale.texture, quandale.pos, WHITE);
        }
        EndDrawing();
    }

    unload_assets(assets);
    for (size_t i = 0; i < picture_count; ++i) free(dir_files[i]);

    free(dir_files);
    free(assets);

    CloseWindow();

    return 0;
}
