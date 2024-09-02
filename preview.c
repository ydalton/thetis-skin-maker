#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>

#include "thetisskinmaker.h"

#define CLASS_NAME "Win32ThetisSkinMakerPreview"

#define IMAGE_WIDTH 200

size_t bitmapWidth = 0, bitmapHeight = 0;

static HBITMAP
ResizeBitmap(HBITMAP hBitmap, int newWidth, int newHeight)
{
  HBITMAP hbmOldSource, hbmOldDest;
  HBITMAP hbmNew;
  HDC hdcSource, hdcDest;
  BITMAP bm;

  if(!GetObject(hBitmap, sizeof(BITMAP), &bm))
    return NULL;

  hdcSource = CreateCompatibleDC(NULL);
  if(!hdcSource)
    return NULL;

  hdcDest = CreateCompatibleDC(hdcSource);
  if(!hdcDest)
    return NULL;

  hbmNew = CreateCompatibleBitmap(hdcSource, newWidth, newHeight);
  if(!hbmNew)
    return NULL;

  hbmOldSource = (HBITMAP)SelectObject(hdcSource, hBitmap);
  hbmOldDest = (HBITMAP)SelectObject(hdcDest, hbmNew);

  StretchBlt(hdcDest, 0, 0, newWidth, newHeight, hdcSource, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

  SelectObject(hdcSource, hbmOldSource);
  SelectObject(hdcDest, hbmOldDest);
  DeleteDC(hdcSource);
  DeleteDC(hdcDest);

  return hbmNew;
}

static void
OnPreviewWindowCreate(HWND hwnd)
{
  HINSTANCE instance;
  HBITMAP resizedBitmap;
  HWND image;

  instance = GetModuleHandle(NULL);

  image = CreateWindowEx(0,
			 WC_STATIC,
			 "",
			 WS_CHILD | WS_VISIBLE | SS_BITMAP,
			 0,
			 0,
			 bitmapWidth,
			 bitmapHeight,
			 hwnd,
			 NULL,
			 instance,
			 NULL);

  CHECK(image);

  resizedBitmap = ResizeBitmap(imageBitmap, bitmapWidth, bitmapHeight);
  if(!resizedBitmap)
    {
      ERROR_BOX("Failed to resize bitmap!");
      return;
    }

  SendMessage(image, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) resizedBitmap);
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
  WNDCLASS wc = {0};
  BITMAP bitmap;
  HWND previewWindow;
  RECT rect = {0};

  instance = GetModuleHandle(NULL);

  if(!imageBitmap)
    {
      ERROR_BOX("Bitmap is not set!");
      return;
    }

  if(!GetObject(imageBitmap, sizeof(bitmap), &bitmap))
    {
      ERROR_BOX("Failed to get object for bitmap!");
      return;
    }

  wc.lpfnWndProc = PreviewWndProc;
  wc.lpszClassName = CLASS_NAME;
  wc.hbrBackground = (HBRUSH) COLOR_WINDOW;

  RegisterClass(&wc);

  float ratio = (float)IMAGE_WIDTH / bitmap.bmHeight;
  float new_height = bitmap.bmHeight * ratio;

  rect.right = IMAGE_WIDTH;
  rect.bottom = (int) new_height;

  /* get the normal size of the window, including window decorations */
  AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

  bitmapWidth = rect.right - rect.left;
  bitmapHeight = rect.bottom - rect.top;

  previewWindow = CreateWindowEx(0,
				 CLASS_NAME,
				 "Image preview",
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
