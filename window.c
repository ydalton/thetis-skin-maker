#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commdlg.h>

#include <stdio.h>
#include <assert.h>

#include "resources.h"
#include "thetisskinmaker.h"

HBITMAP imageBitmap;

static BOOL
FillBaseSkinDropDown(HWND hwnd)
{
  WIN32_FIND_DATA data;
  HANDLE file;
  char expandedPath[MAX_PATH];
  char wildcard[MAX_PATH];

  if(!ExpandEnvironmentStrings(THETIS_SKIN_PATH, expandedPath, MAX_PATH))
    return FALSE;

  strncpy(wildcard, expandedPath, MAX_PATH);
  strncat(wildcard, "\\*", MAX_PATH - 1);

  file = FindFirstFile(wildcard, &data);
  /* get all files in the skin folder */
  if(file != INVALID_HANDLE_VALUE)
    {
      char picdisplayPath[MAX_PATH];
      char *filename;
      do
	{
	  filename = data.cFileName;
	  if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
	    continue;

	  snprintf(picdisplayPath,
		   MAX_PATH,
		   "%s\\%s\\%s",
		   expandedPath,
		   filename,
		   THETIS_PICDISPLAY_PATH);

	  /* it's not a skin if it doesn't have at least the picdisplay file */
	  if(!PathFileExists(picdisplayPath))
	    continue;

	  SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM) filename);
	}
      while(FindNextFile(file, &data));
    }

  return TRUE;
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
			(HMENU) IDC_SKIN_NAME_EDIT,
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
			    CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VISIBLE | WS_CHILD | WS_VSCROLL,
			    20,
			    75,
			    WINDOW_WIDTH - 55,
			    200,
			    hwnd,
			    (HMENU)IDC_BASE_SKIN_DROPDOWN,
			    instance,
			    NULL);

  CHECK(comboBox);

  SendMessage(comboBox, EM_SETREADONLY, TRUE, 0);

  if(!FillBaseSkinDropDown(comboBox))
    ERROR_BOX("Failed filling in dropdown box!");

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
    return;

  previewButton = GetDlgItem(hwnd, IDC_PREVIEW_BUTTON);

  if(!previewButton)
    {
      ERROR_BOX("Failed to get preview button!");
    }

  /* make the button clickable */
  EnableWindow(previewButton, TRUE);

  /* FIXME: we need GDI+ to load images other than BMP */
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

static char *
GetDropDownItem(HWND hwnd)
{
  TCHAR *string;
  int index, length;

  /* get currently selected index */
  index = SendMessage(hwnd, CB_GETCURSEL, 0, 0);

  if(index == CB_ERR)
    return NULL;

  /* get length of currently selected item */
  length = SendMessage(hwnd, CB_GETLBTEXTLEN, index, 0);
  assert(length != CB_ERR);

  string = malloc((length + 1) * sizeof(TCHAR));
  SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM) string);

  return string;
}

int
CopyFolderRecursively(const char *src, const char *dest)
{
  SHFILEOPSTRUCT s = {0};

  s.wFunc = FO_COPY;
  s.fFlags = FOF_SILENT;
  s.pFrom = src;
  s.pTo = dest;

  return SHFileOperation(&s);
}

void
OnSaveButtonClick(HWND hwnd)
{
  HWND comboBox, edit;
  char expanded[MAX_PATH] = {0};
  char srcPath[MAX_PATH] = {0};
  char destPath[MAX_PATH] = {0};
  char skinName[64];
  char *sourceSkin;

  assert(ExpandEnvironmentStrings(THETIS_SKIN_PATH, expanded, MAX_PATH));

  edit = GetDlgItem(hwnd, IDC_SKIN_NAME_EDIT);
  assert(edit != NULL);

  GetWindowText(edit, skinName, 64);

  assert(skinName != NULL);
  if(strcmp(skinName, "") == 0)
    {
      ERROR_BOX_WITH_CAPTION("Please enter a skin name.", "Empty Skin Name");
      return;
    }

  comboBox = GetDlgItem(hwnd, IDC_BASE_SKIN_DROPDOWN);
  assert(comboBox != NULL);

  /* only returns NULL if no item is selected */
  sourceSkin = GetDropDownItem(comboBox);
  if(!sourceSkin)
    {
      ERROR_BOX_WITH_CAPTION("Please select a base skin.", "Base Skin Not Selected");
      return;
    }

  snprintf(srcPath, MAX_PATH, "%s\\%s", expanded, sourceSkin);
  snprintf(destPath, MAX_PATH, "%s\\%s", expanded, skinName);

  if(PathFileExists(destPath))
    {
      ERROR_BOX("Skin already exists. Please select another name!");
      return;
    }

  if(CopyFolderRecursively(srcPath, destPath))
    {
      ERROR_BOX("Failed to copy folder for skin. This should not happen.");
      return;
    }

  MessageBox(NULL,
	     "Skin successfully saved! Please restart Thetis if it is already open.",
	     "Skin Saved",
	     MB_OK | MB_ICONINFORMATION);
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
