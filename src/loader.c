#include <stdio.h> 
#include <stdlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <direct.h>

#define DEF2STR(x) #x

#include "structs.c"
#include "sim_settings_util.c"

#define MAX_PATH_LENGTH 32767 //Probably overkill size

struct save_pointers {
    const char* path; //expects

    FILE *tick_atlas;
    FILE *tick_store;

    FILE *sheep_atlas;
    FILE *sheep_store;

    FILE *food_atlas;
    FILE* food_store;

    FILE *sim_settings;
};

char *TICK_ATLAS_FILE_NAME   = "tick.atlas";
char *TICK_STORE_FILE_NAME   = "tick.store";
char *SHEEP_ATLAS_FILE_NAME  = "sheep.atlas";
char *SHEEP_STORE_FILE_NAME  = "sheep.store";
char *FOOD_ATLAS_FILE_NAME   = "food.atlas";
char *FOOD_STORE_FILE_NAME   = "food.store";
char *SETTINGS_FILE_NAME     = "settings"; //would use .settings but vscode hides it in exploror
#define MAX_LOADER_FILE_NAME 12; // temp

int dirExists(const char *folderPath)
{
    struct stat sb;
    return stat(folderPath, &sb) == 0 && S_ISDIR(sb.st_mode);
}

long getFileSize(FILE *fp)
{
    long currentPosition = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, currentPosition, SEEK_SET);
    return size;
}

int openSave(struct save_pointers *save, const char * mode)
{

    // Making Sure the directory actually exists
    if (!dirExists(save->path))
    {
        if (_mkdir(save->path) == -1)
        {
            printf("Error: Creating Directory");
            return 1;
        }
    }

    char file_path[MAX_PATH_LENGTH];

    // Tick Atlas
    snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", save->path, TICK_ATLAS_FILE_NAME);
    save->tick_atlas = fopen(file_path, mode);

    // Tick Store
    snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", save->path, TICK_STORE_FILE_NAME);
    save->tick_store = fopen(file_path, mode);

    // Sheep Atlas
    snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", save->path, SHEEP_ATLAS_FILE_NAME);
    save->sheep_atlas = fopen(file_path, mode);

     // Sheep Store
    snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", save->path, SHEEP_STORE_FILE_NAME);
    save->sheep_store = fopen(file_path, mode);

    // Food Atlas
    snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", save->path, FOOD_ATLAS_FILE_NAME);
    save->food_atlas = fopen(file_path, mode);

    // Food Store
    snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", save->path, FOOD_STORE_FILE_NAME);
    save->food_store = fopen(file_path, mode);

    // Settings
    snprintf(file_path, MAX_PATH_LENGTH, "%s\\%s", save->path, SETTINGS_FILE_NAME);
    save->sim_settings = fopen(file_path, mode);
}

int closeSave(struct save_pointers *save)
{
    fclose(save->tick_atlas);
    fclose(save->tick_store);
    fclose(save->sheep_atlas);
    fclose(save->sheep_store);
    fclose(save->sim_settings);
}

long getAtlasPosition(long id)
{
    return id * sizeof(long); // acounting for the space seperator 
}

long readAtlasPos(FILE *atlas, long id)
{
    long atlas_pos = getAtlasPosition(id);
    fseek(atlas, atlas_pos, SEEK_SET);

    long pos;
    fscanf(atlas, "%ld ", &pos);
    return pos;
}

int startSave(struct save_pointers *save, struct SimSettings *settings)
{
    openSave(save, "wb+");

    write_sim_settings(save->sim_settings, settings);
}

void advanceTick(struct save_pointers *save)
{
    fseek(save->tick_atlas, 0, SEEK_END);
    long fileSize = getFileSize(save->tick_store);
    fwrite(&fileSize, sizeof(long), 1, save->tick_atlas);
}

#define SHEEP_STATIC_INT_COUNT 2
int writeStaticSheep(struct save_pointers *save, struct Sheep* sheep)
{
    // Getting the character it will start to write at
    long startByte = getFileSize(save->sheep_store);
    fseek(save->sheep_store, 0, SEEK_END); // Making sure it starts at the end
    
    // Writing the data
    int ints[SHEEP_STATIC_INT_COUNT];
    ints[0] = sheep->gender;
    ints[1] = sheep->start_tick;
    fwrite(ints, sizeof(int), SHEEP_STATIC_INT_COUNT, save->sheep_store);

    // Writing the pos to the atlas
    fseek(save->sheep_atlas, 0, SEEK_END);
    fwrite(&startByte, sizeof(long), 1, save->sheep_atlas);

    return 0;
}

int readStaticSheep(struct save_pointers *save, struct Sheep* sheep, long id)
{
    int atlas_pos = getAtlasPosition(id);
    long startByte;

    // Getting the starting byte in the store
    fseek(save->sheep_atlas, atlas_pos, SEEK_SET);
    fread(&startByte, sizeof(long), 1, save->sheep_atlas);

    // Reading the data
    int ints[SHEEP_STATIC_INT_COUNT];
    fread(ints, sizeof(int), SHEEP_STATIC_INT_COUNT, save->sheep_store);
    sheep->gender     = ints[0];
    sheep->start_tick = ints[1];

    return 0;
}

#define SHEEP_VARIABLE_INT_COUNT 4
#define SHEEP_VARIABLE_FLOAT_COUNT 4
void writeVariableSheep(struct save_pointers *save, struct Sheep* sheep)
{
    int ints[SHEEP_VARIABLE_INT_COUNT];
    ints[0] = sheep->id;
    ints[1] = sheep->lookingForMate;
    ints[2] = sheep->mateId;
    ints[3] = sheep->pregnantPeriod;
    fwrite(ints, sizeof(int), SHEEP_VARIABLE_INT_COUNT, save->tick_store);

    float floats[SHEEP_VARIABLE_FLOAT_COUNT];
    floats[0] = sheep->x;
    floats[1] = sheep->y;
    floats[2] = sheep->a;
    floats[3] = sheep->hunger;
    fwrite(floats, sizeof(float), SHEEP_VARIABLE_FLOAT_COUNT, save->tick_store);
}

void readVariableSheep(struct save_pointers *save, struct Sheep* sheep)
{
    int ints[SHEEP_VARIABLE_INT_COUNT];
    fread(ints, sizeof(int), SHEEP_VARIABLE_INT_COUNT, save->tick_store);
    sheep->id             = ints[0];
    sheep->lookingForMate = ints[1];
    sheep->mateId         = ints[2];
    sheep->pregnantPeriod = ints[3];


    float floats[SHEEP_VARIABLE_FLOAT_COUNT];
    fread(floats, sizeof(float), SHEEP_VARIABLE_FLOAT_COUNT, save->tick_store);
    sheep->x      = floats[0];
    sheep->y      = floats[1];
    sheep->a      = floats[2];
    sheep->hunger = floats[3];
} 

#define FOOD_STATIC_FLOAT_COUNT 3
void writeStaticFood(struct save_pointers *save, struct Food* food)
{
    // Getting the character it will start to write at
    long startByte = getFileSize(save->food_store);
    fseek(save->food_store, 0, SEEK_END); // Making sure it starts at the end

    // Writing the data
    float floats[FOOD_STATIC_FLOAT_COUNT];
    floats[0] = food->x;
    floats[1] = food->y;
    floats[2] = food->value;
    fwrite(floats, sizeof(float), FOOD_STATIC_FLOAT_COUNT, save->food_store);

    // Writing the pos to the atlas
    fseek(save->food_atlas, 0, SEEK_END);
    fwrite(&startByte, sizeof(long), 1, save->food_atlas);
}

void readStaticFood(struct save_pointers *save, struct Food* food, long id)
{
    int atlas_pos = getAtlasPosition(id);
    long startByte;

    // Getting the starting byte in the store
    fseek(save->food_atlas, atlas_pos, SEEK_SET);
    fread(&startByte, sizeof(long), 1, save->food_atlas);

    fseek(save->food_store, startByte, SEEK_SET);

    // Reading the data
    float floats[FOOD_STATIC_FLOAT_COUNT];
    fread(floats, sizeof(float), FOOD_STATIC_FLOAT_COUNT, save->food_store);
    food->x     = floats[0];
    food->y     = floats[1];
    food->value = floats[2];
}

#define FOOD_VARIABLE_INT 1
void writeVariableFood(struct save_pointers *save, struct Food* food)
{
    int ints[FOOD_VARIABLE_INT];
    ints[0] = food->id;
    fwrite(ints, sizeof(int), FOOD_VARIABLE_INT, save->tick_store);
} 

void readVariableFood(struct save_pointers *save, struct Food* food)
{
    int ints[FOOD_VARIABLE_INT];
    fread(ints, sizeof(int), FOOD_VARIABLE_INT, save->tick_store);
    food->id = ints[0];
} 