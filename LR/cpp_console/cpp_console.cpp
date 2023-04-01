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



DWORD WINAPI body(LPVOID lpParam);


class EventManager
{
public:

    EventManager()
    {
        mutex = CreateMutex(NULL, FALSE, NULL);
        threadConfirmSmthEvent = CreateEvent(NULL, FALSE, FALSE, "ConfirmThread");
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


class SimpleThread
{
public:

    SimpleThread(const EventManager& manager, const int threadIndex)
        : hMutex(manager.GetMutexHandle()), hConfirmEvent(manager.GetConfirmEventHandle()), hUniqueEvent(manager.GetLastUniqueEvent()), i(threadIndex) 
    { 
        thisThread = CreateThread(NULL, 0, body, (LPVOID*)this, 0, NULL);
    }


    HANDLE GetThreadHandle()
    {
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


    void SyncConsoleWrite(std::string text)
    {
        WaitForSingleObject(GetMutexHandle(), INFINITE);
        std::cout << text << std::endl;
        ReleaseMutex(GetMutexHandle());
    }


private:
    HANDLE hMutex;
    HANDLE hConfirmEvent;
    HANDLE hUniqueEvent;

    const int i;

    HANDLE thisThread;

};


class MainThread
{
public:

    MainThread() 
    {
        std::cout << "Main thread" << std::endl;
    }

    void start()
    {
        while (true)
        {
            DWORD dwWaitResult = WaitForMultipleObjects(3, hExtermalEvents, FALSE, INFINITE);

            switch (dwWaitResult)
            {
            case WAIT_OBJECT_0:
                StartEventHandler();
                ResetEvent(hExtermalEvents[0]);
                break;

            case WAIT_OBJECT_0 + 1:
                StopEventHandler();
                ResetEvent(hExtermalEvents[1]);
                break;

            case WAIT_OBJECT_0 + 2:
                ResetEvent(hExtermalEvents[2]);
                End();
                break;

            default:
                break;
            }

            SetEvent(hConfirmExOpEvent);
        }
    }


    void StartEventHandler()
    {
        int ind = optionalThreads.size() + 1;
        HANDLE ev = CreateEvent(NULL, FALSE, FALSE, std::to_string(ind).c_str());
        manager.AddUniqueEvent(ev);

        optionalThreads.push(SimpleThread(manager, ind));

        SetEvent(ev);

        WaitForSingleObject(manager.GetConfirmEventHandle(), INFINITE);
    }


    void StopEventHandler()
    {
        SetEvent(manager.GetLastUniqueEvent());
        WaitForSingleObject(manager.GetConfirmEventHandle(), INFINITE);

        CloseLastOptionalThread();
        manager.CloseLastUniqueEvent();
    }


    void End()
    {
        for (int i = optionalThreads.size(); i > 0; --i)
        {
            StopEventHandler();
        }

        ExitProcess(1);
    }


    void CloseLastOptionalThread()
    {
        CloseHandle(optionalThreads.top().GetThreadHandle());
        optionalThreads.pop();
    }


private:
    EventManager manager = EventManager();

    std::stack<SimpleThread> optionalThreads;
    
    HANDLE hExtermalEvents[3] = { CreateEvent(NULL, TRUE, FALSE, "Start"),
                                  CreateEvent(NULL, TRUE, FALSE, "Stop"),
                                  CreateEvent(NULL, TRUE, FALSE, "Close") };

    HANDLE hConfirmExOpEvent = CreateEvent(NULL, FALSE, FALSE, "Confirm");
};


DWORD WINAPI body(LPVOID lpParam)
{
    SimpleThread Thread = *static_cast<SimpleThread*>(lpParam);


    WaitForSingleObject(Thread.GetUniqueEventHandle(), INFINITE);
    
    Thread.SyncConsoleWrite("start thread " + std::to_string(Thread.GetIndex()));

    SetEvent(Thread.GetConfirmEventHandle());


    WaitForSingleObject(Thread.GetUniqueEventHandle(), INFINITE);
    
    Thread.SyncConsoleWrite("stop thread " + std::to_string(Thread.GetIndex()));

    SetEvent(Thread.GetConfirmEventHandle());

    
    return 0;
}


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
            mt.start();
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
