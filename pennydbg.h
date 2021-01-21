#ifndef PENNYDBG_H
#define PENNYDBG_H

#define PENNY_DBG_ERROR 0
#define PENNY_DBG_SUCCESS 1

#include <QThread>
#include <windows.h>
#include <fstream>
#include <psapi.h>
#include <DbgHelp.h>
#include "debuggedprocessdata.h"

typedef LONG (__stdcall *NtSuspendProcess)(HANDLE ProcessHandle);
typedef LONG (__stdcall *NtResumeProcess)(HANDLE ProcessHandle);

class PennyDbg : public QThread
{
    Q_OBJECT
public:
    explicit PennyDbg(LPDebuggedProcessData pPData, QObject *parent = nullptr);
    void run() override;
    void exec();
    LPDebuggedProcessData GetPtrToProcessData();
private:
    SYSTEM_INFO systemInfo;
    DebuggedProcessData pData;
    void InitDbg();
    int OpenProcess();
    int AttachProcess();
    void AddLoadedDllData(LoadedDllData loadedDllData);
    void FillLoadedDllData(LPLoadedDllData lpLoadedDllData);
    void AddThreadData(ThreadData threadData);
    std::vector<MEMORY_BASIC_INFORMATION>  EnumVirtualAllocations();
    BOOL CheckMemoryInformationAcess(PMEMORY_BASIC_INFORMATION pMemoryInformation);
    int DumpProcessMemory(std::wstring dumpFileName);
    int DumpModuleMemory(LPVOID address);
    int ScanMemory(char* buffer, size_t bSize);
    //Debug Event Handle
    DWORD OnExceptionDebugEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnCreateThreadDebugEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnCreateProcessDebugEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnExitThreadDebugEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnExitProcessDebugEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnLoadDllDebugEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnUnloadDllDebugEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnOutputDebugStringEvent(LPDEBUG_EVENT lpdebugEv);
    DWORD OnRIPEvent(LPDEBUG_EVENT lpdebugEv);
signals:
    void console_log(QString txt); //send log to the console
    void console_err(QString txt); //send error to the console

    void loadedDll_GUI_update(); //update all modules show on the interface
    void threads_GUI_update(); //update all threads show on the interface

    //void memorydump_receive();
public slots:
    void on_dump_process_memory(std::wstring dumpFileName);
    void on_dump_module_memory(LPVOID address);
    void on_memory_scan(char* buffer, size_t bSize);
};

#endif // PENNYDBG_H
