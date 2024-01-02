#include <stdio.h> 
#include "structs.c"

void write_token(FILE *fp, int token, double value)
{
    fprintf(fp, "%d %f ", token, value);
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

#define SHEEP_STATIC_SCAN_STRING  "%d "
#define SHEEP_STATIC_WRITE_STRING "%d "
void write_static_sheep(FILE *fp, struct Sheep * sheep)
{
    int mateId = -1;
    if (sheep->pregnantPeriod != -1)
        mateId = sheep->mate->id;

    fprintf(fp, SHEEP_STATIC_WRITE_STRING, 
        sheep->start_tick,
        sheep->gender
    );
}

void read_static_sheep(FILE *fp, struct Sheep *sheep, int *mateId)
{
    fscanf(fp, SHEEP_STATIC_SCAN_STRING,
        &(sheep->start_tick), 
        &(sheep->gender)
    );
}

#define SHEEP_VARIABLE_SCAN_STRING  "%d %lf %lf %d %lf %d %d %d "
#define SHEEP_VARIABLE_WRITE_STRING "%d %g %g %g %d %d %d %d "
void write_variable_sheep(FILE *fp, struct Sheep * sheep)
{
    int mateId = -1;
    if (sheep->pregnantPeriod != -1)
        mateId = sheep->mate->id;

    fprintf(fp, SHEEP_STATIC_WRITE_STRING, 
        sheep->id,
        sheep->x,
        sheep->y,
        sheep->a,
        sheep->hunger,
        sheep->lookingForMate,
        mateId,
        sheep->pregnantPeriod
    );
}

void read_variable_sheep(FILE *fp, struct Sheep *sheep, int *mateId)
{
    fscanf(fp, SHEEP_STATIC_SCAN_STRING, 
        &(sheep->id),
        &(sheep->x),
        &(sheep->y),
        &(sheep->a),
        &(sheep->hunger),
        &(sheep->lookingForMate),
        &mateId,
        &(sheep->pregnantPeriod)
    );
}