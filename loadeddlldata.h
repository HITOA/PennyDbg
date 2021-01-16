#ifndef _LOADEDDLLDATA_H
#define _LOADEDDLLDATA_H

#include <windows.h>

typedef struct _LoadedDllData
{
public:
    wchar_t fullPath[MAX_PATH];
    wchar_t fullName[256];
    LPVOID lpBaseOfDll;
    size_t dllMappedSize; //In bytes
    size_t dllDiskSize; //In bytes
}LoadedDllData, *LPLoadedDllData;

#endif // _LOADEDDLLDATA_H
