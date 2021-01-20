#include "debuggedprocessdata.h"

_DebuggedProcessData::_DebuggedProcessData(std::wstring fullPath)
{
    this->dbgStartMode = Open;
    this->fullPath = fullPath;
    this->seenNtDllLoad = FALSE;
    this->seenProcessModule = FALSE;
}

_DebuggedProcessData::_DebuggedProcessData(DWORD dwProcessId)
{
    this->dbgStartMode = Attach;
    this->dwProcessid = dwProcessId;
    this->seenNtDllLoad = FALSE;
    this->seenProcessModule = FALSE;
}

_DebuggedProcessData::_DebuggedProcessData()
{
    this->seenNtDllLoad = FALSE;
    this->seenProcessModule = FALSE;
}

DbgStartMode _DebuggedProcessData::GetDbgStartMode() {
    return this->dbgStartMode;
}

std::wstring _DebuggedProcessData::GetFullPath() {
    return this->fullPath;
}

HANDLE _DebuggedProcessData::GetProcessHandle() {
    return this->hProcess;
}

void _DebuggedProcessData::SetProcessHandle(HANDLE hProcess) {
    this->hProcess = hProcess;
}

DWORD _DebuggedProcessData::GetProcessId() {
    return this->dwProcessid;
}

void _DebuggedProcessData::SetProcessId(DWORD dwProcessId) {
    this->dwProcessid = dwProcessId;
}

BOOL _DebuggedProcessData::GetSeenNTDLLLoad() {
    return this->seenNtDllLoad;
}

void _DebuggedProcessData::SetSeenNTDLLLoad() {
    this->seenNtDllLoad = TRUE;
}

BOOL _DebuggedProcessData::GetSeenProcessModule() {
    return this->seenProcessModule;
}

void _DebuggedProcessData::SetSeenProcessModule() {
    this->seenProcessModule = TRUE;
}

LPLoadedDllData _DebuggedProcessData::GetLoadedDllData(int index) {
    return &loadedDlls.at(index);
}

size_t _DebuggedProcessData::GetLoadedDllsSize() {
    return loadedDlls.size();
}

void _DebuggedProcessData::AddLoadedDllData(LoadedDllData loadedDllData) {
    loadedDlls.push_back(loadedDllData);
}

void _DebuggedProcessData::RemoveLoadedDllData(LPVOID lpBaseOfDll) {
    for (size_t i = 0; i < loadedDlls.size(); i++) {
        if (loadedDlls.at(i).lpBaseOfDll == lpBaseOfDll) {
            loadedDlls.erase(loadedDlls.begin() + i);
            break;
        }
    }
}


LPThreadData _DebuggedProcessData::GetThreadData(int index) {
    return &threads.at(index);
}

size_t _DebuggedProcessData::GetThreadsSize() {
    return threads.size();
}

void _DebuggedProcessData::AddThreadData(ThreadData threadData) {
    threads.push_back(threadData);
}

void _DebuggedProcessData::RemoveThreadData(DWORD dwThreadId) {
    for (size_t i = 0; i < threads.size(); i++) {
        if (threads.at(i).dwThreadId == dwThreadId) {
            threads.erase(threads.begin() + i);
            break;
        }
    }
}

LPProcessData _DebuggedProcessData::GetMainProcessData() {
    return &mainProcessData;
}

void _DebuggedProcessData::SetMainProcessData(ProcessData processData) {
    this->mainProcessData = processData;
}
