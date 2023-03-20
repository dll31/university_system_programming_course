// cpp_console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "cpp_console.h"

#include <stack>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif






class EventManager
{
public:

    EventManager()
    {
        mutex = CreateMutex(NULL, FALSE, NULL);
        threadConfirmSmthEvent = CreateEvent(NULL, FALSE, FALSE, "OutEvent");
    }


    HANDLE GetMutexHandle() const
    {
        return mutex;
    }


    HANDLE GetConfirmEventHandle() const
    {
        return threadConfirmSmthEvent;
    }


    HANDLE GetLastUniqueEvent() const
    {
        return threadsUniqueEvents.top();
    }


    void AddUniqueEvent(const HANDLE& event)
    {
        threadsUniqueEvents.push(event);
    }


    void CloseLastUniqueEvent()
    {
        CloseHandle(threadsUniqueEvents.top());
        threadsUniqueEvents.pop();
    }


private:
    HANDLE mutex;
    HANDLE threadConfirmSmthEvent;

    std::stack<HANDLE> threadsUniqueEvents;

};



DWORD WINAPI body(LPVOID lpParam)
{
    SimpleThread* Thread = static_cast<SimpleThread*>(lpParam);

    WaitForSingleObject(Thread->GetUniqueEventHandle(), INFINITE);
    
    WaitForSingleObject(Thread->GetMutexHandle(), INFINITE);
    std::cout << "start thread " << Thread->GetIndex() << std::endl;
    ReleaseMutex(Thread->GetMutexHandle());

    SetEvent(Thread->GetConfirmEventHandle());


    WaitForSingleObject(Thread->GetUniqueEventHandle(), INFINITE);
    
    WaitForSingleObject(Thread->GetMutexHandle(), INFINITE);
    std::cout << "stop thread " << Thread->GetIndex() << std::endl;
    ReleaseMutex(Thread->GetMutexHandle());

    SetEvent(Thread->GetConfirmEventHandle());

    return 0;
}


class MainThread
{
public:

    MainThread() { }


    void StartEventCatcher()
    {
        WaitForSingleObject(hExtermalEvents[0], INFINITE);

        WaitForSingleObject(hCatchersSyncMutex, INFINITE);
        

        int ind = optionalThreads.size() + 1;
        HANDLE ev = CreateEvent(NULL, FALSE, FALSE, std::to_string(ind).c_str());
        manager.AddUniqueEvent(ev);

        optionalThreads.push(SimpleThread(manager, ind).CreateSimpleThread());

        SetEvent(ev);
        

        WaitForSingleObject(manager.GetConfirmEventHandle(), INFINITE);
        SetEvent(hConfirmExOpEvent);

        ReleaseMutex(hCatchersSyncMutex);
    }


    void StopEventCatcher()
    {
        WaitForSingleObject(hExtermalEvents[1], INFINITE);
        WaitForSingleObject(hCatchersSyncMutex, INFINITE);


        SetEvent(manager.GetLastUniqueEvent());
        WaitForSingleObject(manager.GetConfirmEventHandle(), INFINITE);

        CloseLastOptionalThread();
        manager.CloseLastUniqueEvent();


        SetEvent(hConfirmExOpEvent);
        ReleaseMutex(hCatchersSyncMutex);
    }


    void CloseEventCatcher()
    {
        WaitForSingleObject(hExtermalEvents[2], INFINITE);
        WaitForSingleObject(hCatchersSyncMutex, INFINITE);


        ExitProcess(1);
    }


    void CloseLastOptionalThread()
    {
        CloseHandle(optionalThreads.top());
        optionalThreads.pop();
    }


private:
    EventManager manager = EventManager();

    const HANDLE hCatchersSyncMutex = CreateMutex(NULL, FALSE, NULL);

    std::stack<HANDLE> optionalThreads;

    HANDLE hExtermalEvents[3] = { CreateEvent(NULL, FALSE, FALSE, "Start"),
                                  CreateEvent(NULL, FALSE, FALSE, "Stop"),
                                  CreateEvent(NULL, FALSE, FALSE, "Close") };

    HANDLE hConfirmExOpEvent = CreateEvent(NULL, FALSE, FALSE, "ConfirmExOp");
};


class SimpleThread
{
public:

    SimpleThread(const EventManager& manager, const int& threadIndex)
        : hMutex(manager.GetMutexHandle()), hConfirmEvent(manager.GetConfirmEventHandle()), hUniqueEvent(manager.GetLastUniqueEvent()), i(threadIndex) {}


    HANDLE CreateSimpleThread()
    {
        thisThread = CreateThread(NULL, 0, body, (LPVOID*)this, 0, NULL);
        return thisThread;
    }


    HANDLE GetMutexHandle() const
    {
        return hMutex;
    }


    HANDLE GetConfirmEventHandle() const
    {
        return hConfirmEvent;
    }


    HANDLE GetUniqueEventHandle() const
    {
        return hUniqueEvent;
    }
    

    int GetIndex() const
    {
        return i;
    }


private:
    HANDLE hMutex;
    HANDLE hConfirmEvent;
    HANDLE hUniqueEvent;

    int i = 0;

    HANDLE thisThread;

};


// The one and only application object

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: code your application's behavior here.
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            MainThread mt = MainThread();
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}
