#include "debuggedprocessdata.h"

_DebuggedProcessData::_DebuggedProcessData(std::wstring fullPath)
{
    this->dbgStartMode = Open;
    this->fullPath = fullPath;
    this->seenNtDllLoad = FALSE;
}

_DebuggedProcessData::_DebuggedProcessData(DWORD dwProcessId)
{
    this->dbgStartMode = Attach;
    this->dwProcessid = dwProcessId;
}

_DebuggedProcessData::_DebuggedProcessData()
{
    this->seenNtDllLoad = FALSE;
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

LPLoadedDllData _DebuggedProcessData::GetLoadedDllData(int index) {
    return &loadedDlls.at(index);
}

size_t _DebuggedProcessData::GetLoadedDllsSize() {
    return loadedDlls.size();
}

void _DebuggedProcessData::AddLoadedDllData(LoadedDllData loadedDllData) {
    loadedDlls.push_back(loadedDllData);
}
