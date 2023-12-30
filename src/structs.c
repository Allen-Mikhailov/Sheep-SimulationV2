struct Sheep
{
    int age;
    int gender;

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