#include <windows.h>
#include <stdio.h> 

#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "LinkedLists.c"
#include "sim_structs.c"

FILE *rfp;
int current_tick = 0;

int map_x;
int map_y;

struct Tick {
    struct Sheep *sheep;
    int sheep_count;

    struct Food *food;
    int food_count;
};

struct Tick *ticks;

int tick_count = 0;

void ReadReplay()
{
    // First Variables
    int token;
    double value;
    while (fscanf(rfp, "%d, %f") != NULL)
    {
        switch (token)
        {
            case R_MAP_X:
                map_x = (int) value;
                break;
            case R_MAP_Y:
                map_y = (int) value;
                break;
            case R_SIM_TICKS:
                tick_count = (int) value;
                break;
        }
    }

    int sim_ticks;
    fscanf(rfp, "%d", &sim_ticks);
    printf("SIM_TICKS: %d", sim_ticks);

    
}

void PaintTick()
{

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    rfp = fopen("replay.sim", "r");
    if (rfp == NULL) {
        printf("Not able to open the replay file ");
        return 1;
    }

    // Register the window class
    const char CLASS_NAME[] = "WindowClass";

    WNDCLASS wc = {0};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,                          // Optional window styles
        CLASS_NAME,                 // Window class
        "Sheep Simulation Replay",            // Window title
        WS_OVERLAPPEDWINDOW,        // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Window procedure callback function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}