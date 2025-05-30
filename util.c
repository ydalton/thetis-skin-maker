#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <assert.h>

static const WCHAR WIN32_INVALID_CHARS[] = L"<>:\"/\\|?*";
static const WCHAR* WIN32_RESERVED_NAMES[] = {
    L"CON", L"PRN", L"AUX", L"NUL",
    L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6", L"COM7", L"COM8", L"COM9",
    L"LPT1", L"LPT2", L"LPT3", L"LPT4", L"LPT5", L"LPT6", L"LPT7", L"LPT8", L"LPT9"
};

BOOL FileNameIsValid(LPCWSTR name)
{
    WCHAR baseName[MAX_PATH];
    const WCHAR* p;
    unsigned int i;

    if (!name || !*name || wcslen(name) >= MAX_PATH)
        return FALSE;

    for (p = name; *p; ++p)
    {
        if (wcschr(WIN32_INVALID_CHARS, *p))
            return FALSE;
    }

    _wsplitpath_s(name, NULL, 0, NULL, 0, baseName, MAX_PATH, NULL, 0);
    for (i = 0; i < _countof(WIN32_RESERVED_NAMES); ++i)
    {
        if (_wcsicmp(baseName, WIN32_RESERVED_NAMES[i]) == 0)
            return FALSE;
    }

    return TRUE;
}


int CopyFolderRecursively(LPWSTR src, LPWSTR dest)
{
    SHFILEOPSTRUCTW s = {0};

    s.wFunc = FO_COPY;
    s.fFlags = FOF_SILENT;
    s.pFrom = src;
    s.pTo = dest;

    return SHFileOperationW(&s);
}

LPWSTR GetDropDownItem(HWND hComboBox)
{
    LPWSTR string = NULL;
    int index, length;

    /* get currently selected index */
    index = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

    if(index != CB_ERR)
    {
        /* get length of currently selected item */
        length = SendMessage(hComboBox, CB_GETLBTEXTLEN, index, 0);
        assert(length != CB_ERR);

        string = malloc((length + 1) * sizeof(TCHAR));
        SendMessage(hComboBox, CB_GETLBTEXT, index, (LPARAM) string);
    }

    return string;
}