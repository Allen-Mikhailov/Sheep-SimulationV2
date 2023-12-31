#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "math_util.c"

#include "LinkedLists.c"
#include "structs.c"
#include "replay_util.c"

struct SimSettings *ss;

struct LinkedList *sheepList;
struct LinkedList *foodList;

int totalSheepCreated = 0;

struct LinkedList *grassChunks;
int chunks;

const int chunkRadiusX[] = {0, 0, 0, -1, -1, -1, 1, 1, 1};
const int chunkRadiusY[] = {0, 1, -1, 0, 1, -1, 0, 1, -1};

FILE *rfp;

double foodSpawnIndex = 0;
double sheepVisionTime = 0;

double random_pos()
{
    return (double) rand() / RAND_MAX * ss->sim_map_size;
}

int get_chunk(double x, double y)
{
    int cx = clamp(floor(x/ss->sim_grass_chunk_size) + 1, 1, chunks);
    int cy = clamp(floor(y/ss->sim_grass_chunk_size) + 1, 1, chunks);

    return cx + cy * (chunks+2);
}

void spawn_food()
{
    foodSpawnIndex += ss->sim_food_spawn_rate;

    while (foodSpawnIndex >= 1 && foodList->count < ss->sim_food_max)
    {
        struct Food * newFood = malloc(sizeof(struct Food));
        newFood->x = random_pos();
        newFood->y = random_pos();
        newFood->mainListNode = AddToList(foodList, newFood);

        // Chunk
        int chunk = get_chunk(newFood->x, newFood->y);
        struct LinkedList* grassChunk = grassChunks+chunk;
        newFood->chunk = grassChunk;
        newFood->chunkListNode = AddToList(grassChunk, newFood);

        foodSpawnIndex--;
    }
}

#pragma region Sheep Functions

struct Sheep* new_sheep()
{
    struct Sheep* sheep = malloc(sizeof(struct Sheep));
    sheep->age = 0;
    sheep->hunger = .5;
    
    sheep->a = (double) ( rand() / RAND_MAX ) * M_PI*2 - M_PI;
    sheep->pregnantPeriod = -1;
    sheep->gender = floor((double)rand()/RAND_MAX * 2);
    sheep->id = totalSheepCreated;
    totalSheepCreated++;

    return sheep;
}

void kill_sheep(struct LinkedListNode * sheep)
{
    RemoveFromList(sheepList, sheep);
    free(sheep->obj);
    free(sheep);
}

struct SheepVision
{
    struct Food *nFood;
    double nFoodDist;
    double nFoodAngle;
};

void sheep_vision(struct Sheep * sheep, struct SheepVision *vision)
{
    clock_t t = clock();

     // Food Vision
    vision->nFood = NULL;
    vision->nFoodDist = ss->sheep_view_distance;

    int sChunk = get_chunk(sheep->x, sheep->y);

    for (int i = 0; i < 9; i++)
    {
        int lsChunk = sChunk + chunkRadiusX[i] + chunkRadiusY[i] * (chunks+2);
        struct LinkedList * grassChunk = grassChunks + lsChunk;
        struct LinkedListNode *foodLHead = grassChunk->tail;

        while (foodLHead != NULL)
        {  
            struct Food *food = (struct Food *) (foodLHead->obj);
            
            double distance = hypot(sheep->x - food->x, sheep->y - food->y);
            double angle = atan2(food->y - sheep->y, food->x - sheep->x);
            double angleDif = compare_angles(sheep->a, angle);

            if (distance < vision->nFoodDist && fabs(angleDif) < ss->sheep_view_angle)
            {
                vision->nFood = foodLHead->obj;
                vision->nFoodDist = distance;
                vision->nFoodAngle = angleDif;
            }

            foodLHead = foodLHead->next;
        }
    }

    sheepVisionTime += ( (double) clock() - t ) / CLOCKS_PER_SEC;
}

void Birth(struct Sheep * s1)
{
    struct Sheep *newSheep = new_sheep();
    newSheep->x = s1->x;
    newSheep->y = s1->y;

    AddToList(sheepList, newSheep);
}

void female_sheep_tick(struct LinkedListNode * sheepNode)
{
    clock_t start = clock();
    struct Sheep *sheep = (struct Sheep*) (sheepNode->obj);
    if (sheep->age < ss->sheep_egg_min_age) {return;}

    if (!sheep->lookingForMate) 
    {
        if (floor(random() * ss->sheep_egg_chance) == 0)
            sheep->lookingForMate = 1;
    } 
    else if (sheep->pregnantPeriod == -1)
    {
        // Looking for a mate
        struct Sheep *closestPartner = NULL;
        double distance = ss->sheep_mate_distance;

        struct LinkedListNode *sheepLHead = sheepList->tail;
        while (sheepLHead != NULL)
        {
            struct Sheep *s2 = (struct Sheep *) (sheepLHead->obj);
            double dist = hypot(sheep->x-s2->x, sheep->y-s2->y);
            if (s2->gender == 0 && dist < distance )
            {
                distance = dist;
                closestPartner = s2;
            }
            sheepLHead = sheepLHead->next;
        }

        if (closestPartner != NULL)
        {
            // Cloning Sheep Data
            struct Sheep s2 = *closestPartner;

            sheep->mate = &s2;
            sheep->pregnantPeriod = 0;
        }
    } else {
        // printf("Pregnant");
        sheep->pregnantPeriod++;
        if (sheep->pregnantPeriod >= ss->sheep_pregnant_period 
            && sheep->hunger > ss->sheep_pregnant_hunger_cost)
        {
            Birth(sheep);
            sheep->pregnantPeriod = -1;
            sheep->lookingForMate = 1;
        }
    }
}

void sheep_tick(struct LinkedListNode * sheepNode)
{
    struct Sheep *sheep = (struct Sheep*) (sheepNode->obj);
    sheep->age++;
    if (sheep->age >= ss->sheep_max_lifespan)
        kill_sheep(sheepNode);

    double TurnSpeed;
    double MoveSpeed;

    struct SheepVision vis;
    struct SheepVision *vision = &vis;
    sheep_vision(sheep, vision);

    // TEMP AI
    if (vision->nFood == NULL)
    {
        MoveSpeed = .4;
        TurnSpeed = .5;
    } else {
        TurnSpeed =  signbit(vision->nFoodAngle)?-1:1;
        MoveSpeed = 1;
    }

    // Movement
    sheep->a = atanRange(sheep->a + TurnSpeed * ss->sheep_max_turn_speed);
    sheep->x = clamp(sheep->x + cos(sheep->a) * MoveSpeed * ss->sheep_max_speed, 0, ss->sim_map_size);
    sheep->y = clamp(sheep->y + sin(sheep->a) * MoveSpeed * ss->sheep_max_speed, 0, ss->sim_map_size);

    // Eating
    if (vision->nFoodDist < ss->sheep_eating_range)
    {
        sheep->hunger = 1;
        RemoveFromList(foodList, vision->nFood->mainListNode);
        RemoveFromList(vision->nFood->chunk, vision->nFood->chunkListNode);
        // RemoveFromList(vision->nFood)
        // printf("Sheep ate food");
    }

    if (sheep->gender == 1)
        female_sheep_tick(sheepNode);

    // Hunger
    sheep->hunger -= ss->sheep_starve_rate;

    if (sheep->hunger <= 0)
        kill_sheep(sheepNode);
}


#pragma endregion 

void tick(struct LinkedList *sheepList, struct LinkedList *foodList, struct TickData * tData )
{
    struct LinkedListNode *sheepLHead = sheepList->tail;
    while (sheepLHead != NULL)
    {   
        struct LinkedListNode *nextSheep = sheepLHead->next;
        sheep_tick(sheepLHead);
        sheepLHead = nextSheep;
    }

    spawn_food();   

    tData->sheepCount = sheepList->count;
    tData->grassCount = foodList->count;
}

void write_sheep_states(FILE *fp)
{
    struct LinkedListNode *sheepLHead = sheepList->tail;
    struct Sheep *sheep;
    fprintf(fp, "%d ", sheepList->count);
    while (sheepLHead != NULL)
    {   
        struct LinkedListNode *nextSheep = sheepLHead->next;
        sheep = (struct Sheep *) sheepLHead->obj;
        write_sheep(fp, sheep);
        sheepLHead = nextSheep;
    }
}

void refresh_replay_pointer()
{
    fclose(rfp);
    rfp = fopen("./replay.sim","a");
}

int run_simulation()
{
    struct SimSettings settings;
    ss = &settings;
    getDefaultSettings(ss);

    printf("Setting up simulation...\n");
    clock_t setupStart = clock();
    sheepList = newList();
    foodList = newList();

    // Creating Grass Chunks
    chunks = ceil((double) ss->sim_map_size / ss->sim_grass_chunk_size);
    grassChunks = malloc(sizeof(struct LinkedList) * (chunks+2) * (chunks+2));
    struct LinkedList *grassChunksHead = grassChunks;
    for (int i = 0; i < (chunks+2) * (chunks+2); i++)
    {
        startList(grassChunksHead);
        grassChunksHead++;
    }

    // Tick Data
    struct TickData *tDataList = malloc(sizeof(struct TickData)*ss->sim_ticks);
    struct TickData *tDataHead = tDataList;

    printf("Opening Replay\n");

    rfp = fopen("./replay.sim","w");

    write_sim_settings(rfp, ss);
    write_token(rfp, R_SIM_START, 0);

    refresh_replay_pointer();

    printf("Wrote replay pointers\n");

    // Obj Set up
    for (int i = 0; i < ss->sim_starting_sheep; i++)
    {
        struct Sheep *sheep = new_sheep();
        sheep->x = random_pos();
        sheep->y = random_pos();
        AddToList(sheepList, sheep);
    }

    printf("Setup inital Sheep\n");

    // Running simulation
    printf("Started Simulation %fs\n", (setupStart-clock())/CLOCKS_PER_SEC);

    write_sheep_states(rfp);

    printf("Wrote initial Sheep states\n");

    double lastPercent = 0;
    double lastVisionTime = 0;
    for (int i = 0; i < ss->sim_ticks; i++)
    {
        // printf("tick: %d\n", i);
        tick(sheepList, foodList, tDataHead);
        tDataHead++;

        write_sheep_states(rfp);
        
        

        // grassChunksHead = grassChunks;
        // for (int i = 0; i < chunks+2; i++)
        // {
        //     for (int j = 0; j < chunks+2; j++)
        //     {
        //         printf("%d ", grassChunksHead->count);
        //         grassChunksHead++;
        //     }
        //     printf("\n");
        // }

        double percent = floor((double) i / ss->sim_ticks * (100))/100.0;
        if (lastPercent != percent)
        {
            lastPercent = percent;
            printf("%f%%", lastPercent*100);
            printf(" Sheep Count: %d", sheepList->count);
            printf(" Grass Count: %d", foodList->count);
            printf(" Vision Time: %f +%f", sheepVisionTime, 
                (sheepVisionTime-lastVisionTime)/ss->sim_ticks*100);
            printf("\n");
            // printf("%f%% Sheep Count: %d Vision Time : %f\n", lastPercent*100, sheepList->count, sheepVisionTime);

            lastVisionTime = sheepVisionTime;
        }
    }

    fclose(rfp);

    printf("Completed Simulation\n");

    printf("Writing CSV\n");
    FILE *fptr;
    fptr = fopen("./data.csv","w");
    if(fptr == NULL)
    {
        printf("Error!");   
        exit(1);             
    }

    fprintf(fptr, "Sheep Count, Grass Count\n");

    tDataHead = tDataList;

    for (int i = 0; i < ss->sim_ticks; i++)
    {
        fprintf(fptr, "%d,%d\n", tDataHead->sheepCount, tDataHead->grassCount);
        tDataHead++;
    }

    fclose(fptr);

    printf("Completed writing csv");

    return 0;
}