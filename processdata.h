#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include <windows.h>

typedef struct _ProcessData
{
public:
    wchar_t fullPath[MAX_PATH];
    wchar_t fullName[256];
    LPVOID lpBaseOfProcess;
    size_t processMappedSize; //In bytes
    size_t processDiskSize; //In bytes
}ProcessData, *LPProcessData;

#endif // PROCESSDATA_H
