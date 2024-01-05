#include "shared.c"

BOOL opened_replay = FALSE;
BOOL loaded_frame = FALSE;
struct SimSettings settings;

struct Sheep *frame_sheep;
int sheep_count;

struct Food *frame_food;
int food_count;

void FreeFrame()
{
    if (!opened_replay) {return;}
    opened_replay = FALSE;
    free(frame_sheep);
}

int OpenReplay(struct save_pointers *save)
{
    openSave(save, "rb");
    opened_replay = TRUE;
}

int LoadFrame(struct save_pointers *save)
{
    FreeFrame();

    fseek(save->sim_settings, 0, SEEK_SET);
    read_sim_settings(save->sim_settings, &settings);

    long frame_atlas_pos = getAtlasPosition(replay_frame);
    fseek(save->tick_atlas, getAtlasPosition(replay_frame), SEEK_SET);

    long frame_pos;
    fread(&frame_pos, sizeof(long), 1, save->tick_atlas);

    fseek(save->tick_store, frame_pos, SEEK_SET);

    // Getting Sheep Count
    fread(&sheep_count, sizeof(int), 1, save->tick_store);
    frame_sheep = malloc(sizeof(struct Sheep) * sheep_count);
    
    // Reading The Sheep
    for (int i = 0; i < sheep_count; i++)
    {
        readVariableSheep(save, &frame_sheep[i]);
        readStaticSheep(save, &frame_sheep[i], frame_sheep[i].id);
    }   

    // Getting the Food Count
    fread(&food_count, sizeof(int), 1, save->tick_store);
    frame_food = malloc(sizeof(struct Food) * food_count);

    // Reading The Sheep
    for (int i = 0; i < sheep_count; i++)
    {
        readVariableFood(save, &frame_food[i]);
        readStaticFood(save, &frame_food[i], frame_food[i].id);
    }
}

#define REPLAY_BACKGROUND_COLOR RGB(200, 200, 200)

#define REPLAY_SHEEP_COLOR RGB(0, 0, 0)
#define REPLAY_SHEEP_RADIUS 5

#define REPLAY_FOOD_COLOR RGB(0, 255, 0)
#define REPLAY_FOOD_RADIUS 3

void drawCircle(HDC hdc, float x, float y, float r)
{
    Ellipse(hdc, x-r, y-r,x+r, y+r);
}

void DrawReplay(HDC bitmap, int width, int height)
{
    // Drawing Background
    HBRUSH backgroundBrush = (HBRUSH) CreateSolidBrush(REPLAY_BACKGROUND_COLOR);
    RECT rect = {0, 0, width, height};
    FillRect(bitmap, &rect, backgroundBrush);
    DeleteObject(backgroundBrush);

    // Drawing the Sheep
    HBRUSH sheepBrush = (HBRUSH) CreateSolidBrush(REPLAY_SHEEP_COLOR);
    SelectObject(bitmap, sheepBrush);

    // 0, 0 is bottom left
    printf("sheep count, %d\n", sheep_count);
    printf("map size %f\n", settings.sim_map_size);
    for (int i = 0; i < sheep_count; i++)
    {
        struct Sheep *sheep = &frame_sheep[i];

        double x = sheep->x/settings.sim_map_size * width;
        double y = (1 - sheep->y/settings.sim_map_size) * height;

        drawCircle(bitmap, x, y, REPLAY_SHEEP_RADIUS);

        // Drawing the view direction
        MoveToEx(bitmap, x, height-y, NULL);
        LineTo(bitmap, x + cos(sheep->a)*20, height-(y + sin(sheep->a)*20));
    }

    DeleteObject(sheepBrush);

    HBRUSH foodBrush = (HBRUSH) CreateSolidBrush(REPLAY_FOOD_COLOR);
    SelectObject(bitmap, foodBrush);

    for (int i = 0; i < food_count; i++)
    {
        struct Food *food = &frame_food[i];

        double x = food->x/settings.sim_map_size * width;
        double y = (1 - food->y/settings.sim_map_size) * height;

        drawCircle(bitmap, x, y, REPLAY_SHEEP_RADIUS);
    }
    DeleteObject(foodBrush);
}