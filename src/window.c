#include "shared.c"
#include "simulation.c"
#include "replay.c"

#define MENU_SIM_START 1 
#define MENU_REPLAY_START 2
#define MENU_REPLAY_NEXT_FRAME 3
#define MENU_REPLAY_NEXT_10_FRAME 4
#define MENU_REPLAY_PREVIOUS_FRAME 5
#define MENU_REPLAY_PREVIOUS_10_FRAME 6

struct save_pointers replay_save;

void RequestReplayDraw(HWND hwnd)
{
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    int width = clientRect.right-clientRect.left;
    int height = clientRect.bottom-clientRect.top;

    int size = min(height, width) * .7;
    int horizontalPadding = (width - size) / 2;
    int vertialPadding = (height - size) / 2;

    HDC hdc = GetDC(hwnd);

    HDC replay_hdc = CreateCompatibleDC(hdc);
    HBITMAP bitmap = createCompatibleBitmap(hdc, size, size);
    SelectObject(replay_hdc, bitmap);

    LoadFrame(&replay_save);
    DrawReplay(replay_hdc, size);

    // Drawing Replay
    BitBlt(hdc, horizontalPadding, vertialPadding, size, size, replay_hdc, 0, 0, SRCCOPY);

    HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
    SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 0, 255));

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "FRAME: % 6d", replay_frame);

    TextOut(hdc, horizontalPadding, vertialPadding-20, buffer, 14);

    DeleteObject(hFont);

    ReleaseDC(hwnd, replay_hdc);
}

HMENU CreateMenuOptions()
{
    // Create the menu bar
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hSimulationMenu = CreatePopupMenu();
    HMENU hReplayMenu = CreatePopupMenu();

    AppendMenu(hSimulationMenu, MF_STRING, MENU_SIM_START, "Start Simulation");

    AppendMenu(hReplayMenu, MF_STRING, MENU_REPLAY_START, "Open Replay");
    AppendMenu(hReplayMenu, MF_STRING, MENU_REPLAY_NEXT_FRAME, "Next Frame");
    AppendMenu(hReplayMenu, MF_STRING, MENU_REPLAY_NEXT_10_FRAME, "Next 10 Frames");
    AppendMenu(hReplayMenu, MF_STRING, MENU_REPLAY_PREVIOUS_FRAME, "Previous Frame");
    AppendMenu(hReplayMenu, MF_STRING, MENU_REPLAY_PREVIOUS_10_FRAME, "Previous 10 Frames");


    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSimulationMenu, "Simulation");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hReplayMenu, "Replay");

    return hMenu;
}

void MoveFrame(HWND hwnd, int offset)
{
    replay_frame = clamp(replay_frame + offset, 0, settings.sim_ticks);
    RequestReplayDraw(hwnd);
}   

void HandleWM_COMMAND(HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam)) {
        // Handle menu item commands here
        case MENU_SIM_START:
            // Code for handling Menu Item 1
            printf("Simulation Start");
            run_simulation();
            break;
        case MENU_REPLAY_START:
            replay_save.path = "./replay";
            OpenReplay(&replay_save);

            RequestReplayDraw(hwnd);
            break;
            
        case MENU_REPLAY_NEXT_FRAME:
            MoveFrame(hwnd, 1);
            break;
        case MENU_REPLAY_NEXT_10_FRAME:
            MoveFrame(hwnd, 10);
            break;
        case MENU_REPLAY_PREVIOUS_FRAME:
            MoveFrame(hwnd, -1);
            break;
        case MENU_REPLAY_PREVIOUS_10_FRAME:
            MoveFrame(hwnd, -10);
            break;

        // Add more cases as needed
    }
}

void HandleKeyDown(HWND hwnd, WPARAM key)
{
    switch (key)
    {
        case 0x27:
            MoveFrame(hwnd, 1);
            break;
        case 0x25:
            MoveFrame(hwnd, -1);
            break;
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    wchar_t msg[32];
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_COMMAND:
            HandleWM_COMMAND(hwnd, wParam);
            break;  
        case WM_KEYDOWN:
            HandleKeyDown(hwnd, wParam);
            // wprintf(L"WM_KEYDOWN: 0x%x\n", wParam);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    return 0;
}


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

    HMENU hMenu = CreateMenuOptions();
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