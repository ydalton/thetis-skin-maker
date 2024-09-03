#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <propidl.h>
#include <gdiplus.h>

#include <assert.h>

#include "thetisskinmaker.h"

CLSID g_pngEncoder = {0};

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

static CLSID
GetEncoderForFormat(LPWSTR format)
{
  ImageCodecInfo *info;
  UINT num = 0, size = 0;

  GdipGetImageEncodersSize(&num, &size);
  if(!size)
    return CLSID_NULL;

  info = malloc(size);
  assert(info != NULL);

  GdipGetImageEncoders(num, size, info);

  for(int i = 0; i < num; i++)
    {
      if(wcscmp(info[i].MimeType, format) == 0)
	return info[i].Clsid;
    }

  return CLSID_NULL;
}


int SaveBitmapToFile(HBITMAP bitmap, LPWSTR path)
{
  GdiplusStartupInput startup = {0};
  ULONG_PTR token;
  GpBitmap *gdiBmp;
  int ret = 0;

  startup.GdiplusVersion = 1;

  ret = GdiplusStartup(&token, &startup, NULL);
  /* you better work if we already loaded the image */
  assert(ret == 0);
  ret = GdipCreateBitmapFromHBITMAP(bitmap, NULL, &gdiBmp);
  if(ret != 0)
    return ret;

  if(IsEqualCLSID(&g_pngEncoder, &CLSID_NULL))
    /* we will always have to use PNG */
    g_pngEncoder = GetEncoderForFormat(L"image/png");

  assert(!IsEqualCLSID(&g_pngEncoder, &CLSID_NULL));

  ret = GdipSaveImageToFile(gdiBmp, path, &g_pngEncoder, NULL);
  if(ret)
    return ret;

  GdipDisposeImage((GpImage *) gdiBmp);
  GdiplusShutdown(token);

  return 0;
}
