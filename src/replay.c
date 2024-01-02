#include "shared.c"

struct Tick {
    struct Sheep *sheep;
    int sheep_count;
};

BOOL loaded_replay = FALSE;
struct SimSettings settings;
struct Tick *ticks;

void FreeReplay()
{
    if (!loaded_replay) {return;}
    loaded_replay = FALSE;
}

int LoadReplay(FILE *fp)
{
    FreeReplay();

    loaded_replay = TRUE;
    read_sim_settings(fp, &settings);
    ticks = (struct Tick *) malloc(sizeof(struct Tick) * settings.sim_ticks+1);
    
    struct Tick *tick_head = ticks;
    for (int i = 0; i < settings.sim_ticks+1; i++)
    {
        int sheepCount = -1;
        fscanf(fp, "%d ", &sheepCount);
        tick_head->sheep_count = sheepCount;

        if (sheepCount == -1) {
            printf("Failed to Load Sheep Count on tick %d", i);
            return 0;
        }

        tick_head->sheep = (struct Sheep *) malloc(sizeof(struct Sheep) * sheepCount);
        struct Sheep *sheed_head = tick_head->sheep;

        for (int j = 0; j < sheepCount; j++)
        {
            int mateId;
            read_variable_sheep(fp, sheed_head, &mateId);
            sheed_head++;
        }

        tick_head++;
    }
}

#define REPLAY_BACKGROUND_COLOR RGB(200, 200, 200)
#define REPLAY_SHEEP_COLOR RGB(0, 0, 0)
#define REPLAY_SHEEP_RADIUS 5

void DrawReplay(HDC bitmap, int frame, int width, int height)
{
    struct Tick *tick = &ticks[frame];
    
    // Drawing Background
    HBRUSH backgroundBrush = (HBRUSH) CreateSolidBrush(REPLAY_BACKGROUND_COLOR);
    RECT rect = {0, 0, width, height};
    FillRect(bitmap, &rect, backgroundBrush);
    DeleteObject(backgroundBrush);

    HBRUSH sheepBrush = (HBRUSH) CreateSolidBrush(REPLAY_SHEEP_COLOR);
    SelectObject(bitmap, sheepBrush);

    // 0, 0 is bottom left
    printf("sheep count, %d\n", tick->sheep_count);
    printf("map size %f\n", settings.sim_map_size);
    for (int i = 0; i < tick->sheep_count; i++)
    {
        struct Sheep *sheep = &tick->sheep[i];

        double x = sheep->x/settings.sim_map_size * width;
        double y = sheep->y/settings.sim_map_size * height;

        Ellipse(bitmap, 
        x-REPLAY_SHEEP_RADIUS, 
        height-(y-REPLAY_SHEEP_RADIUS),
        x+REPLAY_SHEEP_RADIUS, 
        height-(y+REPLAY_SHEEP_RADIUS)
        );

        MoveToEx(bitmap, x, height-y, NULL);

        LineTo(bitmap, x + cos(sheep->a)*20, height-(y + sin(sheep->a)*20));
    }

    DeleteObject(backgroundBrush);
}