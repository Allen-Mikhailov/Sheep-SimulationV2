#ifndef SIM_STRUCTS

#define _USE_MATH_DEFINES
#include <math.h>

struct Sheep
{
    int age;
    int gender;

    int id;

    double x;
    double y;
    double a;

    double hunger;

    int lookingForMate;
    struct Sheep *mate;
    int pregnantPeriod;
};

struct Food
{
    double x;
    double y;
    double value;

    struct LinkedList *chunk;
    struct LinkedListNode * mainListNode;
    struct LinkedListNode * chunkListNode;
};

struct TickData
{
    int sheepCount;
    int grassCount;
};

struct SimSettings {
    // Sheep Constants
    double sheep_starve_rate;
    double sheep_thirst_rate;

    double sheep_eating_range;

    double sheep_max_speed;
    double sheep_max_turn_speed;

    double sheep_view_distance;
    double sheep_view_angle;

    double sheep_max_lifespan;

    double sheep_egg_min_age;
    double sheep_egg_chance;

    double sheep_mate_distance;
    double sheep_pregnant_period;
    double sheep_pregnant_hunger_cost;

    // Simulation Constants
    int sim_starting_sheep;
    int sim_ticks;

    double sim_food_spawn_rate;
    int sim_food_max;

    double sim_grass_chunk_size;

    double sim_map_size;
};

void getDefaultSettings(struct SimSettings *ss)
{

    ss->sheep_starve_rate = .005;
    ss->sheep_thirst_rate = 0;

    ss->sheep_eating_range = 5;

    ss->sheep_max_speed = 1;
    ss->sheep_max_turn_speed = .05;

    ss->sheep_view_distance = 100;
    ss->sheep_view_angle = (M_1_PI / 4);

    ss->sheep_max_lifespan = 1000,

    ss->sheep_egg_min_age = 100;
    ss->sheep_egg_chance = 100;
    
    ss->sheep_mate_distance = 100;
    ss->sheep_pregnant_period = 100;
    ss->sheep_pregnant_hunger_cost = 100;

    ss->sim_starting_sheep = 1;
    ss->sim_ticks = 0;
    ss->sim_food_spawn_rate = 100;
    ss->sim_food_max = 1000;
    ss->sim_map_size = 1000;
    ss->sim_grass_chunk_size = ss->sheep_view_distance;
}

#define SIM_STRUCTS

#endif