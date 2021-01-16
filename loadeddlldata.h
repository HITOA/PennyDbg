#ifndef _LOADEDDLLDATA_H
#define _LOADEDDLLDATA_H

#include <windows.h>

typedef struct _LoadedDllData
{
public:
    wchar_t fullPath[MAX_PATH];
    wchar_t *fullName;
    LPVOID lpBaseOfDll;
    size_t dllSize;
}LoadedDllData, *LPLoadedDllData;

#endif // _LOADEDDLLDATA_H
