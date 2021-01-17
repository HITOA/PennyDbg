#ifndef PENNYDBG_H
#define PENNYDBG_H

#define PENNY_DBG_ERROR 0
#define PENNY_DBG_SUCCESS 1

#include <QThread>
#include <windows.h>
#include <fstream>
#include <psapi.h>
#include "debuggedprocessdata.h"

class PennyDbg : public QThread
{
    Q_OBJECT
public:
    explicit PennyDbg(LPDebuggedProcessData pPData, QObject *parent = nullptr);
    void run() override;
    void exec();
    LPDebuggedProcessData GetPtrToProcessData();
private:
    DebuggedProcessData pData;
    int OpenProcess();
    int AttachProcess();
    void AddLoadedDllData(LoadedDllData loadedDllData);
    void FillLoadedDllData(LPLoadedDllData lpLoadedDllData);
    void AddThreadData(ThreadData threadData);
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
};

#endif // PENNYDBG_H
