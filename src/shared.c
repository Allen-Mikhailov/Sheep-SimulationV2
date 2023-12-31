#ifndef SHARED
#define SHARED

#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>
#include <commctrl.h>
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "LinkedLists.c"

#include "math_util.c"
#include "structs.c"
#include "replay_util.c"


// Replay Frame
int replay_frame = 0;
int replay_frame_width = 400;
int replay_frame_height = 400;


#endif