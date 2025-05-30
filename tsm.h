#ifndef _TSM_H_
#define _TSM_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Thetis specific paths */
#define THETIS_SKIN_PATH L"%APPDATA%\\OpenHPSDR\\Skins"
#define THETIS_PICDISPLAY_PATH L"Console\\picDisplay.png"

#define WINDOW_NAME L"ThetisSkinMaker"

#define ERROR_BOX(text) \
  MessageBoxW(NULL, text, WINDOW_NAME, MB_OK | MB_ICONERROR )

#define THETIS_SKIN_NAME_MAX    64

extern BOOL g_thetisSkinsExist;

typedef struct
{
    WCHAR skinName[THETIS_SKIN_NAME_MAX];
    WCHAR baseSkin[MAX_PATH];
    WCHAR filePath[MAX_PATH];
} ThetisSkin;

BOOL ThetisSkin_IsValid(ThetisSkin *pSkin, WCHAR *error, int max);
BOOL ThetisSkin_Save(ThetisSkin *pSkin, WCHAR *error, int max);

extern HBITMAP hbmpImage;

void OnCreate(HWND hwnd);
void OnBrowse(HWND hwnd);
void OnPreview(HWND hwnd);
void OnSave(HWND hwnd);
void OnReset(HWND hwnd);

/* bitmap */
HBITMAP CreateBitmapFromPath(LPWSTR path);
int SaveBitmapToFile(HBITMAP bitmap, LPWSTR path);

/* utilities */
int CopyFolderRecursively(LPWSTR src, LPWSTR dest);
BOOL FileNameIsValid(LPWSTR name);

#endif /* _TSM_H_ */
