#ifndef PENNYDBG_H
#define PENNYDBG_H

#define PENNY_DBG_ERROR 0
#define PENNY_DBG_SUCCESS 1

#include <QThread>
#include <windows.h>

enum DbgStartMode {
    Open,
    Attach
};

typedef struct _PennyDbgStruct {
    DbgStartMode startMode;
    std::wstring path;
    union {
        HANDLE hProcess;
        DWORD dwProcessId;
    } u;
}PennyDbgStruct, *PPennyDbgStruct;

class PennyDbg : public QThread
{
    Q_OBJECT
public:
    explicit PennyDbg(PPennyDbgStruct dbgStruct, QObject *parent = nullptr);
    void run() override;
    void exec();
private:
    PennyDbgStruct dbgStruct;
    int OpenProcess();
    int AttachProcess();
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
};

#endif // PENNYDBG_H
