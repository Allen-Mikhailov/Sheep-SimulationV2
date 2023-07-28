#include <stdio.h> 

#define _USE_MATH_DEFINES
#include <math.h>

#include "LinkedLists.c"

#pragma region Sheep constants

#define SHEEP_STARVE_RATE 0.005
#define SHEEP_THIRST_RATE 0.005

#define SHEEP_MAX_SPEED 1
#define SHEEP_MAX_TURN_SPEED .05

#define SHEEP_VIEW_DISTANCE 100
#define SHEEP_VIEW_ANGLE M_1_PI / 4

#define SHEEP_MAX_LIFE_SPAN 3000

#define SHEEP_EGG_MIN_AGE 100
#define SHEEP_EGG_CHANCE 100

#define SHEEP_MATE_DISTANCE 100
#define SHEEP_PREGNANT_PERIOD 100
#define SHEEP_PREGNANT_HUNGER_COST .5

#pragma endregion

#pragma region Simulation Constants

#define SIM_STARTING_SHEEP 10
#define SIM_TICKS 10

#define SIM_FOOD_SPAWN_RATE 1
#define SIM_FOOD_MAX 1

#define SIM_MAP_SIZE 1000

#pragma endregion

#pragma region Simulation Structs

struct Sheep
{
    int age;
    int gender;

    double x;
    double y;

    double hunger;
};

struct Food
{
    double x;
    double y;
    double value;
};

#pragma endregion

struct LinkedList *sheepList;
struct LinkedList *foodList;

double foodSpawnIndex = 0;

double random_pos()
{
    return (double) rand() / RAND_MAX * SIM_MAP_SIZE;
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

struct Sheep* new_sheep()
{
    struct Sheep* sheep = malloc(sizeof(struct Sheep));
    sheep->age = 0;
    sheep->hunger = .5;

    return sheep;
}

void kill_sheep(struct LinkedListNode * sheep)
{
    RemoveFromList(sheepList, sheep);
}

void tick(struct LinkedList *sheepList, struct LinkedList *foodList )
{
    struct LinkedListNode *sheepLHead = sheepList->tail;
    while (sheepLHead->next != NULL)
    {   
        struct Sheep *sheep = (struct Sheep*) (sheepLHead->obj);
        sheep->age++;
        sheep->hunger -= SHEEP_STARVE_RATE;

        if (sheep->hunger <= 0)
        {
            // Dead Sheep
            kill_sheep(sheepLHead);
            // printf("Sheep Dead");
        }

        sheepLHead = sheepLHead->next;
    }

    spawn_food();
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

    return 0;
}