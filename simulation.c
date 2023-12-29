#include <stdio.h> 
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "LinkedLists.c"
#include "sim_structs.c"

#pragma region Sheep constants

#define SHEEP_STARVE_RATE 0.005
#define SHEEP_THIRST_RATE 0.005

#define SHEEP_EATING_RANGE 5

#define SHEEP_MAX_SPEED 1
#define SHEEP_MAX_TURN_SPEED .05

#define SHEEP_VIEW_DISTANCE 100
#define SHEEP_VIEW_ANGLE (M_1_PI / 4)

#define SHEEP_MAX_LIFE_SPAN 1000

#define SHEEP_EGG_MIN_AGE 100
#define SHEEP_EGG_CHANCE 100

#define SHEEP_MATE_DISTANCE 100
#define SHEEP_PREGNANT_PERIOD 100
#define SHEEP_PREGNANT_HUNGER_COST .5

#pragma endregion

#pragma region Simulation Constants

#define SIM_STARTING_SHEEP 100
#define SIM_TICKS 100
#define SIM_PERCENT .01

#define SIM_FOOD_SPAWN_RATE 100
#define SIM_FOOD_MAX 1000

#define SIM_MAP_SIZE 1000

#define SIM_GRASS_CHUNK_SIZE SHEEP_VIEW_DISTANCE

#pragma endregion

#pragma region Display Constants

// #define DISPLAY

#define DISPLAY_SIZE 10
#define DISPLAY_BUFFER_LENGTH DISPLAY_SIZE*(DISPLAY_SIZE+1)

#define DISPLAY_GRASS_CHAR '-'
#define DISPLAY_SHEEP_CHAR 'O'

#pragma endregion

#define CHARS_PER_INT 8

struct LinkedList *sheepList;
struct LinkedList *foodList;

struct LinkedList *grassChunks;
int chunks;

const int chunkRadiusX[] = {0, 0, 0, -1, -1, -1, 1, 1, 1};
const int chunkRadiusY[] = {0, 1, -1, 0, 1, -1, 0, 1, -1};


double foodSpawnIndex = 0;

double sheepVisionTime = 0;

#pragma region Math functions

double random_pos()
{
    return (double) rand() / RAND_MAX * SIM_MAP_SIZE;
}

double atanRange(double a)
{
    // printf("Inoput: %f\n", a);
    a = fmod(a, M_PI*2);
    // printf("FMOD: %f\n", a);
    if (a > M_PI)
        return a - M_PI*2;
    if (a < -M_PI)
        return a + M_PI*2;

    return a;
}

double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

double compare_angles(double a1, double a2)
{
    double dif = a2-a1;

    if(dif < -M_PI)
        dif += M_PI*2;
    if(dif > M_PI)
        dif -= M_PI*2;
    return dif;
}

#pragma endregion

int get_chunk(double x, double y)
{
    int cx = clamp(floor(x/SIM_GRASS_CHUNK_SIZE) + 1, 1, chunks);
    int cy = clamp(floor(y/SIM_GRASS_CHUNK_SIZE) + 1, 1, chunks);

    return cx + cy * (chunks+2);
}

void spawn_food()
{
    foodSpawnIndex += SIM_FOOD_SPAWN_RATE;

    while (foodSpawnIndex >= 1 && foodList->count < SIM_FOOD_MAX)
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
    vision->nFoodDist = SHEEP_VIEW_DISTANCE;

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

            if (distance < vision->nFoodDist && fabs(angleDif) < SHEEP_VIEW_ANGLE)
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
    if (sheep->age < SHEEP_EGG_MIN_AGE) {return;}

    if (!sheep->lookingForMate) 
    {
        if (floor((double) rand() / RAND_MAX * SHEEP_EGG_CHANCE) == 0)
            sheep->lookingForMate = 1;
    } 
    else if (sheep->pregnantPeriod == -1)
    {
        // Looking for a mate
        struct Sheep *closestPartner = NULL;
        double distance = SHEEP_MATE_DISTANCE;

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
        if (sheep->pregnantPeriod >= SHEEP_PREGNANT_PERIOD && sheep->hunger > SHEEP_PREGNANT_HUNGER_COST)
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
    if (sheep->age >= SHEEP_MAX_LIFE_SPAN)
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
    sheep->a = atanRange(sheep->a + TurnSpeed * SHEEP_MAX_TURN_SPEED);
    sheep->x = clamp(sheep->x + cos(sheep->a) * MoveSpeed * SHEEP_MAX_SPEED, 0, SIM_MAP_SIZE);
    sheep->y = clamp(sheep->y + sin(sheep->a) * MoveSpeed * SHEEP_MAX_SPEED, 0, SIM_MAP_SIZE);

    // Eating
    if (vision->nFoodDist < SHEEP_EATING_RANGE)
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
    sheep->hunger -= SHEEP_STARVE_RATE;

    if (sheep->hunger <= 0)
        kill_sheep(sheepNode);
}

void move_cursor(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}

void display_tick()
{
    // move_cursor(0, 0);
    char display[DISPLAY_BUFFER_LENGTH+1];

    // Space Fill
    int k = 0;
    for (int i = 0; i < DISPLAY_SIZE; i++)
    {
        for (int j = 0; j < DISPLAY_SIZE; j++)
        {
            display[k++] = ' ';
        }
        display[k++] = '\n';
    }
    display[k] = '\0';

    double displayScale = (double) DISPLAY_SIZE / SIM_MAP_SIZE;

    // Displaying Grass
    struct LinkedListNode *foodLHead = foodList->tail;
    while (foodLHead != NULL)
    {
        struct Food *food = ( struct Food *)(foodLHead->obj);

        #define GRASS_BUFFER_X ((int) (food->x*displayScale))
        #define GRASS_BUFFER_Y ((int) (food->y*displayScale))

        int bufferIndex = GRASS_BUFFER_X + (GRASS_BUFFER_Y * (DISPLAY_SIZE + 1));
        display[bufferIndex] = DISPLAY_GRASS_CHAR;
        foodLHead = foodLHead->next;
    }

    // Displaying Sheep
    struct LinkedListNode *sheepLHead = sheepList->tail;
    while (sheepLHead != NULL)
    {
        struct Sheep *sheep = ( struct Sheep *)(sheepLHead->obj);

        #define SHEEP_BUFFER_X ((int) clamp(sheep->x*displayScale, 0, DISPLAY_SIZE-1))
        #define SHEEP_BUFFER_Y ((int) clamp(sheep->y*displayScale, 0, DISPLAY_SIZE-1))

        int bufferIndex = SHEEP_BUFFER_X + (SHEEP_BUFFER_Y * (DISPLAY_SIZE + 1));
        display[bufferIndex] = DISPLAY_SHEEP_CHAR;
        sheepLHead = sheepLHead->next;
    }

    move_cursor(0, 0);
    // printf("\033[0;0H");
    printf((char*)&display);
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



    #ifdef DISPLAY
    display_tick();
    #endif
}

void write_to_replay(FILE *fp, int token, double value)
{
    fprintf(fp, "%d %f\n", token, value);
}

int main()
{
    printf("Setting up simulation...\n");
    clock_t setupStart = clock();
    sheepList = newList();
    foodList = newList();

    // Creating Grass Chunks
    chunks = ceil((double) SIM_MAP_SIZE / SIM_GRASS_CHUNK_SIZE);
    grassChunks = malloc(sizeof(struct LinkedList) * (chunks+2) * (chunks+2));
    struct LinkedList *grassChunksHead = grassChunks;
    for (int i = 0; i < (chunks+2) * (chunks+2); i++)
    {
        startList(grassChunksHead);
        grassChunksHead++;
    }

    // Tick Data
    struct TickData *tDataList = malloc(sizeof(struct TickData)*SIM_TICKS);
    struct TickData *tDataHead = tDataList;

    // Obj Set up
    for (int i = 0; i < SIM_STARTING_SHEEP; i++)
    {
        struct Sheep *sheep = new_sheep();
        sheep->x = random_pos();
        sheep->y = random_pos();
        AddToList(sheepList, sheep);
    }

    printf("Opening Replay\n");
    FILE *rfptr;
    rfptr = fopen("./replay.sim","w");
    write_to_replay(rfptr, R_SIM_TICKS, SIM_TICKS);

    // Running simulation
    printf("Started Simulation %fs\n", (setupStart-clock())/CLOCKS_PER_SEC);

    double lastPercent = 0;
    double lastVisionTime = 0;
    for (int i = 0; i < SIM_TICKS; i++)
    {
        // printf("tick: %d\n", i);
        tick(sheepList, foodList, tDataHead);
        tDataHead++;

        write_to_replay(rfptr, R_TICK_START, i);
        

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

        double percent = floor((double) i / SIM_TICKS * (1/SIM_PERCENT))*SIM_PERCENT;
        if (lastPercent != percent)
        {
            lastPercent = percent;
            printf("%f%%", lastPercent*100);
            printf(" Sheep Count: %d", sheepList->count);
            printf(" Grass Count: %d", foodList->count);
            printf(" Vision Time: %f +%f", sheepVisionTime, (sheepVisionTime-lastVisionTime)/SIM_TICKS*100);
            printf("\n");
            // printf("%f%% Sheep Count: %d Vision Time : %f\n", lastPercent*100, sheepList->count, sheepVisionTime);

            lastVisionTime = sheepVisionTime;
        }
    }

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

    for (int i = 0; i < SIM_TICKS; i++)
    {
        fprintf(fptr, "%d,%d\n", tDataHead->sheepCount, tDataHead->grassCount);
        tDataHead++;
    }

    fclose(fptr);

    printf("Completed writing csv");

    return 0;
}