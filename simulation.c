#include <stdio.h> 

#define _USE_MATH_DEFINES
#include <math.h>

#include "LinkedLists.c"

#pragma region Sheep constants

#define SHEEP_STARVE_RATE 0.005
#define SHEEP_THIRST_RATE 0.005

#define SHEEP_EATING_RANGE 5

#define SHEEP_MAX_SPEED 1
#define SHEEP_MAX_TURN_SPEED .05

#define SHEEP_VIEW_DISTANCE 100
#define SHEEP_VIEW_ANGLE (M_1_PI / 4)

#define SHEEP_MAX_LIFE_SPAN 30000

#define SHEEP_EGG_MIN_AGE 100
#define SHEEP_EGG_CHANCE 100

#define SHEEP_MATE_DISTANCE 100
#define SHEEP_PREGNANT_PERIOD 100
#define SHEEP_PREGNANT_HUNGER_COST .5

#pragma endregion

#pragma region Simulation Constants

#define SIM_STARTING_SHEEP 5
#define SIM_TICKS 1000

#define SIM_FOOD_SPAWN_RATE 10
#define SIM_FOOD_MAX 10

#define SIM_MAP_SIZE 100

#pragma endregion

#pragma region Simulation Structs

#pragma region Display Constants

#define DISPLAY

#define DISPLAY_SIZE 10
#define DISPLAY_BUFFER_LENGTH DISPLAY_SIZE*(DISPLAY_SIZE+1)

#define DISPLAY_GRASS_CHAR '-'
#define DISPLAY_SHEEP_CHAR 'O'

#pragma endregion

struct Sheep
{
    int age;
    int gender;

    double x;
    double y;
    double a;

    double hunger;
};

struct Food
{
    double x;
    double y;
    double value;
};

struct TickData
{
    int sheepCount;
    int grassCount;
};

#pragma endregion

struct LinkedList *sheepList;
struct LinkedList *foodList;

double foodSpawnIndex = 0;

double random_pos()
{
    return (double) rand() / RAND_MAX * SIM_MAP_SIZE;
}

double atanRange(double a)
{
    a = fmod(a, M_PI_2);
    if (a > M_PI)
        return M_PI_2 - a;
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
        dif += M_PI_2;
    if(dif > M_PI)
        dif -= M_PI_2;
    return dif;
}

void spawn_food()
{
    foodSpawnIndex += SIM_FOOD_SPAWN_RATE;

    while (foodSpawnIndex >= 1 && foodList->count < SIM_FOOD_MAX)
    {
        struct Food * newFood = malloc(sizeof(struct Food));
        newFood->x = random_pos();
        newFood->y = random_pos();
        AddToList(foodList, newFood);

        foodSpawnIndex--;
    }
}

#pragma region Sheep Functions

struct Sheep* new_sheep()
{
    struct Sheep* sheep = malloc(sizeof(struct Sheep));
    sheep->age = 0;
    sheep->hunger = .5;
    sheep->a = (double) rand()/RAND_MAX * M_PI_2 - M_1_PI;

    return sheep;
}

void kill_sheep(struct LinkedListNode * sheep)
{
    // RemoveFromList(sheepList, sheep);
    // free(sheep->obj);
    // free(sheep);
}

struct SheepVision
{
    struct LinkedListNode *nFood;
    double nFoodDist;
    double nFoodAngle;
};

void sheep_vision(struct Sheep * sheep, struct SheepVision *vision)
{
     // Food Vision
    vision->nFood = NULL;
    vision->nFoodDist = SHEEP_VIEW_DISTANCE;
    struct LinkedListNode *foodLHead = foodList->tail;

    // if (foodLHead == NULL)
    //     return;

    while (foodLHead != NULL)
    {  
        struct Food *food = (struct Food *) (foodLHead->obj);
        
        double distance = hypot(sheep->x - food->x, sheep->y - food->y);
        double angle = atan2(food->y - sheep->y, food->x - sheep->x);
        double angleDif = compare_angles(sheep->a, angle);

        if (distance < vision->nFoodDist && fabs(angleDif) < SHEEP_VIEW_ANGLE)
        {
            vision->nFood = foodLHead;
            vision->nFoodDist = distance;
            vision->nFoodAngle = angleDif;
        }

        foodLHead = foodLHead->next;
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

    sheep->a = atanRange(sheep->a + TurnSpeed);
    sheep->x = clamp(sheep->x + cos(sheep->a) * MoveSpeed, 0, SIM_MAP_SIZE);
    sheep->y = clamp(sheep->y + sin(sheep->a) * MoveSpeed, 0, SIM_MAP_SIZE);

    // Eating
    if (vision->nFoodDist < SHEEP_EATING_RANGE)
    {
        sheep->hunger = 1;
        RemoveFromList(foodList, vision->nFood);
        printf("Sheep ate food");
    }

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
    move_cursor(0, 0);
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

        #define SHEEP_BUFFER_X ((int) (sheep->x*displayScale))
        #define SHEEP_BUFFER_Y ((int) (sheep->y*displayScale))

        int bufferIndex = SHEEP_BUFFER_X + (SHEEP_BUFFER_Y * (DISPLAY_SIZE + 1));
        display[bufferIndex] = DISPLAY_SHEEP_CHAR;
        sheepLHead = sheepLHead->next;
    }

    printf((char*)&display);
}

#pragma endregion 

void tick(struct LinkedList *sheepList, struct LinkedList *foodList )
{
    struct LinkedListNode *sheepLHead = sheepList->tail;
    while (sheepLHead != NULL)
    {   
        struct LinkedListNode *nextSheep = sheepLHead->next;
        sheep_tick(sheepLHead);
        sheepLHead = nextSheep;
    }

    spawn_food();

    #ifdef DISPLAY
    display_tick();
    #endif
}

int main()
{
    sheepList = newList();
    foodList = newList();

    for (int i = 0; i < SIM_STARTING_SHEEP; i++)
    {
        struct Sheep *sheep = new_sheep();
        sheep->x = random_pos();
        sheep->y = random_pos();
        AddToList(sheepList, (void*) sheep);
    }

    for (int i = 0; i < SIM_TICKS; i++)
    {
        // printf("tick: %d\n", i);
        tick(sheepList, foodList);
    }

    printf("Completed Simulation\n");

    return 0;
}