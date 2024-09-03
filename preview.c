#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <assert.h>

#include "thetisskinmaker.h"

#define CLASS_NAME L"Win32ThetisSkinMakerPreview"

#define IMAGE_WIDTH 450

size_t bitmapWidth = 0, bitmapHeight = 0;

static void
OnPreviewWindowCreate(HWND hwnd)
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
			  NULL,
			  instance,
			  NULL);

  CHECK(image);

  SendMessage(image, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) imageBitmap);
}

static LRESULT CALLBACK
PreviewWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  switch(msg)
    {
    case WM_CREATE:
      OnPreviewWindowCreate(hwnd);
      break;
    default:
      return DefWindowProc(hwnd, msg, wparam, lparam);
    }
  return 0;
}

void
OnPreviewButtonClick(HWND hwnd)
{
  HINSTANCE instance;
  WNDCLASSW wc = {0};
  BITMAP bitmap;
  HWND previewWindow;
  RECT rect = {0};

  instance = GetModuleHandle(NULL);

  assert(imageBitmap != NULL);

  if(!GetObject(imageBitmap, sizeof(bitmap), &bitmap))
    {
      ERROR_BOX(L"Failed to get object for bitmap!");
      return;
    }

  wc.lpfnWndProc = PreviewWndProc;
  wc.lpszClassName = CLASS_NAME;
  wc.hbrBackground = (HBRUSH) COLOR_WINDOW;

  RegisterClassW(&wc);

  float ratio = (float)IMAGE_WIDTH / bitmap.bmWidth;
  float new_height = ((float) bitmap.bmHeight) * ratio;

  printf("%f %f", (float) IMAGE_WIDTH, new_height);

  rect.right = IMAGE_WIDTH;
  rect.bottom = (int) new_height;

  /* get the normal size of the window, including window decorations */
  AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

  bitmapWidth = rect.right - rect.left;
  bitmapHeight = rect.bottom - rect.top;

  previewWindow = CreateWindowExW(0,
				  CLASS_NAME,
				  L"Image preview",
				  WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX,
				  CW_USEDEFAULT,
				  CW_USEDEFAULT,
				  bitmapWidth,
				  bitmapHeight,
				  hwnd,
				  NULL,
				  instance,
				  NULL);

  CHECK(previewWindow);

  ShowWindow(previewWindow, SW_NORMAL);
}
