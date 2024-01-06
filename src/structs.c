#ifndef SIM_STRUCTS

#define _USE_MATH_DEFINES
#include <math.h>

struct Sheep {
    int start_tick;
    int gender;

    int id;

    float x;
    float y;
    float a; // Angle

    float hunger;

    // For debugging
    int visible_food_id;

    // Might make them asexual
    int lookingForMate;
    int mateId;
    int pregnantPeriod;
};

struct Food
{
    int id;
    float x;
    float y;
    float value;

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
    float sheep_starve_rate;
    float sheep_thirst_rate;

    float sheep_eating_range;

    float sheep_max_speed;
    float sheep_max_turn_speed;

    float sheep_view_distance;
    float sheep_view_angle;

    int sheep_max_lifespan;

    float sheep_egg_min_age;
    float sheep_egg_chance;

    float sheep_mate_distance;
    float sheep_pregnant_period;
    float sheep_pregnant_hunger_cost;

    // Simulation Constants
    int sim_starting_sheep;
    int sim_ticks;

    float sim_food_spawn_rate;
    int sim_food_max;

    float sim_grass_chunk_size;

    float sim_map_size;
};

#define SIM_STRUCTS

#endif