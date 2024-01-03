#include "shared.c"

struct SimSettings *ss;

struct LinkedList *sheepList;
struct LinkedList *foodList;

int totalSheepCreated;

struct LinkedList *grassChunks;
int chunks;

const int chunkRadiusX[] = {0, 0, 0, -1, -1, -1, 1, 1, 1};
const int chunkRadiusY[] = {0, 1, -1, 0, 1, -1, 0, 1, -1};

FILE *rfp;
FILE *static_sheep_fp;
FILE *replay_ticks_fp;

double foodSpawnIndex;
double sheepVisionTime;

int current_tick;

double random_pos()
{
    return (double) rand() / RAND_MAX * ss->sim_map_size;
}

int get_chunk(double x, double y)
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

        // Chunk
        int chunk = get_chunk(newFood->x, newFood->y);
        struct LinkedList* grassChunk = grassChunks+chunk;
        newFood->chunk = grassChunk;
        newFood->chunkListNode = AddToList(grassChunk, newFood);

        foodSpawnIndex--;
    }
}

#pragma region Sheep Functions

struct Sheep* new_sheep()
{
    struct Sheep* sheep = malloc(sizeof(struct Sheep));
    sheep->start_tick = current_tick;
    sheep->hunger = .5;
    
    sheep->a =  ( (double) rand() / RAND_MAX ) * M_PI*2 - M_PI;
    sheep->pregnantPeriod = -1;
    sheep->gender = floor((double)rand()/RAND_MAX * 2);
    sheep->id = totalSheepCreated;
    totalSheepCreated++;

    write_static_sheep(static_sheep_fp, sheep);

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
    double nFoodDist;
    double nFoodAngle;
};

void sheep_vision(struct Sheep * sheep, struct SheepVision *vision)
{
    clock_t t = clock();

     // Food Vision
    vision->nFood = NULL;
    vision->nFoodDist = ss->sheep_view_distance;

    int sChunk = get_chunk(sheep->x, sheep->y);

    for (int i = 0; i < 9; i++)
    {
        int lsChunk = sChunk + chunkRadiusX[i] + chunkRadiusY[i] * (chunks+2);
        struct LinkedList * grassChunk = grassChunks + lsChunk;
        struct LinkedListNode *foodLHead = grassChunk->tail;

        while (foodLHead != NULL)
        {  
            struct Food *food = (struct Food *) (foodLHead->obj);
            
            double distance = hypot(sheep->x - food->x, sheep->y - food->y);
            double angle = atan2(food->y - sheep->y, food->x - sheep->x);
            double angleDif = compare_angles(sheep->a, angle);

            if (distance < vision->nFoodDist && fabs(angleDif) < ss->sheep_view_angle)
            {
                vision->nFood = foodLHead->obj;
                vision->nFoodDist = distance;
                vision->nFoodAngle = angleDif;
            }

            foodLHead = foodLHead->next;
        }
    }

    sheepVisionTime += ( (double) clock() - t ) / CLOCKS_PER_SEC;
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

    double TurnSpeed;
    double MoveSpeed;

    struct SheepVision vis;
    struct SheepVision *vision = &vis;
    sheep_vision(sheep, vision);

    // TEMP AI
    if (vision->nFood == NULL)
    {
        MoveSpeed = .4;
        TurnSpeed = .5;
    } else {
        TurnSpeed =  signbit(vision->nFoodAngle)?-1:1;
        MoveSpeed = 1;
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

void write_sheep_states(FILE *fp)
{
    struct LinkedListNode *sheepLHead = sheepList->tail;
    struct Sheep *sheep;
    fprintf(fp, "%d ", sheepList->count);
    while (sheepLHead != NULL)
    {   
        struct LinkedListNode *nextSheep = sheepLHead->next;
        sheep = (struct Sheep *) sheepLHead->obj;
        write_variable_sheep(fp, sheep);
        sheepLHead = nextSheep;
    }
}

void reset_simulation()
{
    current_tick = 0;
    foodSpawnIndex = 0;
    sheepVisionTime = 0;
    totalSheepCreated = 0;

    sheepList = newList();
    foodList = newList();
}

int run_simulation()
{
    struct SimSettings settings;
    ss = &settings;
    getDefaultSettings(ss);

    reset_simulation();

    printf("Setting up simulation...\n");
    clock_t setupStart = clock();

    // Creating Grass Chunks
    chunks = ceil((double) ss->sim_map_size / ss->sim_grass_chunk_size);
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

    write_sheep_states(replay_ticks_fp);

    printf("Wrote initial Sheep states\n");

    double lastPercent = 0;
    double lastVisionTime = 0;
    for (current_tick = 0; current_tick < ss->sim_ticks; current_tick++)
    {
        // printf("tick: %d\n", i);
        tick(sheepList, foodList, tDataHead);
        tDataHead++;

        write_sheep_states(replay_ticks_fp);
        

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

    // Reopen to read them back

    fclose(static_sheep_fp);
    fclose(replay_ticks_fp);

    static_sheep_fp = fopen("./static_sheep.temp", "r");
    replay_ticks_fp = fopen("./replay_ticks.temp", "r");

    // Transfering from Temp files
    rfp = fopen("replay.sim", "w");
    write_sim_settings(rfp, ss);

    fprintf(rfp, "%d ", totalSheepCreated);

    int ch;

    // Copying Static Sheep
    while ((ch = fgetc(static_sheep_fp)) != EOF) {
        fputc(ch, rfp);
    }

    // Copying Replay Ticks 
    while ((ch = fgetc(replay_ticks_fp)) != EOF) {
        fputc(ch, rfp);
    }

    fclose(static_sheep_fp);
    fclose(replay_ticks_fp);

    fclose(rfp);

    printf("Completed Simulation\n");

    return 0;
}