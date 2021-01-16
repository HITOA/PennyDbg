#include "pennydbg.h"
#include <iostream>

PennyDbg::PennyDbg(PPennyDbgStruct dbgStruct, QObject *parent) : QThread(parent)
{
    this->dbgStruct = *dbgStruct;
    needUpdate = FALSE;
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
        CloseHandle(dbgStruct.hProcess);
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

void PennyDbg::on_gui_update() {
    if (needUpdate) {
        //TODO : Update gui
        emit modules_update(dbgStruct.pennyDebuggedProcess.modules);
        needUpdate = FALSE;
    }
}

int PennyDbg::OpenProcess() {
    STARTUPINFO si{ 0 };
    PROCESS_INFORMATION pi{ 0 };
    if (CreateProcessW(dbgStruct.path.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi)) {
        dbgStruct.hProcess = pi.hProcess;
        dbgStruct.dwProcessId = pi.dwProcessId;
        ResumeThread(pi.hThread);
        return PENNY_DBG_SUCCESS;
    }
    return PENNY_DBG_ERROR;
}

int PennyDbg::AttachProcess() {
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, NULL, dbgStruct.dwProcessId);
    if (hProcess) {
        if (DebugActiveProcess(dbgStruct.dwProcessId)) {
            dbgStruct.hProcess = hProcess;
            return PENNY_DBG_SUCCESS;
        }
    }
    return PENNY_DBG_ERROR;
}

void PennyDbg::AddModuleToList(PNMODULE module) {
    dbgStruct.pennyDebuggedProcess.modules.insert({module.moduleBaseAddr,module});
    needUpdate = TRUE;
    //emit console_log(QString("Module : 0x%1 : %2 loaded.").arg((quintptr)module.moduleBaseAddr, QT_POINTER_SIZE * 2, 16,QChar('0')).arg(module.modulePath));
    //emit modules_update(dbgStruct.pennyDebuggedProcess.modules);
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
    LPLOAD_DLL_DEBUG_INFO loadDllDebugInfo = &lpdebugEv->u.LoadDll;

    PNMODULE module;

    LPCVOID lpModuleFileName = 0;

    SIZE_T bytesRead = 0;
    BOOL rpmRet = TRUE;

    if (dbgStruct.pennyDebuggedProcess.ntDllSeen == FALSE) {
        wchar_t ntdllname[] = L"\\System32\\ntdll.dll";
        UINT dlen = GetWindowsDirectoryW(module.modulePath, MAX_PATH);
        memcpy(module.modulePath + dlen, ntdllname, sizeof(ntdllname) + 1);
        module.moduleName = module.modulePath + dlen + 20;
        dbgStruct.pennyDebuggedProcess.ntDllSeen = TRUE;
    }else {
        rpmRet = ::ReadProcessMemory(dbgStruct.hProcess, loadDllDebugInfo->lpImageName, &lpModuleFileName, sizeof(LPCVOID), &bytesRead);

        if ((rpmRet == TRUE) && (lpModuleFileName != 0)) {
            if (loadDllDebugInfo->fUnicode) {
                //UNICODE
                DWORD dwSize = MAX_PATH * sizeof(wchar_t);
                do {
                    rpmRet = ::ReadProcessMemory(dbgStruct.hProcess, lpModuleFileName, &module.modulePath, dwSize, &bytesRead);
                    dwSize -= 20;
                } while ((rpmRet == FALSE) && (dwSize > 20));
            }else {
                //ASCII
                char tcName[MAX_PATH];
                rpmRet = ::ReadProcessMemory(dbgStruct.hProcess, lpModuleFileName, &tcName, MAX_PATH, &bytesRead);
                if (rpmRet == TRUE) {
                    mbstowcs_s(&bytesRead, module.modulePath, MAX_PATH, tcName, _TRUNCATE);
                }
            }
        }else {
            //Todo : Get filename with GetModuleFileNameEx
        }
    }

    module.moduleBaseAddr = loadDllDebugInfo->lpBaseOfDll;

    AddModuleToList(module);

    CloseHandle(loadDllDebugInfo->hFile);
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
