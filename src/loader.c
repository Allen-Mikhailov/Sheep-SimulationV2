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
    FILE *sim_settings;
};

char *TICK_ATLAS_FILE_NAME   = "tick.atlas";
char *TICK_STORE_FILE_NAME   = "tick.store";
char *SHEEP_ATLAS_FILE_NAME  = "sheep.atlas";
char *SHEEP_STORE_FILE_NAME  = "sheep.store";
char *SETTINGS_FILE_NAME     = "settings"; //would use .settings but vscode hides it in exploror
#define MAX_LOADER_FILE_NAME 12; // temp

#define ATLAS_DIGITS 12
#define ATLAS_WRITE_STR "%012x "

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

int getAtlasPosition(int id)
{
    return id * (ATLAS_DIGITS+1); // acounting for the space seperator 
}

int startSave(struct save_pointers *save, struct SimSettings *settings)
{
    openSave(save, "w+");

    write_sim_settings(save->sim_settings, settings);
}

void advanceTick(struct save_pointers *save)
{
    fseek(save->tick_atlas, 0, SEEK_END);
    long fileSize = getFileSize(save->tick_store);
    fprintf(save->tick_atlas, ATLAS_WRITE_STR, fileSize);
}

#define SHEEP_STATIC_SCAN_STRING  "%d %d "
#define SHEEP_STATIC_WRITE_STRING "%d %d "
int writeStaticSheep(struct save_pointers *save, struct Sheep* sheep)
{
    // Getting the character it will start to write at
    int startChar = (int) getFileSize(save->sheep_store);

    fseek(save->sheep_store, 0, SEEK_END); // Making sure it starts at the end
    
    // Writing the data
    fprintf(save->sheep_store, SHEEP_STATIC_WRITE_STRING, 
        sheep->start_tick,
        sheep->gender
    );

    // Writing the pos to the atlas
    fprintf(save->sheep_atlas, ATLAS_WRITE_STR , startChar);

    return 0;
}

int readStaticSheep(struct save_pointers *save, struct Sheep* sheep, int id)
{
    int atlas_pos = getAtlasPosition(id);
    long store_pos;

    fseek(save->sheep_atlas, atlas_pos, SEEK_SET);
    fscanf(save->sheep_atlas, "%x ", &store_pos);

    fseek(save->sheep_store, store_pos, SEEK_SET);
    fscanf(save->sheep_store, SHEEP_STATIC_SCAN_STRING,
        &(sheep->start_tick), 
        &(sheep->gender)
    );

    return 0;
}

#define SHEEP_VARIABLE_SCAN_STRING  "%d %lf %lf %lf %lf %d %d %d "
#define SHEEP_VARIABLE_WRITE_STRING "%d %g %g %g %g %d %d %d "
void writeVariableSheep(struct save_pointers *save, struct Sheep* sheep)
{
    fprintf(save->tick_store, SHEEP_VARIABLE_WRITE_STRING, 
        sheep->id,
        sheep->x,
        sheep->y,
        sheep->a,
        sheep->hunger,
        sheep->lookingForMate,
        sheep->mateId,
        sheep->pregnantPeriod
    );
}

void readVariableSheep(struct save_pointers *save, struct Sheep* sheep)
{
    fscanf(save->tick_store, SHEEP_VARIABLE_SCAN_STRING, 
        &(sheep->id),
        &(sheep->x),
        &(sheep->y),
        &(sheep->a),
        &(sheep->hunger),
        &(sheep->lookingForMate),
        &(sheep->mateId),
        &(sheep->pregnantPeriod)
    );
}