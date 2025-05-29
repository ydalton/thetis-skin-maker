#ifndef __COMMON_H_
#define __COMMON_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define WINDOW_NAME L"ThetisSkinMaker"
#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 240

#define THETIS_SKIN_PATH L"%APPDATA%\\OpenHPSDR\\Skins"
#define THETIS_PICDISPLAY_PATH L"Console\\picDisplay.png"


#define ERROR_BOX(text) \
  MessageBoxW(NULL, text, WINDOW_NAME, MB_OK | MB_ICONERROR )

#define ERROR_BOX_WITH_CAPTION(text, caption) \
  MessageBoxW(NULL, text, caption, MB_OK | MB_ICONERROR)

#define THETIS_SKIN_NAME_MAX    64

typedef struct
{
    WCHAR skinName[THETIS_SKIN_NAME_MAX];
    WCHAR baseSkin[MAX_PATH];
    WCHAR filePath[MAX_PATH];
} ThetisSkin;

BOOL ThetisSkin_IsValid(ThetisSkin *pSkin, WCHAR *error, int max);
BOOL ThetisSkin_Save(ThetisSkin *pSkin, WCHAR *error, int max);

extern HBITMAP hbmpImage;

void OnPreview(HWND hwnd);
void OnBrowse(HWND hwnd);
void OnSave(HWND hwnd);
void OnReset(HWND hwnd);
void OnCreate(HWND hwnd);

HBITMAP CreateBitmapFromPath(LPWSTR path);
int SaveBitmapToFile(HBITMAP bitmap, LPWSTR path);

#endif
