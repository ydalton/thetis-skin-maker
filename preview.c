#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

#include <assert.h>

#include "thetisskinmaker.h"
#include "resources.h"

#define CLASS_NAME L"Win32ThetisSkinMakerPreview"

#define IMAGE_WIDTH 450

size_t bitmapWidth = 0, bitmapHeight = 0;

ATOM wcPreview = 0;

static LRESULT CALLBACK PreviewWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch(msg)
    {
        case WM_CREATE:
            {
                HINSTANCE instance;
                HWND image;

                instance = GetModuleHandle(NULL);

                image = CreateWindowExW(0,
                    WC_STATICW,
                    L"",
                    WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_REALSIZECONTROL,
                    0,
                    0,
                    bitmapWidth,
                    bitmapHeight,
                    hwnd,
                    (HMENU) IDC_PREVIEW_STATIC_BITMAP,
                    instance,
                    NULL);

                SendMessage(image, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hbmpImage);
            }
            break;
        case WM_SIZE:
            {
                HWND hwndStaticBitmap;

                hwndStaticBitmap = GetDlgItem(hwnd, IDC_PREVIEW_STATIC_BITMAP);
                assert(hwndStaticBitmap != NULL);

                SetWindowPos(hwndStaticBitmap,
                    NULL,
                    0,
                    0,
                    LOWORD(lparam),
                    HIWORD(lparam),
                    0);
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

void OnPreview(HWND hwnd)
{
    HINSTANCE instance;
    BITMAP bitmap;
    HWND hwndPreview;
    RECT rect = {0};
    float ratio, new_height;

    instance = GetModuleHandle(NULL);

    assert(hbmpImage != NULL);

    if(!GetObject(hbmpImage, sizeof(bitmap), &bitmap))
    {
        ERROR_BOX(L"Failed to get object for bitmap!");
        return;
    }

    if (wcPreview == 0)
    {
        WNDCLASSW wc = {0};
        /* the whole window needs to be repainted */
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = PreviewWndProc;
        wc.lpszClassName = CLASS_NAME;
        wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
        wc.hInstance = instance;
        wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(ID_ICON));

        wcPreview = RegisterClassW(&wc);
    }

    ratio = (float)IMAGE_WIDTH / bitmap.bmWidth;
    new_height = ((float) bitmap.bmHeight) * ratio;

    rect.right = IMAGE_WIDTH;
    rect.bottom = (int) new_height;

    /* get the normal size of the window, including window decorations */
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

    bitmapWidth = rect.right - rect.left;
    bitmapHeight = rect.bottom - rect.top;

    hwndPreview = CreateWindowExW(0,
                                  CLASS_NAME,
                                  L"Image preview",
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  bitmapWidth,
                                  bitmapHeight,
                                  hwnd,
                                  NULL,
                                  instance,
                                  NULL);

    ShowWindow(hwndPreview, SW_NORMAL);
}
