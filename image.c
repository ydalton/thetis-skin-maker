#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <propidl.h>
#include <gdiplus.h>

#include <assert.h>

#include "thetisskinmaker.h"

HBITMAP CreateBitmapFromPath(LPWSTR path)
{
  GdiplusStartupInput startup = {0};
  GpBitmap *gdiBmp;
  ULONG_PTR token;
  HBITMAP bitmap = NULL;
  int ret = 0;

  startup.GdiplusVersion = 1;

  ret = GdiplusStartup(&token, &startup, NULL);
  assert(ret == 0 && "GDI+ failed to initialize. This should not happen.");
  GdipCreateBitmapFromFile(path, &gdiBmp);
  if(gdiBmp)
    {
      GdipCreateHBITMAPFromBitmap(gdiBmp, &bitmap, 0x00FFFFFF);
      GdipDisposeImage((GpImage *) gdiBmp);
    }

  GdiplusShutdown(token);

  return bitmap;
}
