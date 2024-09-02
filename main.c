#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <propidl.h>
#include <gdiplus.h>

#include "resources.h"
#include "thetisskinmaker.h"

#define CLASS_NAME "Win32SkinMakerClass"

BOOL
ShouldContinue(void)
{
  char expanded[MAX_PATH];

  if(!ExpandEnvironmentStringsA(SKIN_FOLDER, expanded, MAX_PATH))
    return FALSE;

  if(!PathFileExistsA(SKIN_FOLDER))
    return FALSE;

  return TRUE;
}

static LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch(msg)
    {
    case WM_CREATE:
      OnCreate(hwnd);
      break;      
    case WM_DESTROY:
      if(imageBitmap)
	{
	  DeleteObject(imageBitmap);
	}
      PostQuitMessage(0);
      break;
    case WM_COMMAND:
      switch(LOWORD(wparam))
	{
	case IDM_HELP_ABOUT:
	  ShellAboutA(hwnd,
		      WINDOW_NAME,
		      "A program to create skins to be used by OpenHPSDR Thetis.",
		      NULL);
	  break;
	case IDC_PREVIEW_BUTTON:
	  OnPreviewButtonClick(hwnd);
	  break;
	case IDC_IMAGE_BUTTON:
	  OnImageButtonClick(hwnd);
	  break;
	case IDC_SAVE_BUTTON:
	  OnSaveButtonClick(hwnd);
	  break;
	}
      break;
    default:
      return DefWindowProc(hwnd, msg, wparam, lparam);
    }

  return 0;
}

int WINAPI WinMain(HINSTANCE instance,
		   HINSTANCE prevInstance,
		   LPSTR cmdLine,
		   int cmdShow)
{
  WNDCLASS wc = {0};

  if(!ShouldContinue())
    {
      MessageBox(NULL,
		 "No skins were found. Is Thetis installed properly?",
		 "Skins Not Found",
		 MB_OK | MB_ICONERROR);
      return -1;
    }

  wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
  wc.lpfnWndProc = WndProc;
  wc.hInstance = instance;
  wc.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
  wc.lpszClassName = CLASS_NAME;

  RegisterClass(&wc);

  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  
  int windowX = (screenWidth/2) - (WINDOW_WIDTH/2);
  int windowY = (screenHeight/2) - (WINDOW_HEIGHT/2);

  HWND hwnd = CreateWindowEx(0,
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
  if(!hwnd)
    {
      OutputDebugStringA("Could not create window.\n");
      return -1;
    }

  ShowWindow(hwnd, cmdShow);

  MSG msg = {0};

  while(GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

  return 0;
}
