#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>

#include "resources.h"
#include "thetisskinmaker.h"

HBITMAP imageBitmap;

static void
FillBaseSkinDropDown(HWND hwnd)
{

}

static void
CreateActionControls(HWND hwnd, HFONT font, HINSTANCE instance)
{
  HWND groupBox, saveButton, newButton;

  groupBox = CREATE_GROUPBOX("Actions", 10, 160, WINDOW_WIDTH - 35, 50, hwnd, instance);

  CHECK(groupBox);

  saveButton = CreateWindowEx(0,
			      WC_BUTTON,
			      "Save skin",
			      WS_VISIBLE | WS_CHILD,
			      20,
			      178,
			      190,
			      25,
			      hwnd,
			      (HMENU) IDC_SAVE_BUTTON,
			      instance,
			      NULL);

  CHECK(saveButton);

  newButton = CreateWindowEx(0,
			     WC_BUTTON,
			     "New skin",
			     WS_VISIBLE | WS_CHILD,
			     220,
			     178,
			     195,
			     25,
			     hwnd,
			     (HMENU) IDC_NEW_BUTTON,
			     instance,
			     NULL);

  CHECK(newButton);

  SET_FONT(groupBox, font);
  SET_FONT(saveButton, font);
  SET_FONT(newButton, font);
}

static void
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


static void
CreateBaseSkinControls(HWND hwnd, HFONT font, HINSTANCE instance)
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

  CHECK(comboBox);

  FillBaseSkinDropDown(comboBox);

  SET_FONT(groupBox, font);
  SET_FONT(comboBox, font);
}

static void
CreateBackgroundImageControls(HWND hwnd, HFONT font, HINSTANCE instance)
{
  HWND groupBox, button, previewButton;

  groupBox = CREATE_GROUPBOX("Background image", 10, 105, WINDOW_WIDTH - 35, 55, hwnd, instance);

  CHECK(groupBox);

  button = CreateWindowEx(0,
			  WC_BUTTON,
			  "Select background image",
			  WS_CHILD | WS_VISIBLE,
			  20,
			  125,
			  190,
			  25,
			  hwnd,
			  (HMENU) IDC_IMAGE_BUTTON,
			  instance,
			  NULL);

  CHECK(button);

  previewButton = CreateWindowEx(0,
				 WC_BUTTON,
				 "View preview",
				 WS_CHILD | WS_VISIBLE | WS_DISABLED,
				 220,
				 125,
				 195,
				 25,
				 hwnd,
				 (HMENU) IDC_PREVIEW_BUTTON,
				 instance,
				 NULL);

  CHECK(previewButton);

  SET_FONT(groupBox, font);
  SET_FONT(button, font);
  SET_FONT(previewButton, font);
}

void
OnImageButtonClick(HWND hwnd)
{
  HWND previewButton;

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

  previewButton = GetDlgItem(hwnd, IDC_PREVIEW_BUTTON);

  if(!previewButton)
    {
      ERROR_BOX("Failed to get preview button!");
    }

#if 0
  /* make the button clickable */
  EnableWindow(previewButton, TRUE);
#endif

  imageBitmap = LoadImageA(NULL,
		     fileName,
		     IMAGE_BITMAP,
		     0,
		     0,
		     LR_LOADFROMFILE);
  if(!imageBitmap)
    {
      ERROR_BOX("Failed to load image!");
      return;
    }
}

void
OnSaveButtonClick(HWND hwnd)
{

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
  CreateActionControls(hwnd, font, instance);
}
