// Replay Tokens
#define R_TICK_START 0
#define R_SIM_TICKS 1

#define R_SHEEP_COUNT 6
#define R_SHEEP_START 2
#define R_SHEEP_X 3
#define R_SHEEP_Y 4
#define R_SHEEP_ROT 5

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