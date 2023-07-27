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

#define SIM_STARTING_SHEEP 2
#define SIM_TICKS 10000

#pragma endregion

int main()
{
    struct LinkedList sheepList = newList();

    for (int i = 0; i < SIM_STARTING_SHEEP; i++)
    {
        struct Sheep sheep;
        sheep.age = i;
        AddToList(&sheepList, (void*) &sheep);
        printf("plz %d \n", ((struct Sheep*) (sheepList.tail->obj))->age);
    }

    printf("plzz %d \n", sheepList.head == NULL);

    printf("Counted %d", countList(&sheepList));

    return 0;
}