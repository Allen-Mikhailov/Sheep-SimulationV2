#include "shared.c"

struct SimSettings *ss;

struct LinkedList *sheepList;
struct LinkedList *foodList;

int totalSheepCreated;
int totalFoodCreated;

struct LinkedList *grassChunks;
int chunks;

const int chunkRadiusX[] = {0, 0, 0, -1, -1, -1, 1, 1, 1};
const int chunkRadiusY[] = {0, 1, -1, 0, 1, -1, 0, 1, -1};

float foodSpawnIndex;
float sheepVisionTime;

int current_tick;

struct save_pointers *sim_save;

float random_pos()
{
    return random() * ss->sim_map_size;
}

int get_chunk(float x, float y)
{
    int cx = clamp(floor(x/ss->sim_grass_chunk_size) + 1, 1, chunks);
    int cy = clamp(floor(y/ss->sim_grass_chunk_size) + 1, 1, chunks);

    return cx + cy * (chunks+2);
}

void spawn_food()
{
    foodSpawnIndex += ss->sim_food_spawn_rate;

    while (foodSpawnIndex >= 1 && foodList->count < ss->sim_food_max)
    {
        struct Food * newFood = malloc(sizeof(struct Food));
        newFood->x = random_pos();
        newFood->y = random_pos();
        newFood->mainListNode = AddToList(foodList, newFood);

        newFood->id = totalFoodCreated;
        totalFoodCreated++;

        // Chunk
        int chunk = get_chunk(newFood->x, newFood->y);
        struct LinkedList* grassChunk = grassChunks+chunk;
        newFood->chunk = grassChunk;
        newFood->chunkListNode = AddToList(grassChunk, newFood);

        #ifdef STORE_FOOD
        writeStaticFood(sim_save, newFood);
        #endif

        foodSpawnIndex--;
    }
}

#pragma region Sheep Functions

struct Sheep* new_sheep()
{
    struct Sheep* sheep = malloc(sizeof(struct Sheep));
    sheep->start_tick = current_tick;
    sheep->hunger = .5;

    sheep->visible_food_id = -1;
    
    sheep->a =  ( (double) rand() / RAND_MAX ) * M_PI*2 - M_PI;
    sheep->pregnantPeriod = -1;
    sheep->gender = floor((double)rand()/RAND_MAX * 2);
    sheep->id = totalSheepCreated;
    totalSheepCreated++;

    #ifdef STORE_SHEEP
    writeStaticSheep(sim_save, sheep);
    #endif

    return sheep;
}

void kill_sheep(struct LinkedListNode * sheep)
{
    RemoveFromList(sheepList, sheep);
    free(sheep->obj);
    free(sheep);
}

struct SheepVision
{
    struct Food *nFood;
    float nFoodDist;
    float nFoodAngle;
};

void sheep_vision(struct Sheep * sheep, struct SheepVision *vision)
{
    clock_t t = clock();

     // Food Vision
    vision->nFood = NULL;
    vision->nFoodDist = ss->sheep_view_distance;

    const int sChunk = get_chunk(sheep->x, sheep->y);

    for (int i = 0; i < 9; i++)
    {
        const int lsChunk = sChunk + chunkRadiusX[i] + chunkRadiusY[i] * (chunks+2);
        const struct LinkedList * grassChunk = grassChunks + lsChunk;
        struct LinkedListNode *foodLHead = grassChunk->tail;

        while (foodLHead != NULL)
        {  
            const struct Food *food = (struct Food *) (foodLHead->obj);
            
            const float distance = hypot(sheep->x - food->x, sheep->y - food->y);
            const float angle = atan2(food->y - sheep->y, food->x - sheep->x);
            const float angleDif = COMPARE_ANGLES(sheep->a, angle);

            if (distance < vision->nFoodDist && fabs(angleDif) < ss->sheep_view_angle)
            {
                vision->nFood = foodLHead->obj;
                vision->nFoodDist = distance;
                vision->nFoodAngle = angleDif;
            }

            foodLHead = foodLHead->next;
        }
    }

    sheepVisionTime += ( (float) clock() - t ) / CLOCKS_PER_SEC;
}

void Birth(struct Sheep * s1)
{
    struct Sheep *newSheep = new_sheep();
    newSheep->x = s1->x;
    newSheep->y = s1->y;

    AddToList(sheepList, newSheep);
}

void female_sheep_tick(struct LinkedListNode * sheepNode)
{
    clock_t start = clock();
    struct Sheep *sheep = (struct Sheep*) (sheepNode->obj);
    if (current_tick-sheep->start_tick < ss->sheep_egg_min_age) {return;}

    if (!sheep->lookingForMate) 
    {
        if (floor(random() * ss->sheep_egg_chance) == 0)
            sheep->lookingForMate = 1;
    } 
    else if (sheep->pregnantPeriod == -1)
    {
        // Looking for a mate
        struct Sheep *closestPartner = NULL;
        double distance = ss->sheep_mate_distance;

        struct LinkedListNode *sheepLHead = sheepList->tail;
        while (sheepLHead != NULL)
        {
            struct Sheep *s2 = (struct Sheep *) (sheepLHead->obj);
            double dist = hypot(sheep->x-s2->x, sheep->y-s2->y);
            if (s2->gender == 0 && dist < distance )
            {
                distance = dist;
                closestPartner = s2;
            }
            sheepLHead = sheepLHead->next;
        }

        if (closestPartner != NULL)
        {
            sheep->mateId = closestPartner->id;
            sheep->pregnantPeriod = 0;
        }
    } else {
        // printf("Pregnant");
        sheep->pregnantPeriod++;
        if (sheep->pregnantPeriod >= ss->sheep_pregnant_period 
            && sheep->hunger > ss->sheep_pregnant_hunger_cost)
        {
            Birth(sheep);
            sheep->pregnantPeriod = -1;
            sheep->lookingForMate = 1;
        }
    }
}

void sheep_tick(struct LinkedListNode * sheepNode)
{
    struct Sheep *sheep = (struct Sheep*) (sheepNode->obj);
    if (current_tick-sheep->start_tick >= ss->sheep_max_lifespan)
        kill_sheep(sheepNode);

    float TurnSpeed;
    float MoveSpeed;

    struct SheepVision vis;
    struct SheepVision *vision = &vis;
    sheep_vision(sheep, vision);

    // TEMP AI
    if (vision->nFood == NULL)
    {
        MoveSpeed = .4;
        TurnSpeed = .5;

        sheep->visible_food_id = -1;
    } else {
        int turn_dir = (0 < vision->nFoodAngle) - (vision->nFoodAngle < 0);
        // Makes it so the sheep doesnt rotate past its food
        TurnSpeed = min(fabs(vision->nFoodAngle), ss->sheep_max_turn_speed) * turn_dir / ss->sheep_max_turn_speed;
        MoveSpeed = 1;

        sheep->visible_food_id = vision->nFood->id;
    }

    // Movement
    sheep->a = atanRange(sheep->a + TurnSpeed * ss->sheep_max_turn_speed);
    sheep->x = clamp(sheep->x + cos(sheep->a) * MoveSpeed * ss->sheep_max_speed, 0, ss->sim_map_size);
    sheep->y = clamp(sheep->y + sin(sheep->a) * MoveSpeed * ss->sheep_max_speed, 0, ss->sim_map_size);

    // Eating
    if (vision->nFoodDist < ss->sheep_eating_range)
    {
        sheep->hunger = 1;
        RemoveFromList(foodList, vision->nFood->mainListNode);
        RemoveFromList(vision->nFood->chunk, vision->nFood->chunkListNode);
        // RemoveFromList(vision->nFood)
        // printf("Sheep ate food");
    }

    if (sheep->gender == 1)
        female_sheep_tick(sheepNode);

    // Hunger
    sheep->hunger -= ss->sheep_starve_rate;

    if (sheep->hunger <= 0)
        kill_sheep(sheepNode);
}


#pragma endregion 

void tick(struct LinkedList *sheepList, struct LinkedList *foodList, struct TickData * tData )
{
    struct LinkedListNode *sheepLHead = sheepList->tail;
    while (sheepLHead != NULL)
    {   
        struct LinkedListNode *nextSheep = sheepLHead->next;
        sheep_tick(sheepLHead);
        sheepLHead = nextSheep;
    }

    spawn_food();   

    tData->sheepCount = sheepList->count;
    tData->grassCount = foodList->count;
}

void writeSheepStates()
{
    #ifdef STORE_SHEEP
    struct LinkedListNode *sheepLHead = sheepList->tail;
    struct Sheep *sheep;
    fwrite(&sheepList->count, sizeof(int), 1, sim_save->tick_store);
    while (sheepLHead != NULL)
    {   
        struct LinkedListNode *nextSheep = sheepLHead->next;
        sheep = (struct Sheep *) sheepLHead->obj;
        writeVariableSheep(sim_save, sheep);
        sheepLHead = nextSheep;
    }
    #endif
}

void writeFoodStates()
{
    #ifdef STORE_FOOD
    struct LinkedListNode *foodLHead = foodList->tail;
    struct Food *food;
    fwrite(&foodList->count, sizeof(int), 1, sim_save->tick_store);
    while (foodLHead != NULL)
    {   
        struct LinkedListNode *nextFood = foodLHead->next;
        food = (struct Food *) foodLHead->obj;
        writeVariableFood(sim_save, food);
        foodLHead = nextFood;
    }
    #endif
}

void reset_simulation()
{
    current_tick = 0;
    foodSpawnIndex = 0;
    sheepVisionTime = 0;
    totalSheepCreated = 0;
    totalFoodCreated = 0;

    sheepList = newList();
    foodList = newList();
}

int run_simulation()
{
    struct SimSettings settings;
    ss = &settings;
    getDefaultSettings(ss);

    reset_simulation();

    struct save_pointers new_save;
    sim_save = &new_save;
    sim_save->path = "./replay";
    startSave(sim_save, &settings);

    printf("Setting up simulation...\n");
    clock_t setupStart = clock();

    // Creating Grass Chunks
    chunks = ceil(ss->sim_map_size / ss->sim_grass_chunk_size);
    grassChunks = malloc(sizeof(struct LinkedList) * (chunks+2) * (chunks+2));
    struct LinkedList *grassChunksHead = grassChunks;
    for (int i = 0; i < (chunks+2) * (chunks+2); i++)
    {
        startList(grassChunksHead);
        grassChunksHead++;
    }

    // Tick Data
    struct TickData *tDataList = malloc(sizeof(struct TickData)*ss->sim_ticks);
    struct TickData *tDataHead = tDataList;

    printf("Wrote replay pointers\n");

    // Obj Set up
    for (int i = 0; i < ss->sim_starting_sheep; i++)
    {
        struct Sheep *sheep = new_sheep();
        sheep->x = random_pos();
        sheep->y = random_pos();
        AddToList(sheepList, sheep);
    }

    printf("Setup inital Sheep\n");

    // Running simulation
    printf("Started Simulation %fs\n", (setupStart-clock())/CLOCKS_PER_SEC);

    advanceTick(sim_save);

    writeSheepStates();
    writeFoodStates();

    printf("Wrote initial Sheep states\n");

    double lastPercent = 0;
    double lastVisionTime = 0;
    for (current_tick = 0; current_tick < ss->sim_ticks; current_tick++)
    {
        // printf("tick: %d\n", i);
        tick(sheepList, foodList, tDataHead);
        tDataHead++;

        advanceTick(sim_save);
        writeSheepStates();
        writeFoodStates();
        

        double percent = floor((double) current_tick / ss->sim_ticks * (100))/100.0;
        if (lastPercent != percent)
        {
            lastPercent = percent;
            printf("%f%%", lastPercent*100);
            printf(" Sheep Count: %d", sheepList->count);
            printf(" Grass Count: %d", foodList->count);
            printf(" Vision Time: %f +%f", sheepVisionTime, 
                (sheepVisionTime-lastVisionTime)/ss->sim_ticks*100);
            printf("\n");
            // printf("%f%% Sheep Count: %d Vision Time : %f\n", lastPercent*100, sheepList->count, sheepVisionTime);

            lastVisionTime = sheepVisionTime;
        }
    }

    printf("Simulation Concluded\n");
    printf("Writing replay files\n");

    closeSave(sim_save);

    printf("Completed Simulation\n");

    return 0;
}