#ifndef PENNYDBG_H
#define PENNYDBG_H

#define PENNY_DBG_ERROR 0
#define PENNY_DBG_SUCCESS 1

#define PENNY_GUI_UPDATE_TIMER 1000

#include <QThread>
#include <windows.h>

enum DbgStartMode {
    Open,
    Attach
};

typedef struct _PNMODULE {
    wchar_t modulePath[MAX_PATH];
    wchar_t *moduleName;
    LPVOID moduleBaseAddr;
}PNMODULE, *PPNMODULE;

typedef struct _PennyDebuggedProcess {
    BOOL ntDllSeen = FALSE;
    std::map<LPVOID, PNMODULE> modules;
}PennyDebuggedProcess, *PPennyDebuggedProcess;

typedef struct _PennyDbgStruct {
    DbgStartMode startMode;
    std::wstring path;
    HANDLE hProcess;
    DWORD dwProcessId;
    PennyDebuggedProcess pennyDebuggedProcess;
}PennyDbgStruct, *PPennyDbgStruct;

class PennyDbg : public QThread
{
    Q_OBJECT
public:
    explicit PennyDbg(PPennyDbgStruct dbgStruct, QObject *parent = nullptr);
    void run() override;
    void exec();
public slots:
    void on_gui_update();
private:
    PennyDbgStruct dbgStruct;
    BOOL needUpdate;
    int OpenProcess();
    int AttachProcess();
    void AddModuleToList(PNMODULE module);
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
    void modules_update(std::map<LPVOID, PNMODULE> modules); //update all modules show on the interface
};

#endif // PENNYDBG_H
