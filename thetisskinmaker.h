#ifndef __COMMON_H_
#define __COMMON_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define WINDOW_NAME L"ThetisSkinMaker"
#define WINDOW_WIDTH 450
#define WINDOW_HEIGHT 500

#define THETIS_SKIN_PATH L"%APPDATA%\\OpenHPSDR\\Skins"
#define THETIS_PICDISPLAY_PATH L"Console\\picDisplay.png"

#define SET_FONT(hwnd, font) \
  SendMessage(hwnd, WM_SETFONT, (WPARAM) font, MAKELPARAM(FALSE, 0))

#define ERROR_BOX(text) \
  MessageBoxW(NULL, text, WINDOW_NAME, MB_OK | MB_ICONERROR )

#define ERROR_BOX_WITH_CAPTION(text, caption) \
  MessageBoxW(NULL, text, caption, MB_OK | MB_ICONERROR)

#define CREATE_GROUPBOX(text, x, y, width, height, parent, instance) \
  CreateWindowExW(0, \
		  WC_BUTTONW,				\
		  text,					\
		  WS_CHILD | WS_VISIBLE | BS_GROUPBOX,	\
		  x,					\
		  y,					\
		  width,				\
		  height,				\
		  parent,				\
		  NULL,					\
		  instance,				\
		  NULL)

#define CHECK(hwnd)			\
  if(!hwnd)					\
    {						\
      ERROR_BOX(L"Failed to create " #hwnd L"!");	\
    }							\


extern HBITMAP imageBitmap;

void OnPreviewButtonClick(HWND hwnd);
void OnImageButtonClick(HWND hwnd);
void OnSaveButtonClick(HWND hwnd);
void OnNewButtonClick(HWND hwnd);
void OnCreate(HWND hwnd);

HBITMAP CreateBitmapFromPath(LPWSTR path);
int SaveBitmapToFile(HBITMAP bitmap, LPWSTR path);

#endif
