
// // Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN                     // Exclude rarely-used stuff from Windows headers
#include <windows.h>
// #include <stdlib.h>
// #include <malloc.h>
// #include <memory.h>
// #include <tchar.h>

#if _DEBUG
#include <stdio.h>
#endif

#include "resource.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

const int SCREEN_SIZE = 300;

enum class TILE { NO_PLAYER, PLAYER_1, PLAYER_2, BOTH_PLAYERS };
enum class TURN { NO_PLAYER, PLAYER_1, PLAYER_2 };

TURN currentTurn = TURN::PLAYER_1;
TILE* lastPlayed = nullptr;

TILE board[3][3];

POINT mousePos = { 0 };


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void                NewGame();
bool                CheckForWin();
void                FillRectangle(int x, int y, int width, int height, HDC hdc, HBRUSH hBrush);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

#if _DEBUG
    AllocConsole();
    FILE* console;
    freopen_s(&console, "CONOUT$", "w", stdout);
#endif

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICKOATTWO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    hInst = hInstance; // Store instance handle in our global variable

    SetProcessDPIAware();

    LONG windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    RECT windowRect = { 0, 0, SCREEN_SIZE, SCREEN_SIZE };
    AdjustWindowRect(&windowRect, windowStyle, true);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    RECT clientRect = { 0 };
    SystemParametersInfo(SPI_GETWORKAREA, 0, &clientRect, 0);

    int displayWidth = clientRect.right - clientRect.left;
    int displayHeight = clientRect.bottom - clientRect.top;

    HWND hwnd = CreateWindowW(szWindowClass, szTitle, windowStyle,
        (displayWidth - windowWidth) / 2,
        (displayHeight - windowHeight) / 2,
        windowWidth, windowHeight,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd)
    {
        return false;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICKOATTWO));

    MSG msg = { 0 };

    // Main message loop:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            GetCursorPos(&mousePos);
            ScreenToClient(hwnd, &mousePos);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICKOATTWO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TICKOATTWO);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = nullptr;

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        SetTimer(hwnd, 1, 20, nullptr);
        break;

    case WM_TIMER:
        InvalidateRect(hwnd, nullptr, false);
        break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
            break;
        case ID_FILE_NEWGAME:
            NewGame();
            break;
        case IDM_EXIT:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        int tileSize = SCREEN_SIZE / 3;

        int mouseX = mousePos.x / tileSize;
        int mouseY = mousePos.y / tileSize;

        if (mouseX < 0 || mouseX > 2 || mouseY < 0 || mouseY > 2)
            break;

        TILE* tile = &board[mouseY][mouseX];

        bool validMove = false;

        if (tile == lastPlayed)
            break;

        if (currentTurn == TURN::PLAYER_1)
        {
            if (*tile == TILE::NO_PLAYER)
            {
                *tile = TILE::PLAYER_1;
                validMove = true;
            }
            else if (*tile == TILE::PLAYER_2)
            {
                *tile = TILE::BOTH_PLAYERS;
                validMove = true;
            }
        }

        else if (currentTurn == TURN::PLAYER_2)
        {
            if (*tile == TILE::NO_PLAYER)
            {
                *tile = TILE::PLAYER_2;
                validMove = true;
            }
            else if (*tile == TILE::PLAYER_1)
            {
                *tile = TILE::BOTH_PLAYERS;
                validMove = true;
            }
        }

        if (!validMove)
            break;

        lastPlayed = tile;

        if (CheckForWin())
            currentTurn = TURN::NO_PLAYER;
        else
        {
            // next turn
            if (currentTurn == TURN::PLAYER_1) currentTurn = TURN::PLAYER_2;
            else if (currentTurn == TURN::PLAYER_2) currentTurn = TURN::PLAYER_1;
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // TODO: Add any drawing code that uses hdc here...

        HDC memDC = CreateCompatibleDC(hdc);

        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, SCREEN_SIZE, SCREEN_SIZE);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

        LONG lineWidth = 6;

        // clear screen
        FillRectangle(0, 0, SCREEN_SIZE, SCREEN_SIZE, memDC, (HBRUSH)GetStockObject(WHITE_BRUSH));

        // draw mouse position
        int tileSize = SCREEN_SIZE / 3;

        if (currentTurn == TURN::NO_PLAYER)
        {
            FillRectangle(0, 0, SCREEN_SIZE, SCREEN_SIZE, memDC, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
        }
        else if (mousePos.x > 0 && mousePos.y > 0)
        {
            HBRUSH brush = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
            if (currentTurn == TURN::PLAYER_1)
                brush = CreateSolidBrush(RGB(255, 192, 192));
            else if (currentTurn == TURN::PLAYER_2)
                brush = CreateSolidBrush(RGB(170, 192, 255));

            FillRectangle(mousePos.x / tileSize * tileSize, mousePos.y / tileSize * tileSize, tileSize, tileSize, memDC, brush);
            DeleteObject(brush);
        }


        // draw board

        int lineX = 50;
        int lineY = 8;

        for (int i = 0; i < ARRAYSIZE(board); i++)
        {
            for (int j = 0; j < ARRAYSIZE(board[i]); j++)
            {
                if (&board[i][j] == lastPlayed)
                {
                    FillRectangle(j * tileSize, i * tileSize, tileSize, tileSize, memDC, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
                }

                if (board[i][j] == TILE::PLAYER_1 || board[i][j] == TILE::BOTH_PLAYERS)
                {
                    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
                    FillRectangle(
                        j * tileSize + tileSize / 2 - lineX / 2,
                        i * tileSize + tileSize / 2 - lineY / 2,
                        lineX, lineY, memDC, brush);
                    DeleteObject(brush);
                }

                if (board[i][j] == TILE::PLAYER_2 || board[i][j] == TILE::BOTH_PLAYERS)
                {
                    HBRUSH brush = CreateSolidBrush(RGB(0, 162, 232));
                    FillRectangle(
                        j * tileSize + tileSize / 2 - lineY / 2,
                        i * tileSize + tileSize / 2 - lineX / 2,
                        lineY, lineX, memDC, brush);
                    DeleteObject(brush);
                }
            }
        }

        FillRectangle(SCREEN_SIZE / 3 - lineWidth / 2, 0, lineWidth, SCREEN_SIZE, memDC, (HBRUSH)GetStockObject(BLACK_BRUSH));

        FillRectangle(SCREEN_SIZE / 3 * 2 - lineWidth / 2, 0, lineWidth, SCREEN_SIZE, memDC, (HBRUSH)GetStockObject(BLACK_BRUSH));

        FillRectangle(0, SCREEN_SIZE / 3 - lineWidth / 2, SCREEN_SIZE, lineWidth, memDC, (HBRUSH)GetStockObject(BLACK_BRUSH));

        FillRectangle(0, SCREEN_SIZE / 3 * 2 - lineWidth / 2, SCREEN_SIZE, lineWidth, memDC, (HBRUSH)GetStockObject(BLACK_BRUSH));


        BitBlt(hdc, 0, 0, SCREEN_SIZE, SCREEN_SIZE, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_ERASEBKGND:
        return true;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void NewGame()
{
    currentTurn = TURN::PLAYER_1;

    for (int i = 0; i < ARRAYSIZE(board); i++)
    {
        for (int j = 0; j < ARRAYSIZE(board[i]); j++)
            board[i][j] = TILE::NO_PLAYER;
    }

    lastPlayed = nullptr;
}

bool CheckForWin()
{
    for (int i = 0; i < ARRAYSIZE(board); i++)
    {
        if (board[i][0] == TILE::BOTH_PLAYERS && board[i][1] == TILE::BOTH_PLAYERS && board[i][2] == TILE::BOTH_PLAYERS)
            return true;
    }

    for (int j = 0; j < ARRAYSIZE(board[0]); j++)
    {
        if (board[0][j] == TILE::BOTH_PLAYERS && board[1][j] == TILE::BOTH_PLAYERS && board[2][j] == TILE::BOTH_PLAYERS)
            return true;
    }

    if (board[0][0] == TILE::BOTH_PLAYERS && board[1][1] == TILE::BOTH_PLAYERS && board[2][2] == TILE::BOTH_PLAYERS)
        return true;

    if (board[2][0] == TILE::BOTH_PLAYERS && board[1][1] == TILE::BOTH_PLAYERS && board[0][2] == TILE::BOTH_PLAYERS)
        return true;

    return false;
}

void FillRectangle(int x, int y, int width, int height, HDC hdc, HBRUSH hBrush)
{
    RECT rect = { x, y, x + width, y + height };
    FillRect(hdc, &rect, hBrush);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
