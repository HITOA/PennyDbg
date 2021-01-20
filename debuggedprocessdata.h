#ifndef _DEBUGGEDPROCESSDATA_H
#define _DEBUGGEDPROCESSDATA_H

#include <iostream>
#include <vector>
#include "threaddata.h"
#include "loadeddlldata.h"
#include "processdata.h"
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
    BOOL GetSeenProcessModule();
    void SetSeenProcessModule();
    LPLoadedDllData GetLoadedDllData(int index);
    size_t GetLoadedDllsSize();
    void AddLoadedDllData(LoadedDllData loadedDllData);
    void RemoveLoadedDllData(LPVOID lpBaseOfDll);
    LPThreadData GetThreadData(int index);
    size_t GetThreadsSize();
    void AddThreadData(ThreadData threadData);
    void RemoveThreadData(DWORD dwThreadId);
    LPProcessData GetMainProcessData();
    void SetMainProcessData(ProcessData processData);
private:
    DbgStartMode dbgStartMode;
    std::wstring fullPath;
    HANDLE hProcess;
    DWORD dwProcessid;
    BOOL seenNtDllLoad;
    BOOL seenProcessModule;
    ProcessData mainProcessData;
    std::vector<LoadedDllData> loadedDlls;
    std::vector<ThreadData> threads;
}DebuggedProcessData, *LPDebuggedProcessData;

#endif // _DEBUGGEDPROCESSDATA_H
