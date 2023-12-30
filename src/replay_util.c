#include <stdio.h> 

#define R_SIM_TICKS                  1 
#define R_SHEEP_STARVE_RATE          2
#define R_SHEEP_THIRST_RATE          3
#define R_SHEEP_EATING_RANGE         4
#define R_SHEEP_MAX_SPEED            5
#define R_SHEEP_MAX_TURN_SPEED       6
#define R_SHEEP_VIEW_DISTANCE        7
#define R_SHEEP_VIEW_ANGLE           8
#define R_SHEEP_MAX_LIFESPAN         9
#define R_SHEEP_EGG_MIN_AGE          10
#define R_SHEEP_EGG_CHANCE           11
#define R_SHEEP_PREGNANT_PERIOD      12
#define R_SHEEP_PREGNANT_HUNGER_COST 13
#define R_SHEEP_MATE_DISTANCE        14
#define R_SIM_STARTING_SHEEP         15
#define R_SIM_FOOD_SPAWN_RATE        16
#define R_SIM_FOOD_MAX               17
#define R_SIM_GRASS_CHUNK_SIZE       18
#define R_SIM_MAP_SIZE               19
#define R_SIM_START                  20

void write_token(FILE *fp, int token, double value)
{
    fprintf(fp, "%d %f ", token, value);
}

