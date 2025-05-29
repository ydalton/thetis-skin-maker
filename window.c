#include <stdint.h>
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

struct CreateWindowInfo
{
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

static struct CreateWindowInfo child_controls[] = {
    /* class        text                 style                                                    x    y    width  height hmenu,                hwnd           */
    { WC_STATICW,   L"Skin name: ",      0,                                                       12,  12,  80,    13,    0,                    &lblSkinName   },
    { WC_EDITW,     NULL,                WS_BORDER | WS_TABSTOP,                                  77,  9,   201,   20,    0,                    &txtSkinName   },
    { WC_COMBOBOXW, NULL,                CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL | WS_TABSTOP, 77,  35,  201,   21,    0,                    &cboBaseSkin   },
    { WC_STATICW,   L"Base skin: ",      0,                                                       12,  38,  56,    13,    0,                    &lblBaseSkin   },
    { WC_BUTTONW,   L"Background image", BS_GROUPBOX,                                             15,  62,  264,   78,    0,                    &grpBackground },
    { WC_BUTTONW,   L"Browse...",        BS_PUSHBUTTON | WS_TABSTOP,                              198, 78,  75,    23,    IDC_BROWSE_BUTTON,    &btnBrowse     },
    { WC_EDITW,     NULL,                WS_BORDER | ES_READONLY,                                 77,  80,  114,   20,    0,                    &txtFile       },
    { WC_BUTTONW,   L"Preview",          BS_PUSHBUTTON | WS_TABSTOP,                              198, 107, 75,    23,    IDC_PREVIEW_BUTTON,   &btnPreview    },
    { WC_BUTTONW,   L"Reset",            BS_PUSHBUTTON | WS_TABSTOP,                              15,  150, 75,    23,    IDC_RESET_BUTTON,     &btnReset      },
    { WC_BUTTONW,   L"Save",             BS_DEFPUSHBUTTON | WS_TABSTOP,                           204, 150, 75,    23,    IDC_SAVE_BUTTON,      &btnSave       },
    { WC_STATICW,   L"File: ",           0,                                                       21,  83,  26,    13,    0,                    &lblFile       },
};

void CreateControls(HWND hwndParent);
void SetDefaults(void);
void SetControls(void);
void LoadSkin(ThetisSkin *pSkin);

void OnCreate(HWND hwndMain)
{
    CreateControls(hwndMain);
    SetDefaults();
    SetControls();

    /* focus the skin name textbox */
    SetFocus(txtSkinName);
}

void CreateControls(HWND hwndParent)
{
    HINSTANCE instance = GetModuleHandleW(NULL);
    HFONT font = NULL;
    unsigned int i = 0;

    font = GetStockObject(DEFAULT_GUI_FONT);

    for (i = 0; i < (sizeof(child_controls)/sizeof(struct CreateWindowInfo)); i++)
    {
        struct CreateWindowInfo *pInfo = &child_controls[i];
        HWND hwndControl;

        hwndControl = CreateWindowEx(0,
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

void SetDefaults(void)
{
    SetWindowTextW(txtSkinName, L"");
    SetWindowTextW(txtFile, L"");
    SetFocus(txtSkinName);
}

void SetControls(void)
{
    WCHAR fileName[MAX_PATH] = L"";
    BOOL btnPreviewEnabled = FALSE;

    GetWindowTextW(txtFile, fileName, MAX_PATH);

    if (wcscmp(fileName, L"") != 0)
    {
        btnPreviewEnabled = TRUE;
    }

    EnableWindow(btnPreview, btnPreviewEnabled);
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
            SetWindowTextW(txtFile, fileName);
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
    GetWindowTextW(txtSkinName, pSkin->skinName, THETIS_SKIN_NAME_MAX);
    GetWindowTextW(cboBaseSkin, pSkin->baseSkin, MAX_PATH);
    GetWindowTextW(txtFile, pSkin->filePath, THETIS_SKIN_NAME_MAX);
}

void OnReset(HWND hwnd)
{
    (void) hwnd;

    SetDefaults();
}
