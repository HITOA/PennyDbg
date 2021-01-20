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
        InitDbg();
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

void PennyDbg::InitDbg() {
    GetSystemInfo(&systemInfo);
    //memset(pDump, 0, (SIZE_T)systemInfo.lpMaximumApplicationAddress);
}

LPDebuggedProcessData PennyDbg::GetPtrToProcessData() {
    return &pData;
}

int PennyDbg::OpenProcess() {
    STARTUPINFO si{ 0 };
    PROCESS_INFORMATION pi{ 0 };
    if (CreateProcessW(pData.GetFullPath().c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED | DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS | PROCESS_QUERY_INFORMATION , NULL, NULL, &si, &pi)) {
        pData.SetProcessHandle(pi.hProcess);
        pData.SetProcessId(pi.dwProcessId);
        ResumeThread(pi.hThread);
        return PENNY_DBG_SUCCESS;
    }
    return PENNY_DBG_ERROR;
}

int PennyDbg::AttachProcess() {
    HANDLE hProcess = ::OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, NULL, pData.GetProcessId());
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

void PennyDbg::AddThreadData(ThreadData threadData) {
    pData.AddThreadData(threadData);
    emit threads_GUI_update();
}

std::vector<MEMORY_BASIC_INFORMATION> PennyDbg::EnumVirtualAllocations() {
    char* p = 0;
    std::vector<MEMORY_BASIC_INFORMATION> memoryInformations;

    MEMORY_BASIC_INFORMATION memoryInformation;

    while (p < systemInfo.lpMaximumApplicationAddress && ::VirtualQueryEx(pData.GetProcessHandle(), p, &memoryInformation, sizeof(memoryInformation))) {
        memoryInformations.push_back(memoryInformation);
        p += memoryInformation.RegionSize;
    }

    return memoryInformations;
}

BOOL PennyDbg::CheckMemoryInformationAcess(PMEMORY_BASIC_INFORMATION pMemoryInformation) {
    if (pMemoryInformation->AllocationProtect == 0) {
        return FALSE;
    }
    if (pMemoryInformation->Protect == PAGE_NOACCESS) {
        return FALSE;
    }
    if (pMemoryInformation->State != MEM_COMMIT) {
        return FALSE;
    }
    return TRUE;
}

int PennyDbg::DumpProcessMemory(std::wstring dumpFileName) {


    std::vector<MEMORY_BASIC_INFORMATION> memoryInformations = EnumVirtualAllocations();

    std::ofstream outF(dumpFileName, std::ios::trunc | std::ios::out | std::ios::binary);

    if (!outF.is_open()) {
        return PENNY_DBG_ERROR;
    }

    SIZE_T bytesRead;

    for (MEMORY_BASIC_INFORMATION memoryInformation: memoryInformations) {
        if (CheckMemoryInformationAcess(&memoryInformation)) {
            char* buffer = new char[memoryInformation.RegionSize];
            if (::ReadProcessMemory(pData.GetProcessHandle(), memoryInformation.BaseAddress, buffer, memoryInformation.RegionSize, &bytesRead)) {
                outF.write(buffer, bytesRead);
            }
        }
    }

    outF.close();

    return PENNY_DBG_SUCCESS;
}

int PennyDbg::DumpModuleMemory(LPVOID address) {
    MEMORY_BASIC_INFORMATION memoryInformation;

    ::VirtualQueryEx(pData.GetProcessHandle(), address, &memoryInformation, sizeof(memoryInformation));

    std::ofstream outF("E:/Library/Desktop/XjMLserver/payload/ChessRPG/module.dump", std::ios::trunc | std::ios::out | std::ios::binary);

    SIZE_T bytesRead;

    char* buffer = new char[memoryInformation.RegionSize];
    if (::ReadProcessMemory(pData.GetProcessHandle(), memoryInformation.BaseAddress, buffer, memoryInformation.RegionSize, &bytesRead)) {
        outF.write(buffer, bytesRead);
    }

    std::cout << "size : " << memoryInformation.RegionSize << std::endl;

    outF.close();
    return PENNY_DBG_SUCCESS;
}

void PennyDbg::on_dump_process_memory(std::wstring dumpFileName) {
    DumpProcessMemory(dumpFileName);
}

void PennyDbg::on_dump_module_memory(LPVOID address) {
    DumpModuleMemory(address);
}

DWORD PennyDbg::OnExceptionDebugEvent(LPDEBUG_EVENT lpdebugEv){
    std::cout << "Exception" << std::endl;
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnCreateThreadDebugEvent(LPDEBUG_EVENT lpdebugEv){
    LPCREATE_THREAD_DEBUG_INFO createThreadDebugInfo = &lpdebugEv->u.CreateThread;
    ThreadData threadData;
    threadData.hThread = createThreadDebugInfo->hThread;

    DWORD threadId = GetThreadId(createThreadDebugInfo->hThread);
    int threadPriority = GetThreadPriority(createThreadDebugInfo->hThread);

    threadData.dwThreadId = threadId;
    threadData.priority = threadPriority;
    threadData.lpStartAdress = createThreadDebugInfo->lpStartAddress;

    AddThreadData(threadData);

    return DBG_CONTINUE;
}

DWORD PennyDbg::OnCreateProcessDebugEvent(LPDEBUG_EVENT lpdebugEv){
    LPCREATE_PROCESS_DEBUG_INFO createProcessDebugInfo = &lpdebugEv->u.CreateProcessInfo;

    ProcessData processData;

    LPCVOID lpModuleFileName = 0;

    SIZE_T bytesRead = 0;
    BOOL rpmRet = TRUE;

    rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), createProcessDebugInfo->lpImageName, &lpModuleFileName, sizeof(LPCVOID), &bytesRead);

    if (pData.GetSeenProcessModule() == FALSE) {
        GetModuleFileNameExW(createProcessDebugInfo->hProcess, NULL, processData.fullPath, MAX_PATH);

        int sIndex = 0;
        for (int i = MAX_PATH; i > 0; i--) {
            if (processData.fullPath[i] == L'\\' || processData.fullPath[i] == L'/') {
                sIndex = i;
                break;
            }
        }
        memcpy(processData.fullName, processData.fullPath + sIndex + 1, sizeof(processData.fullName));

        pData.SetMainProcessData(processData);

        CloseHandle(createProcessDebugInfo->hFile);
        return DBG_CONTINUE;
    }else {
        if ((rpmRet == TRUE) && (lpModuleFileName != 0)) {
            if (createProcessDebugInfo->fUnicode) {
                //UNICODE
                DWORD dwSize = MAX_PATH * sizeof(wchar_t);
                do {
                    rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), lpModuleFileName, &processData.fullPath, dwSize, &bytesRead);
                    dwSize -= 20;
                } while ((rpmRet == FALSE) && (dwSize > 20));
            }else {
                //ASCII
                char tcName[MAX_PATH];
                rpmRet = ::ReadProcessMemory(pData.GetProcessHandle(), lpModuleFileName, &tcName, MAX_PATH, &bytesRead);
                if (rpmRet == TRUE) {
                    mbstowcs_s(&bytesRead, processData.fullPath, MAX_PATH, tcName, _TRUNCATE);
                }
            }
            int sIndex = 0;
            for (int i = MAX_PATH; i > 0; i--) {
                if (processData.fullPath[i] == L'\\' || processData.fullPath[i] == L'/') {
                    sIndex = i;
                    break;
                }
            }
            memcpy(processData.fullName, processData.fullPath + sIndex + 1, sizeof(processData.fullName));
        }else {
            //Todo : Get filename with GetModuleFileNameEx
        }
    }

    std::wcout << processData.fullName << std::endl;

    CloseHandle(createProcessDebugInfo->hFile);

    return DBG_CONTINUE;
}

DWORD PennyDbg::OnExitThreadDebugEvent(LPDEBUG_EVENT lpdebugEv){
    pData.RemoveThreadData(lpdebugEv->dwThreadId);
    emit threads_GUI_update();
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnExitProcessDebugEvent(LPDEBUG_EVENT lpdebugEv){
    if (lpdebugEv->dwProcessId == pData.GetProcessId()) {
        emit console_log("Process exit.");
    }
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
    pData.RemoveLoadedDllData(lpdebugEv->u.UnloadDll.lpBaseOfDll);
    emit loadedDll_GUI_update();
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnOutputDebugStringEvent(LPDEBUG_EVENT lpdebugEv){
    std::cout << "Debug" << std::endl;
    return DBG_CONTINUE;
}

DWORD PennyDbg::OnRIPEvent(LPDEBUG_EVENT lpdebugEv) {
    std::cout << "Rip Event" << std::endl;
    return DBG_CONTINUE;
}
