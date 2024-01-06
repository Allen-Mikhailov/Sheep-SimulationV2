#ifndef SIM_SETTINGS_UTIL
#define SIM_SETTINGS_UTIL

#include <stdio.h>
#include "structs.c"

void getDefaultSettings(struct SimSettings *ss)
{

    ss->sheep_starve_rate = .005f;
    ss->sheep_thirst_rate = 0;

    ss->sheep_eating_range = 3;

    ss->sheep_max_speed = 1;
    ss->sheep_max_turn_speed = .15f;

    ss->sheep_view_distance = 10;
    ss->sheep_view_angle = (M_PI / 8);

    ss->sheep_max_lifespan = 10000,

    ss->sheep_egg_min_age = 100;
    ss->sheep_egg_chance = 100;
    
    ss->sheep_mate_distance = 100;
    ss->sheep_pregnant_period = 100;
    ss->sheep_pregnant_hunger_cost = 100;

    ss->sim_starting_sheep = 25;
    ss->sim_ticks = 1000;
    ss->sim_food_spawn_rate = 1;
    ss->sim_food_max = 1000;
    ss->sim_map_size = 100;
    ss->sim_grass_chunk_size = ss->sheep_view_distance;
}


#define SIM_SETTINGS_INT_COUNT 4
#define SIM_SETTINGS_FLOAT_COUNT 15  
void write_sim_settings(FILE *fp, struct SimSettings *s)
{
    int ints[SIM_SETTINGS_INT_COUNT];
    ints[0] = s->sim_ticks;
    ints[1] = s->sim_food_max;
    ints[2] = s->sim_starting_sheep;
    ints[3] = s->sheep_max_lifespan;
    fwrite(ints, sizeof(int), SIM_SETTINGS_INT_COUNT, fp);

    float floats[SIM_SETTINGS_FLOAT_COUNT];
    floats[0] = s->sheep_starve_rate;
    floats[1] = s->sheep_thirst_rate;
    floats[2] = s->sheep_eating_range;
    floats[3] = s->sheep_max_speed;
    floats[4] = s->sheep_max_turn_speed;
    floats[5] = s->sheep_view_distance;
    floats[6] = s->sheep_view_angle;
    floats[7] = s->sheep_egg_min_age;
    floats[8] = s->sheep_egg_chance;
    floats[9] = s->sheep_mate_distance;
    floats[10] = s->sheep_pregnant_period;
    floats[11] = s->sheep_pregnant_hunger_cost;
    floats[12] = s->sim_food_spawn_rate;
    floats[13] = s->sim_grass_chunk_size;
    floats[14] = s->sim_map_size;
    fwrite(floats, sizeof(floats), SIM_SETTINGS_FLOAT_COUNT, fp);
    
}

void read_sim_settings(FILE *fp, struct SimSettings *s)
{
    int ints[SIM_SETTINGS_INT_COUNT];
    fread(ints, sizeof(int), SIM_SETTINGS_INT_COUNT, fp);
    s->sim_ticks          = ints[0];
    s->sim_food_max       = ints[1];
    s->sim_starting_sheep = ints[2];
    s->sheep_max_lifespan = ints[3];

    float floats[SIM_SETTINGS_FLOAT_COUNT];
    fread(floats, sizeof(float), SIM_SETTINGS_FLOAT_COUNT, fp);
    s->sheep_starve_rate          = floats[0];
    s->sheep_thirst_rate          = floats[1];
    s->sheep_eating_range         = floats[2];
    s->sheep_max_speed            = floats[3];
    s->sheep_max_turn_speed       = floats[4];
    s->sheep_view_distance        = floats[5];
    s->sheep_view_angle           = floats[6];
    s->sheep_egg_min_age          = floats[7];
    s->sheep_egg_chance           = floats[8];
    s->sheep_mate_distance        = floats[9];
    s->sheep_pregnant_period      = floats[10];
    s->sheep_pregnant_hunger_cost = floats[11];
    s->sim_food_spawn_rate        = floats[12];
    s->sim_grass_chunk_size       = floats[13];
    s->sim_map_size               = floats[14];
}

#endif 