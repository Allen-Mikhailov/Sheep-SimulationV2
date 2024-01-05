#ifndef SHARED
#define SHARED

#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>
#include <commctrl.h>
#include <time.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "LinkedLists.c"

#include "math_util.c"
#include "structs.c"
#include "sim_settings_util.c"
#include "sprite_util.c"
#include "loader.c"

#define STORE_SHEEP
#define STORE_FOOD

void pop_path(char* destination, char* path)
{
    // Finding the last slash
    int index = strrchr(path, '\\') - path;
    strncpy(destination, path, index);
}

// Replay Frame
int replay_frame = 0;
int replay_frame_width = 400;
int replay_frame_height = 400;


#endif