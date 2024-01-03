#ifndef SIM_SETTINGS_UTIL
#define SIM_SETTINGS_UTIL

#include <stdio.h>
#include "structs.c"

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

    ss->sim_starting_sheep = 100;
    ss->sim_ticks = 100;
    ss->sim_food_spawn_rate = 100;
    ss->sim_food_max = 1000;
    ss->sim_map_size = 1000;
    ss->sim_grass_chunk_size = ss->sheep_view_distance;
}


#define SIM_SETTINGS_WRITE_STRING "%g %g %g %g %g %g %g %g %g %g %g %g %g %d %d %g %d %g %g\n"
#define SIM_SETTINGS_SCAN_STRING  "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d %lf %d %lf %lf\n"
void write_sim_settings(FILE *fp, struct SimSettings *s)
{
    fprintf(fp, SIM_SETTINGS_WRITE_STRING,
        s->sheep_starve_rate,
        s->sheep_thirst_rate,
        s->sheep_eating_range,
        s->sheep_max_speed,
        s->sheep_max_turn_speed,
        s->sheep_view_distance,
        s->sheep_view_angle,
        s->sheep_max_lifespan,
        s->sheep_egg_min_age,
        s->sheep_egg_chance,
        s->sheep_mate_distance,
        s->sheep_pregnant_period,
        s->sheep_pregnant_hunger_cost,
        s->sim_starting_sheep,
        s->sim_ticks,
        s->sim_food_spawn_rate,
        s->sim_food_max,
        s->sim_grass_chunk_size,
        s->sim_map_size
    );
}

void read_sim_settings(FILE *fp, struct SimSettings *s)
{
    fscanf(fp, SIM_SETTINGS_SCAN_STRING,
        &s->sheep_starve_rate,
        &s->sheep_thirst_rate,
        &s->sheep_eating_range,
        &s->sheep_max_speed,
        &s->sheep_max_turn_speed,
        &s->sheep_view_distance,
        &s->sheep_view_angle,
        &s->sheep_max_lifespan,
        &s->sheep_egg_min_age,
        &s->sheep_egg_chance,
        &s->sheep_mate_distance,
        &s->sheep_pregnant_period,
        &s->sheep_pregnant_hunger_cost,
        &s->sim_starting_sheep,
        &s->sim_ticks,
        &s->sim_food_spawn_rate,
        &s->sim_food_max,
        &s->sim_grass_chunk_size,
        &(s->sim_map_size)
    );
}

#endif 