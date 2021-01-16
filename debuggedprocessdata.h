#ifndef _DEBUGGEDPROCESSDATA_H
#define _DEBUGGEDPROCESSDATA_H

#include <iostream>
#include <vector>
#include "loadeddlldata.h"
enum DbgStartMode {
    Open,
    Attach
};

typedef struct _DebuggedProcessData
{
public:
    _DebuggedProcessData(std::wstring fullPath);
    _DebuggedProcessData(DWORD dwProcessId);
    _DebuggedProcessData();
    DbgStartMode GetDbgStartMode();
    std::wstring GetFullPath();
    HANDLE GetProcessHandle();
    void SetProcessHandle(HANDLE hProcess);
    DWORD GetProcessId();
    void SetProcessId(DWORD dwProcessId);
    BOOL GetSeenNTDLLLoad();
    void SetSeenNTDLLLoad();
    LPLoadedDllData GetLoadedDllData(int index);
    size_t GetLoadedDllsSize();
    void AddLoadedDllData(LoadedDllData loadedDllData);
private:
    DbgStartMode dbgStartMode;
    std::wstring fullPath;
    HANDLE hProcess;
    DWORD dwProcessid;
    BOOL seenNtDllLoad;
    std::vector<LoadedDllData> loadedDlls;
}DebuggedProcessData, *LPDebuggedProcessData;

#endif // _DEBUGGEDPROCESSDATA_H
