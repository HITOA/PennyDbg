#include "pennydbg.h"
#include <iostream>

PennyDbg::PennyDbg(PPennyDbgStruct dbgStruct, QObject *parent) : QThread(parent)
{
    this->dbgStruct = *dbgStruct;
}

void PennyDbg::run() {
    int c;
    switch (dbgStruct.startMode) {
        case Open: {
            emit console_log((QString)"Open Process : "+QString::fromStdWString(dbgStruct.path));
            c = OpenProcess();
            break;
        }
        case Attach: {
            c = AttachProcess();
            break;
        }
        default: {
            c = PENNY_DBG_ERROR;
            break;
        }
    }
    if (c) {
        exec();
    }else {
        emit console_err("Error, can't attach or open the process.");
    }
}

void PennyDbg::exec() {
    DEBUG_EVENT debugEv;
    DWORD c = DBG_CONTINUE;
    for(;;) {
        WaitForDebugEvent(&debugEv, INFINITE);
        switch (debugEv.dwDebugEventCode) {
            case EXCEPTION_DEBUG_EVENT: {
                c = OnExceptionDebugEvent(&debugEv);
                break;
            }
            case CREATE_THREAD_DEBUG_EVENT: {
                c = OnCreateThreadDebugEvent(&debugEv);
                break;
            }
            case CREATE_PROCESS_DEBUG_EVENT: {
                c = OnCreateProcessDebugEvent(&debugEv);
                break;
            }
            case EXIT_THREAD_DEBUG_EVENT: {
                c = OnExitThreadDebugEvent(&debugEv);
                break;
            }
            case EXIT_PROCESS_DEBUG_EVENT: {
                c = OnExitProcessDebugEvent(&debugEv);
                break;
            }
            case LOAD_DLL_DEBUG_EVENT: {
                c = OnLoadDllDebugEvent(&debugEv);
                break;
            }
            case UNLOAD_DLL_DEBUG_EVENT: {
                c = OnUnloadDllDebugEvent(&debugEv);
                break;
            }
            case OUTPUT_DEBUG_STRING_EVENT: {
                c = OnOutputDebugStringEvent(&debugEv);
                break;
            }
            case RIP_EVENT: {
                c = OnRIPEvent(&debugEv);
                break;
            }
            default: {
                //TODO : ?
                break;
            }
        }

        ContinueDebugEvent(debugEv.dwProcessId, debugEv.dwThreadId, c);
    }
}

int PennyDbg::OpenProcess() {
    STARTUPINFO si{ 0 };
    PROCESS_INFORMATION pi{ 0 };
    if (CreateProcessW(dbgStruct.path.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi)) {
        dbgStruct.u.hProcess = pi.hProcess;
        dbgStruct.u.dwProcessId = pi.dwProcessId;
        ResumeThread(pi.hThread);
        return PENNY_DBG_SUCCESS;
    }
    return PENNY_DBG_ERROR;
}

int PennyDbg::AttachProcess() {
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, NULL, dbgStruct.u.dwProcessId);
    if (hProcess) {
        if (DebugActiveProcess(dbgStruct.u.dwProcessId)) {
            dbgStruct.u.hProcess = hProcess;
            return PENNY_DBG_SUCCESS;
        }
    }
    return PENNY_DBG_ERROR;
}

DWORD PennyDbg::OnExceptionDebugEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnCreateThreadDebugEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnCreateProcessDebugEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnExitThreadDebugEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnExitProcessDebugEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnLoadDllDebugEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnUnloadDllDebugEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnOutputDebugStringEvent(LPDEBUG_EVENT lpdebugEv){
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnRIPEvent(LPDEBUG_EVENT lpdebugEv) {
    return DBG_CONTINUE;
}
