#include "pennydbg.h"

PennyDbg::PennyDbg(LPDebuggedProcessData pPData, QObject *parent) : QThread(parent)
{
    this->pData = *pPData;
}

void PennyDbg::run() {
    int c;
    switch (pData.GetDbgStartMode()) {
        case Open: {
            emit console_log((QString)"Open Process : "+QString::fromStdWString(pData.GetFullPath()));
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
        CloseHandle(pData.GetProcessHandle());
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

LPDebuggedProcessData PennyDbg::GetPtrToProcessData() {
    return &pData;
}

int PennyDbg::OpenProcess() {
    STARTUPINFO si{ 0 };
    PROCESS_INFORMATION pi{ 0 };
    if (CreateProcessW(pData.GetFullPath().c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &si, &pi)) {
        pData.SetProcessHandle(pi.hProcess);
        pData.SetProcessId(pi.dwProcessId);
        ResumeThread(pi.hThread);
        return PENNY_DBG_SUCCESS;
    }
    return PENNY_DBG_ERROR;
}

int PennyDbg::AttachProcess() {
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, NULL, pData.GetProcessId());
    if (hProcess) {
        if (DebugActiveProcess(pData.GetProcessId())) {
            pData.SetProcessHandle(hProcess);
            return PENNY_DBG_SUCCESS;
        }
    }
    return PENNY_DBG_ERROR;
}

void PennyDbg::AddLoadedDllData(LoadedDllData loadedDllData) {
    pData.AddLoadedDllData(loadedDllData);
    emit loadedDll_GUI_update();
}

void PennyDbg::FillLoadedDllData(LPLoadedDllData lpLoadedDllData) {
    PIMAGE_DOS_HEADER dllDumpDosHeader = (PIMAGE_DOS_HEADER)malloc(sizeof(IMAGE_DOS_HEADER));
    PIMAGE_NT_HEADERS dllDumpNtHeader = (PIMAGE_NT_HEADERS)malloc(sizeof(IMAGE_NT_HEADERS));

    SIZE_T bytesRead = 0;
    BOOL rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), lpLoadedDllData->lpBaseOfDll, dllDumpDosHeader, sizeof(IMAGE_DOS_HEADER), &bytesRead);

    if ((rpmRet == TRUE) && (dllDumpDosHeader->e_magic == IMAGE_DOS_SIGNATURE)) {
        rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), (LPBYTE)lpLoadedDllData->lpBaseOfDll + dllDumpDosHeader->e_lfanew, dllDumpNtHeader, sizeof(IMAGE_NT_HEADERS), &bytesRead);
        if ((rpmRet == TRUE) && (dllDumpNtHeader->Signature == IMAGE_NT_SIGNATURE)) {
            lpLoadedDllData->dllMappedSize = dllDumpNtHeader->OptionalHeader.SizeOfImage;
            std::ifstream module(lpLoadedDllData->fullPath, std::ios::binary);
            module.seekg(0, module.end);
            lpLoadedDllData->dllDiskSize = module.tellg();
            module.close();
        }
    }

    free(dllDumpNtHeader);
    free(dllDumpDosHeader);
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

    LoadedDllData loadedDllData;

    LPCVOID lpModuleFileName = 0;

    SIZE_T bytesRead = 0;
    BOOL rpmRet = TRUE;

    if (pData.GetSeenNTDLLLoad() == FALSE) {
        wchar_t ntdllname[] = L"\\System32\\ntdll.dll";
        UINT dlen = GetWindowsDirectoryW(loadedDllData.fullPath, MAX_PATH);
        memcpy(loadedDllData.fullPath + dlen, ntdllname, sizeof(ntdllname) + 1);
        memcpy(loadedDllData.fullName, ntdllname + 10, sizeof(ntdllname) - 9);
        pData.SetSeenNTDLLLoad();
    }else {
        rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), loadDllDebugInfo->lpImageName, &lpModuleFileName, sizeof(LPCVOID), &bytesRead);

        if ((rpmRet == TRUE) && (lpModuleFileName != 0)) {
            if (loadDllDebugInfo->fUnicode) {
                //UNICODE
                DWORD dwSize = MAX_PATH * sizeof(wchar_t);
                do {
                    rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), lpModuleFileName, &loadedDllData.fullPath, dwSize, &bytesRead);
                    dwSize -= 20;
                } while ((rpmRet == FALSE) && (dwSize > 20));
            }else {
                //ASCII
                char tcName[MAX_PATH];
                rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), lpModuleFileName, &tcName, MAX_PATH, &bytesRead);
                if (rpmRet == TRUE) {
                    mbstowcs_s(&bytesRead, loadedDllData.fullPath, MAX_PATH, tcName, _TRUNCATE);
                }
            }
            int sIndex = 0;
            for (int i = MAX_PATH; i > 0; i--) {
                if (loadedDllData.fullPath[i] == L'\\' || loadedDllData.fullPath[i] == L'/') {
                    sIndex = i;
                    break;
                }
            }
            memcpy(loadedDllData.fullName, loadedDllData.fullPath + sIndex + 1, sizeof(loadedDllData.fullName));
        }else {
            //Todo : Get filename with GetModuleFileNameEx
        }
    }

    loadedDllData.lpBaseOfDll = loadDllDebugInfo->lpBaseOfDll;

    FillLoadedDllData(&loadedDllData);

    AddLoadedDllData(loadedDllData);

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
