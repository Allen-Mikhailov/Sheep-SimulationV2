#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include "simulation.c"

#define MENU_SIM_START 1 
#define MENU_REPLAY_START 2

HWND replay_window;
HWND simulation_window;
HWND simulation_settings_window;

// Function prototypes
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const char szClassName[] = "SimWindowClass";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = szClassName;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,
        szClassName,
        "Sheep Simulation",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the menu bar
    HMENU hMenu = CreateMenu();
    HMENU hSimMenu = CreatePopupMenu();
    HMENU hReplayMenu = CreatePopupMenu();

    // Add menu items to the submenu
    AppendMenu(hSimMenu, MF_STRING, MENU_SIM_START, "Start Simulation");
    AppendMenu(hSimMenu, MF_STRING, 2, "Menu Item 2");

    AppendMenu(hReplayMenu, MF_STRING, MENU_REPLAY_START, "Start Replay");

    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSimMenu,    "Simulation");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hReplayMenu, "Replay");

    // Set the menu for the window
    SetMenu(hwnd, hMenu);

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                // Handle menu item commands here
                case MENU_SIM_START:
                    // Code for handling Menu Item 1
                    printf("Simulation Start");
                    run_simulation();
                    break;
                case MENU_REPLAY_START:
                    // Code for handling Menu Item 2
                    break;
                // Add more cases as needed
            }
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    return 0;
}
