#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <propidl.h>
#include <gdiplus.h>

HBITMAP GetBitmapFromPath(LPWSTR path)
{
  Gdiplus::GdiplusStartupInput startup;
  Gdiplus::Bitmap *gdiBmp;
  ULONG_PTR token;
  HBITMAP bitmap = NULL;

  Gdiplus::GdiplusStartup(&token, &startup, NULL);
  gdiBmp = Gdiplus::Bitmap::FromFile(path, false);
  if(!bitmap)
    {
      gdiBmp->GetHBITMAP(Gdiplus::Color(255, 255, 255), &bitmap);
      delete gdiBmp;
    }
  Gdiplus::GdiplusShutdown(token);
  return bitmap;
}

