#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "tsm.h"
#include "rsrc.h"

#define CLASS_NAME L"ThetisSkinMakerWndClass"
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 240

static BOOL ThetisSkinsExist(void);
static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
static LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    WNDCLASSW wc = {0};
    MSG msg = {0};
    int screenWidth, screenHeight, windowX, windowY;
    HWND hwndMain = NULL;

    (void) prevInstance;
    (void) cmdLine;

    if(!ThetisSkinsExist())
    {
    	int ret = MessageBoxW(NULL,
    			L"No existing Thetis skins were found. Do you wish to continue?",
    			L"Skins Not Found",
    			MB_YESNO | MB_ICONWARNING);

    	if (ret == IDNO) {
    		return -1;
    	}
    }

    wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(ID_ICON));

    RegisterClassW(&wc);

    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    windowX = (screenWidth/2) - (WINDOW_WIDTH/2);
    windowY = (screenHeight/2) - (WINDOW_HEIGHT/2);

    hwndMain = CreateWindowExW(0,
                               CLASS_NAME,
                               WINDOW_NAME,
                               WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX,
                               windowX, /* x */
                               windowY, /* y */
                               WINDOW_WIDTH, /* width */
                               WINDOW_HEIGHT, /* height */
                               NULL,
                               NULL,
                               instance,
                               NULL);

    if(!hwndMain)
    {
        ERROR_BOX(L"Failed to create window!");
        return -1;
    }

    ShowWindow(hwndMain, cmdShow);

    while(GetMessage(&msg, NULL, 0, 0))
    {
        if (IsDialogMessage(hwndMain, &msg))
            continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

static BOOL ThetisSkinsExist(void)
{
    BOOL shouldContinue = FALSE;
    WCHAR expanded[MAX_PATH];

    ExpandEnvironmentStringsW(THETIS_SKIN_PATH, expanded, MAX_PATH);

    shouldContinue = PathFileExistsW(expanded);

    return shouldContinue;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    (void) lparam;

    switch(msg)
    {
        case WM_CREATE:
            OnCreate(hwnd);
            break;
        case WM_DESTROY:
            if(hbmpImage)
            {
                DeleteObject(hbmpImage);
            }
            PostQuitMessage(0);
            break;
        case WM_SETCURSOR:
            /* Set the cursor when it's over the client area */
            if (LOWORD(lparam) == HTCLIENT) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_COMMAND:
            switch(LOWORD(wparam))
            {
                case IDM_HELP_ABOUT:
                    DialogBoxW(GetModuleHandle(NULL),
                        MAKEINTRESOURCEW(IDD_ABOUT),
                        hwnd,
                        AboutDlgProc);
                    break;
                case IDC_PREVIEW_BUTTON:
                    OnPreview(hwnd);
                    break;
                case IDC_BROWSE_BUTTON:
                    OnBrowse(hwnd);
                    break;
                case IDC_SAVE_BUTTON:
                    OnSave(hwnd);
                    break;
                case IDC_RESET_BUTTON:
                    OnReset(hwnd);
                    break;
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

static LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = TRUE;

    (void) lparam;

    switch(msg)
    {
        case WM_INITDIALOG:
            break;
        case WM_COMMAND:
            switch(LOWORD(wparam))
            {
                case IDOK:
                    EndDialog(hwnd, IDOK);
                    break;
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    break;
            }
            break;
        default:
            result = FALSE;
            break;
    }

    return result;
}
