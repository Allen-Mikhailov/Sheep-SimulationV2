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
    free(frame_food);
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

    #ifdef STORE_SHEEP
    // Getting Sheep Count
    fread(&sheep_count, sizeof(int), 1, save->tick_store);
    frame_sheep = malloc(sizeof(struct Sheep) * sheep_count);
    
    // Reading The Sheep
    for (int i = 0; i < sheep_count; i++)
    {
        readVariableSheep(save, &frame_sheep[i]);
        readStaticSheep(save, &frame_sheep[i], frame_sheep[i].id);
    }

    printf("Loaded Sheep\n");

    #endif

    #ifdef STORE_FOOD
    // Getting the Food Count
    fread(&food_count, sizeof(int), 1, save->tick_store);
    frame_food = malloc(sizeof(struct Food) * food_count);

    // Reading The Food
    for (int i = 0; i < food_count; i++)
    {
        readVariableFood(save, &frame_food[i]);
        readStaticFood(save, &frame_food[i], frame_food[i].id);
    }

    printf("Loaded Food\n");

    #endif
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

void DrawReplay(HDC bitmap, int size)
{
    // Drawing Background
    HBRUSH backgroundBrush = (HBRUSH) CreateSolidBrush(REPLAY_BACKGROUND_COLOR);
    RECT rect = {0, 0, size, size};
    FillRect(bitmap, &rect, backgroundBrush);
    DeleteObject(backgroundBrush);

    printf("MapSize: %f, Sheep: %d, Food: %d\n", settings.sim_map_size, sheep_count, food_count);

    // Drawing the Sheep
    HBRUSH sheepBrush = (HBRUSH) CreateSolidBrush(REPLAY_SHEEP_COLOR);
    SelectObject(bitmap, sheepBrush);

    // The simulation is a square
    const float scale = (size / settings.sim_map_size);
    const float r = settings.sheep_view_distance*scale; 

    // 0, 0 is bottom left
    for (int i = 0; i < sheep_count; i++)
    {
        struct Sheep *sheep = &frame_sheep[i];

        float x = sheep->x*scale;
        float y = (size - sheep->y*scale);

        drawCircle(bitmap, x, y, REPLAY_SHEEP_RADIUS);

        float leftAngle = sheep->a - settings.sheep_view_angle;
        float rightAngle = sheep->a + settings.sheep_view_angle;

        // Drawing the view direction
        MoveToEx(bitmap, x, y, NULL);
        LineTo(bitmap, x + cos(leftAngle)*r, y - sin(leftAngle)*r);

        MoveToEx(bitmap, x, y, NULL);
        LineTo(bitmap, x + cos(rightAngle)*r, y - sin(rightAngle)*r);
    }

    DeleteObject(sheepBrush);

    HBRUSH foodBrush = (HBRUSH) CreateSolidBrush(REPLAY_FOOD_COLOR);
    SelectObject(bitmap, foodBrush);

    for (int i = 0; i < food_count; i++)
    {
        struct Food *food = &frame_food[i];

        double x = food->x*scale;
        double y = size - food->y*scale;

        drawCircle(bitmap, x, y, REPLAY_FOOD_RADIUS);
    }
    DeleteObject(foodBrush);
}