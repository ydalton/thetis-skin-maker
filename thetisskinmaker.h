#ifndef __COMMON_H_
#define __COMMON_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define WINDOW_NAME "ThetisSkinMaker"
#define WINDOW_WIDTH 450
#define WINDOW_HEIGHT 500

#if 0
#define SKIN_FOLDER "%APPDATA%\\OpenHPSDR\\Skins\\"
#else
#define SKIN_FOLDER "%APPDATA%\\"
#endif

#define SET_FONT(hwnd, font) \
  SendMessage(hwnd, WM_SETFONT, (WPARAM) font, MAKELPARAM(FALSE, 0))

#define ERROR_BOX(text) \
        MessageBox(NULL, text, WINDOW_NAME, MB_OK | MB_ICONERROR )
#define CREATE_GROUPBOX(text, x, y, width, height, parent, instance) \
  CreateWindowEx(0, \
		 WC_BUTTON, \
		 text, \
                 WS_CHILD | WS_VISIBLE | BS_GROUPBOX, \
		 x, \
		 y, \
		 width, \
		 height, \
		 parent, \
		 NULL, \
		 instance, \
		 NULL)

#define CHECK(hwnd)			\
  if(!hwnd) \
    {\
      ERROR_BOX("Failed to create " #hwnd "!");	\
    }\

extern HBITMAP imageBitmap;

void OnPreviewButtonClick(HWND hwnd);
void OnImageButtonClick(HWND hwnd);
void OnSaveButtonClick(HWND hwnd);
void OnCreate(HWND hwnd);

#endif
