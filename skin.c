#include <fileapi.h>
#include <handleapi.h>
#include <winnt.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <shlwapi.h>
#include <shellapi.h>
#include "thetisskinmaker.h"

int CopyFolderRecursively(LPWSTR src, LPWSTR dest);
BOOL FileNameIsValid(LPWSTR name);

BOOL ThetisSkin_IsValid(ThetisSkin *pSkin, WCHAR *error, int max)
{
    BOOL isValid = FALSE;

    if (wcscmp(pSkin->skinName, L"") == 0)
    {
        wcscat_s(error, max, L"- Please enter a skin name.\r\n");
    }
    else
    {
        WCHAR expanded[MAX_PATH];
        WCHAR destPath[MAX_PATH];

        ExpandEnvironmentStringsW(THETIS_SKIN_PATH, expanded, MAX_PATH);
        _snwprintf(destPath, MAX_PATH, L"%s\\%s", expanded, pSkin->skinName);

        if (PathFileExistsW(destPath))
        {
            wcscat_s(error, max, L"- A skin with this skin name exists already.\r\n");
        }
        else
        {
            if (!FileNameIsValid(pSkin->skinName))
            {
                wcscat_s(error, max, L"- Please enter a valid skin name.\r\n");
            }
        }
    }

    if (wcscmp(pSkin->baseSkin, L"") == 0)
    {
        wcscat_s(error, max, L"- Please choose a base skin.\r\n");
    }

    if (wcscmp(pSkin->filePath, L"") == 0)
    {
        wcscat_s(error, max, L"- Please choose a background image.\r\n");
    }

    isValid = wcslen(error) == 0;

    return isValid;
}

BOOL FileNameIsValid(LPWSTR name)
{
    BOOL isValid = FALSE;

    HANDLE file = CreateFileW(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);

    if (file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(file);
        isValid = TRUE;
    }
    else
    {
        assert(GetLastError() == ERROR_INVALID_NAME);
    }

    return isValid;
}

BOOL ThetisSkin_Save(ThetisSkin *pSkin, WCHAR *error, int max)
{
    BOOL succeeded = FALSE;

    if (ThetisSkin_IsValid(pSkin, error, max))
    {
        WCHAR expanded[MAX_PATH];
        WCHAR srcPath[MAX_PATH];
        WCHAR destPath[MAX_PATH];
        WCHAR picDisplayPath[MAX_PATH];
        BOOL copySucceeded = FALSE;

        ExpandEnvironmentStringsW(THETIS_SKIN_PATH, expanded, MAX_PATH);
        _snwprintf(srcPath, MAX_PATH, L"%s\\%s", expanded, pSkin->baseSkin);
        _snwprintf(destPath, MAX_PATH, L"%s\\%s", expanded, pSkin->skinName);
        _snwprintf(picDisplayPath, MAX_PATH, L"%s\\%s", destPath, THETIS_PICDISPLAY_PATH);

        copySucceeded = CopyFolderRecursively(srcPath, destPath);
        if (copySucceeded)
        {
            if (hbmpImage != NULL)
            {
                SaveBitmapToFile(hbmpImage, picDisplayPath);
                succeeded = TRUE;
            }
        }
        else
        {
            wcscat_s(error, max, L"Failed to copy folder for skin.");
        }
    }

    return succeeded;
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
