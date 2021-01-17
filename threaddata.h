#ifndef THREADDATA_H
#define THREADDATA_H

#include <windows.h>

typedef struct _ThreadData
{
public:
    HANDLE hThread;
    LPTHREAD_START_ROUTINE lpStartAdress;
    DWORD dwThreadId;
    int priority;
}ThreadData, *LPThreadData;

#endif // THREADDATA_H
