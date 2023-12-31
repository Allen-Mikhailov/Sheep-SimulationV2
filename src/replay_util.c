#include <stdio.h> 
#include "structs.c"

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

#define SIM_SETTINGS_SCAN_STRING "%f %f %f %f %f %f %f %f %f %f %f %f %f %d %d %f %d %f %f"
void write_sim_settings(FILE *fp, struct SimSettings *s)
{
    fprintf(fp, SIM_SETTINGS_SCAN_STRING,
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
        &s->sim_map_size
    );
}

void write_sheep(FILE *fp, struct Sheep * sheep)
{
    int mateId = -1;
    if (sheep->pregnantPeriod != -1)
        mateId = sheep->mate->id;

    fprintf(fp, "%d %f %f %f %d %d %f %d %d %d ", 
        sheep->id,
        sheep->x,
        sheep->y,
        sheep->a,
        sheep->age,
        sheep->gender,
        sheep->hunger,
        sheep->lookingForMate,
        mateId,
        sheep->pregnantPeriod
    );
}

void read_sheep(FILE *fp, struct Sheep *sheep, int *mateId)
{
    fscanf(fp, "%d %f %f %f %d %d %f %d %d %d ", 
        &sheep->id,
        &sheep->x,
        &sheep->y,
        &sheep->a,
        &sheep->age,
        &sheep->gender,
        &sheep->hunger,
        &sheep->lookingForMate,
        &mateId,
        &sheep->pregnantPeriod
    );
}