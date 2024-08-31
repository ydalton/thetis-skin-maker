#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <commdlg.h>
#include <propidl.h>
#include <gdiplus.h>

#include "resources.h"

#define CLASS_NAME "Win32SkinMakerClass"
#define WINDOW_NAME "ThetisSkinMaker"

#define WINDOW_WIDTH 450
#define WINDOW_HEIGHT 500

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

HBITMAP bitmap;
		 

void
CreateSkinNameControls(HWND hwnd, HFONT font, HINSTANCE instance)
{
  HWND edit, groupBox;

  groupBox = CREATE_GROUPBOX("Skin name", 10, 4, WINDOW_WIDTH - 35, 50, hwnd, instance);

  CHECK(groupBox)

  edit = CreateWindowEx(0,
			WC_EDIT,
			"",
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			20,
			25,
			WINDOW_WIDTH - 55,
			20,
			hwnd,
			NULL,
			instance,
			NULL);

  CHECK(edit)

  SET_FONT(groupBox, font);
  SET_FONT(edit, font);
}

void CreateBaseSkinControls(HWND hwnd, HFONT font, HINSTANCE instance)
{
  HWND comboBox, groupBox;

  groupBox = CREATE_GROUPBOX("Base skin", 10, 55, WINDOW_WIDTH - 35, 50, hwnd, instance);

  CHECK(groupBox)

  comboBox = CreateWindowEx(0,
			    WC_COMBOBOX,
			    "",
			    CBS_DROPDOWN | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_VSCROLL,
			    20,
			    75,
			    WINDOW_WIDTH - 55,
			    200,
			    hwnd,
			    NULL,
			    instance,
			    NULL);

  CHECK(comboBox)

  SET_FONT(groupBox, font);
  SET_FONT(comboBox, font);
}

void
CreateBackgroundImageControls(HWND hwnd, HFONT font, HINSTANCE instance)
{
  HWND groupBox, button, image;

  groupBox = CREATE_GROUPBOX("Background image", 10, 105, WINDOW_WIDTH - 35, 70, hwnd, instance);

  CHECK(groupBox)

  button = CreateWindowEx(0,
			  WC_BUTTON,
			  "Select background image",
			  WS_CHILD | WS_VISIBLE,
			  20,
			  125,
			  WINDOW_WIDTH - 55,
			  25,
			  hwnd,
			  (HMENU) IDC_IMAGE_BUTTON,
			  instance,
			  NULL);

  CHECK(button)

  image = CreateWindowEx(0,
			 WC_STATIC,
			 "",
			 WS_CHILD | WS_VISIBLE | SS_BITMAP,
			 20,
			 160,
			 WINDOW_WIDTH - 55,
			 250,
			 hwnd,
			 (HMENU) IDC_IMAGE,
			 instance,
			 NULL);

  CHECK(image)

  SET_FONT(groupBox, font);
  SET_FONT(button, font);
}

void
OnCreate(HWND hwnd)
{
  HFONT font;
  HINSTANCE instance;

  instance = GetModuleHandle(NULL);
  font = GetStockObject(DEFAULT_GUI_FONT);

  CreateSkinNameControls(hwnd, font, instance);
  CreateBaseSkinControls(hwnd, font, instance);
  CreateBackgroundImageControls(hwnd, font, instance);
}

void
OnImageButtonClick(HWND hwnd)
{
  HWND image;
    
  OPENFILENAME ofn = {0};
  char fileName[MAX_PATH] = "";
  ofn.lpstrFilter = "All Image files\0" "*.png;*.jpg;*.jpeg;*.bmp\0" "All\0*.*\0";
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwnd;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  if(!GetOpenFileName(&ofn))
    {
      return;
    }

  image = GetDlgItem(hwnd, IDC_IMAGE);

  if(!image)
    {
      MessageBox(hwnd, "Failed to retrieve image control!", WINDOW_NAME, MB_OK | MB_ICONERROR);
    }

  bitmap = LoadImageA(NULL,
		     fileName,
		     IMAGE_BITMAP,
		     0,
		     0,
		     LR_LOADFROMFILE);
  if(!bitmap)
    {
      ERROR_BOX("Failed to load image!");
      return;
    }

  SendMessage(image, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bitmap);
}

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch(msg)
    {
    case WM_CREATE:
      OnCreate(hwnd);
      break;      
    case WM_DESTROY:
      DeleteObject(bitmap);
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
	case IDC_IMAGE_BUTTON:
	  OnImageButtonClick(hwnd);
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
