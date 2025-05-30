#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commdlg.h>

#include <stdio.h>

#include "rsrc.h"
#include "tsm.h"

struct CreateWindowInfo
{
    int extended_style;
    LPWSTR className;
    LPWSTR text;
    int style;
    int x, y;
    int width, height;
    uintptr_t menu;
    HWND *pHwnd;
};

HBITMAP hbmpImage  = NULL;

HWND lblSkinName   = NULL;
HWND txtSkinName   = NULL;
HWND cboBaseSkin   = NULL;
HWND lblBaseSkin   = NULL;
HWND btnSave       = NULL;
HWND grpBackground = NULL;
HWND btnBrowse     = NULL;
HWND txtFile       = NULL;
HWND btnPreview    = NULL;
HWND btnReset      = NULL;
HWND lblFile       = NULL;
HWND stsStatus     = NULL;

static const struct CreateWindowInfo child_controls[] = {
    /* extended style   class             text                 style                                                         x    y    width  height hmenu,                hwnd           */
    { 0,                WC_STATICW,       L"Skin name: ",       0,                                                           12,  12,  80,    13,    0,                    &lblSkinName   },
    { WS_EX_CLIENTEDGE, WC_EDITW,         NULL,                 WS_TABSTOP,                                                  77,  9,   207,   20,    0,                    &txtSkinName   },
    { WS_EX_CLIENTEDGE, WC_COMBOBOXW,     NULL,                 CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP, 77,  35,  207,   21,    0,                    &cboBaseSkin   },
    { 0,                WC_STATICW,       L"Base skin: ",       0,                                                           12,  38,  56,    13,    0,                    &lblBaseSkin   },
    { 0,                WC_BUTTONW,       L"Background image",  BS_GROUPBOX,                                                 12,  62,  272,   78,    0,                    &grpBackground },
    { 0,                WC_BUTTONW,       L"Browse...",         BS_PUSHBUTTON | WS_TABSTOP,                                  198, 78,  75,    23,    IDC_BROWSE_BUTTON,    &btnBrowse     },
    { WS_EX_CLIENTEDGE, WC_EDITW,         NULL,                 ES_READONLY,                                                 77,  80,  114,   20,    0,                    &txtFile       },
    { 0,                WC_BUTTONW,       L"Preview",           BS_PUSHBUTTON | WS_TABSTOP,                                  198, 107, 75,    23,    IDC_PREVIEW_BUTTON,   &btnPreview    },
    { 0,                WC_BUTTONW,       L"Reset",             BS_PUSHBUTTON | WS_TABSTOP,                                  12,  150, 75,    23,    IDC_RESET_BUTTON,     &btnReset      },
    { 0,                WC_BUTTONW,       L"Save",              BS_DEFPUSHBUTTON | WS_TABSTOP,                               209, 150, 75,    23,    IDC_SAVE_BUTTON,      &btnSave       },
    { 0,                WC_STATICW,       L"File: ",            0,                                                           23,  83,  26,    13,    0,                    &lblFile       },
    { 0,                STATUSCLASSNAMEW, L"Version: " VERSION, 0,                                                           0,   0,   0,     0,     0,                    &stsStatus     },
};

void CreateControls(HWND hwndParent);
void FillComboBoxBaseSkin(void);
void SetDefaults(void);
void SetControls(void);
void LoadSkin(ThetisSkin *pSkin);

void OnCreate(HWND hwndMain)
{
    SetCursor(LoadCursor(NULL, IDC_APPSTARTING));

    CreateControls(hwndMain);
    FillComboBoxBaseSkin();
    SetDefaults();
    SetControls();

    /* focus the skin name textbox */
    SetFocus(txtSkinName);

    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CreateControls(HWND hwndParent)
{
    HINSTANCE instance = GetModuleHandleW(NULL);
    HFONT font = NULL;
    unsigned int i = 0;

    font = GetStockObject(DEFAULT_GUI_FONT);

    for (i = 0; i < (sizeof(child_controls)/sizeof(struct CreateWindowInfo)); i++)
    {
        const struct CreateWindowInfo *pInfo = &child_controls[i];
        HWND hwndControl;

        hwndControl = CreateWindowEx(pInfo->extended_style,
                                     pInfo->className,
                                     pInfo->text,
                                     pInfo->style | WS_VISIBLE | WS_CHILD,
                                     pInfo->x,
                                     pInfo->y,
                                     pInfo->width,
                                     pInfo->height,
                                     hwndParent,
                                     (HMENU) pInfo->menu,
                                     instance,
                                     NULL);

        /* set the correct system font */
        SendMessage(hwndControl, WM_SETFONT, (WPARAM) font, MAKELPARAM(FALSE, 0));

        *(pInfo->pHwnd) = hwndControl;
    }
}

void FillComboBoxBaseSkin(void)
{
    WIN32_FIND_DATAW data;
    HANDLE file;
    LPWSTR filename;
    WCHAR expandedPath[MAX_PATH] = {0};
    WCHAR wildcard[MAX_PATH] = {0};
    WCHAR picdisplayPath[MAX_PATH] = {0};

    ExpandEnvironmentStringsW(THETIS_SKIN_PATH, expandedPath, MAX_PATH);

    /* ??? why does _ make all the difference? */
    _snwprintf(wildcard, MAX_PATH, L"%s\\*", expandedPath);

    file = FindFirstFileW(wildcard, &data);

    /* get all files in the skin folder */
    if(file != INVALID_HANDLE_VALUE)
    {
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

            SendMessage(cboBaseSkin, CB_ADDSTRING, 0, (LPARAM) filename);
        }
        while(FindNextFileW(file, &data));
    }
}

void SetDefaults(void)
{
    SetWindowTextW(txtSkinName, L"");
    SendMessage(cboBaseSkin, CB_SETCURSEL, -1, 0);
    SetWindowTextW(txtFile, L"");
    if (hbmpImage)
    {
        DeleteObject(hbmpImage);
        hbmpImage = NULL;
    }
    SetFocus(txtSkinName);
}

void SetControls(void)
{
    WCHAR fileName[MAX_PATH] = L"";
    BOOL btnPreviewEnabled = FALSE;

    /* check if image file path is set */
    GetWindowTextW(txtFile, fileName, MAX_PATH);
    if (wcscmp(fileName, L"") != 0)
    {
        btnPreviewEnabled = TRUE;
    }

    EnableWindow(btnPreview, btnPreviewEnabled);
    /* disable the save button if no skins found */
    EnableWindow(btnSave, g_thetisSkinsExist);
}

void OnBrowse(HWND hwnd)
{
    OPENFILENAMEW ofn = {0};
    WCHAR fileName[MAX_PATH] = L"";

    ofn.lpstrFilter = L"All Image files\0" "*.png;*.jpg;*.jpeg;*.bmp\0" "All\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = hwnd;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if(GetOpenFileNameW(&ofn))
    {
        /* avoid resource leak by deleting bitmap if a new one is being set */
        if(hbmpImage)
        {
            DeleteObject(hbmpImage);
        }
        hbmpImage = CreateBitmapFromPath(fileName);
        if(hbmpImage)
        {
            /* get basename of filename */
            SetWindowTextW(txtFile, PathFindFileName(fileName));
            SetControls();
        }
        else
        {
            ERROR_BOX(L"Failed to load image!");
        }
    }
}

void OnSave(HWND hwnd)
{
    WCHAR error[256] = {0};
    ThetisSkin skin = {0};

    SetCursor(LoadCursorW(NULL, IDC_WAIT));

    LoadSkin(&skin);

    if (ThetisSkin_Save(&skin, error, sizeof(error)/sizeof(WCHAR)))
    {
        MessageBoxW(NULL, L"Skin successfully saved!\r\nIMPORTANT: Please restart Thetis if it is already open.", L"Skin Saved", MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        MessageBoxW(hwnd, error, L"Error", MB_OK);
    }

    SetCursor(LoadCursorW(NULL, IDC_ARROW));
}

void LoadSkin(ThetisSkin *pSkin)
{
    LPWSTR baseSkin = GetDropDownItem(cboBaseSkin);

    GetWindowTextW(txtSkinName, pSkin->skinName, THETIS_SKIN_NAME_MAX);
    if (baseSkin != NULL)
    {
        wcscpy(pSkin->baseSkin, baseSkin);
        free(baseSkin);
    }
    GetWindowTextW(txtFile, pSkin->filePath, THETIS_SKIN_NAME_MAX);

}

void OnReset(HWND hwnd)
{
    (void) hwnd;

    SetDefaults();
    SetControls();
}
