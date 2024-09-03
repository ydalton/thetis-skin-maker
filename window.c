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
  WIN32_FIND_DATAW data;
  HANDLE file;
  LPWSTR filename;
  WCHAR expandedPath[MAX_PATH] = {0};
  WCHAR wildcard[MAX_PATH] = {0};
  WCHAR picdisplayPath[MAX_PATH] = {0};

  if(!ExpandEnvironmentStringsW(THETIS_SKIN_PATH, expandedPath, MAX_PATH))
    return FALSE;

  /* ??? why does _ make all the difference? */
  _snwprintf(wildcard, MAX_PATH, L"%s\\*", expandedPath);

  file = FindFirstFileW(wildcard, &data);
  /* get all files in the skin folder */

  if(file == INVALID_HANDLE_VALUE)
    return FALSE;

  do
    {
      filename = data.cFileName;
      if(wcscmp(filename, L".") == 0 || wcscmp(filename, L"..") == 0)
	continue;

      _snwprintf(picdisplayPath,
		MAX_PATH,
		L"%s\\%s\\%s",
		expandedPath,
		filename,
		THETIS_PICDISPLAY_PATH);

      /* it's not a skin if it doesn't have at least the picdisplay file */
      if(!PathFileExistsW(picdisplayPath))
	continue;

      SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM) filename);
    }
  while(FindNextFileW(file, &data));

  return TRUE;
}

static void
CreateActionControls(HWND hwnd, HFONT font, HINSTANCE instance)
{
  HWND groupBox, saveButton, newButton;

  groupBox = CREATE_GROUPBOX(L"Actions", 10, 160, WINDOW_WIDTH - 35, 50, hwnd, instance);

  CHECK(groupBox);

  saveButton = CreateWindowExW(0,
			      WC_BUTTONW,
			      L"Save skin",
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

  newButton = CreateWindowExW(0,
			     WC_BUTTONW,
			     L"New skin",
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

  groupBox = CREATE_GROUPBOX(L"Skin name", 10, 4, WINDOW_WIDTH - 35, 50, hwnd, instance);

  CHECK(groupBox)

  edit = CreateWindowExW(0,
			WC_EDITW,
			L"",
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

  groupBox = CREATE_GROUPBOX(L"Base skin", 10, 55, WINDOW_WIDTH - 35, 50, hwnd, instance);

  CHECK(groupBox)

  comboBox = CreateWindowExW(0,
			    WC_COMBOBOXW,
			    L"",
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
    ERROR_BOX(L"Failed filling in dropdown box!");

  SET_FONT(groupBox, font);
  SET_FONT(comboBox, font);
}

static void
CreateBackgroundImageControls(HWND hwnd, HFONT font, HINSTANCE instance)
{
  HWND groupBox, button, previewButton;

  groupBox = CREATE_GROUPBOX(L"Background image", 10, 105, WINDOW_WIDTH - 35, 55, hwnd, instance);

  CHECK(groupBox);

  button = CreateWindowExW(0,
			   WC_BUTTONW,
			   L"Select background image",
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

  previewButton = CreateWindowExW(0,
				  WC_BUTTONW,
				  L"View preview",
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

  OPENFILENAMEW ofn = {0};
  WCHAR fileName[MAX_PATH] = L"";
  ofn.lpstrFilter = L"All Image files\0" "*.png;*.jpg;*.jpeg;*.bmp\0" "All\0*.*\0";
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lStructSize = sizeof(OPENFILENAMEW);
  ofn.hwndOwner = hwnd;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  if(!GetOpenFileNameW(&ofn))
    return;

  previewButton = GetDlgItem(hwnd, IDC_PREVIEW_BUTTON);

  if(!previewButton)
    {
      ERROR_BOX(L"Failed to get preview button!");
    }

  /* make the button clickable */
  EnableWindow(previewButton, TRUE);

  /* FIXME: we need GDI+ to load images other than BMP */
  imageBitmap = CreateBitmapFromPath(fileName);
  if(!imageBitmap)
    {
      ERROR_BOX(L"Failed to load image!");
      return;
    }
}

static WCHAR *
GetDropDownItem(HWND hwnd)
{
  LPWSTR string;
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
CopyFolderRecursively(LPWSTR src, LPWSTR dest)
{
  SHFILEOPSTRUCTW s = {0};

  s.wFunc = FO_COPY;
  s.fFlags = FOF_SILENT;
  s.pFrom = src;
  s.pTo = dest;

  return SHFileOperationW(&s);
}

void
OnSaveButtonClick(HWND hwnd)
{
  HWND comboBox, edit;
  WCHAR expanded[MAX_PATH] = {0};
  WCHAR srcPath[MAX_PATH] = {0};
  WCHAR destPath[MAX_PATH] = {0};
  WCHAR skinName[64];
  WCHAR *sourceSkin;
  int pathIsExpanded = 0;

  pathIsExpanded = ExpandEnvironmentStringsW(THETIS_SKIN_PATH, expanded, MAX_PATH);

  assert(pathIsExpanded);

  edit = GetDlgItem(hwnd, IDC_SKIN_NAME_EDIT);
  assert(edit != NULL);

  GetWindowTextW(edit, skinName, 64);

  assert(skinName != NULL);
  if(wcscmp(skinName, L"") == 0)
    {
      ERROR_BOX_WITH_CAPTION(L"Please enter a skin name.", L"Empty Skin Name");
      return;
    }

  comboBox = GetDlgItem(hwnd, IDC_BASE_SKIN_DROPDOWN);
  assert(comboBox != NULL);

  /* only returns NULL if no item is selected */
  sourceSkin = GetDropDownItem(comboBox);
  if(!sourceSkin)
    {
      ERROR_BOX_WITH_CAPTION(L"Please select a base skin.", L"Base Skin Not Selected");
      return;
    }

  _snwprintf(srcPath, MAX_PATH, L"%s\\%s", expanded, sourceSkin);
  _snwprintf(destPath, MAX_PATH, L"%s\\%s", expanded, skinName);

  if(PathFileExistsW(destPath))
    {
      ERROR_BOX(L"Skin already exists. Please select another name!");
      return;
    }

  if(CopyFolderRecursively(srcPath, destPath))
    {
      ERROR_BOX(L"Failed to copy folder for skin. This should not happen.");
      return;
    }

  MessageBoxW(NULL,
	     L"Skin successfully saved! Please restart Thetis if it is already open.",
	     L"Skin Saved",
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
