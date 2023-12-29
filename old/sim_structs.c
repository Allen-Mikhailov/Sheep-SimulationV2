// Replay Tokens
#define R_SIM_START 0
#define R_SIM_TICKS 1

#define R_TICK_END 6

#define R_SHEEP_COUNT 2
#define R_FOOD_COUNT  3

#define R_MAP_X 4
#define R_MAP_Y 5

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