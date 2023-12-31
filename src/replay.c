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
    ticks = (struct Tick *) malloc(sizeof(struct Tick) * settings.sim_ticks);
    
    struct Tick *tick_head = ticks;
    for (int i = 0; i < settings.sim_ticks; i++)
    {
        int sheepCount = -1;
        fscanf("%d ", &sheepCount);
        tick_head->sheep_count = sheepCount;

        if (sheepCount == -1)
            printf("Failed to Load Sheep Count on tick %d", i);
            return 0;

        tick_head->sheep = (struct Sheep *) malloc(sizeof(struct Tick) * sheepCount);
        struct Sheep *sheed_head = tick_head->sheep;

        for (int j = 0; j < sheepCount; j++)
        {
            int mateId;
            read_sheep(fp, sheed_head, &mateId);
        }

        tick_head++;
    }
}

#define REPLAY_BACKGROUND_COLOR RGB(200, 200, 200)

void DrawReplay(HDC bitmap, int frame, int width, int height)
{
    struct Tick *tick = &ticks[frame];
    
    // Drawing Background
    HBRUSH backgroundBrush = (HBRUSH) CreateSolidBrush(REPLAY_BACKGROUND_COLOR);
    RECT rect = {0, 0, width, height};
    FillRect(bitmap, &rect, backgroundBrush);
    DeleteObject(backgroundBrush);


}