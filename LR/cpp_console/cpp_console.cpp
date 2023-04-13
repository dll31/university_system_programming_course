// cpp_console.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "cpp_console.h"

#include <stack>
#include <string>

#include "../MemMappedFileFunctions/MemMappedFileFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


UINT body(LPVOID lpParam);


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


    HANDLE GetLastCommandUniqueEvent() const
    {
        return threadsUniqueCommandEvents.top();
    }


    HANDLE GetLastFileUniqueEvent() const
    {
        return threadsUniqueFileEvents.top();
    }


    void AddUniqueEvents(const HANDLE& commandEvent, const HANDLE& fileEvent)
    {
        threadsUniqueCommandEvents.push(commandEvent);
        threadsUniqueFileEvents.push(fileEvent);
    }


    void CloseLastUniqueEvent()
    {
        CloseHandle(threadsUniqueCommandEvents.top());
        CloseHandle(threadsUniqueFileEvents.top());
        threadsUniqueCommandEvents.pop();
        threadsUniqueFileEvents.pop();
    }


private:
    HANDLE mutex;
    HANDLE threadConfirmSmthEvent;

    std::stack<HANDLE> threadsUniqueCommandEvents;
    std::stack<HANDLE> threadsUniqueFileEvents;

};


class DllLoader
{
public:
    HINSTANCE hInstDll;
    typedef receiveHeader(__stdcall* MAPRECEIVE)();
    MAPRECEIVE mapreceive;

    int Init()
    {
        if (!dllLoading() && !dllFuncLoader())
            return 0;

        return 1;
    }

private:
    int dllLoading()
    {
        hInstDll = LoadLibrary(_T("MemMappedFileFunctions.dll"));
        if (hInstDll == NULL)
        {
            std::cout << "Troubles with loading dll" << std::endl;
            return 1;
        }

        return 0;
    }

    int dllFuncLoader()
    {
        mapreceive = (MAPRECEIVE)GetProcAddress(hInstDll, "mapreceive");
        if (mapreceive == NULL)
        {
            std::cout << "Broaken dll function loading" << std::endl;
            return 2;
        }

        return 0;
    }
};


class FileManager
{
public:

    DllLoader fileDll = DllLoader();


    FileManager()
    {
        status = fileDll.Init();
        hSharedFileMutex = CreateMutex(NULL, FALSE, NULL);
    }


    ~FileManager()
    {
        FreeLibrary(fileDll.hInstDll);
    }

    int getStatus() { return status; }


    HANDLE getMutexHandle() const { return hSharedFileMutex; }


private:
    int status;
    HANDLE hSharedFileMutex;
};


class SimpleThread
{
public:

    SimpleThread(const EventManager& eManager, const FileManager& fManager, const int threadIndex)
        : hConfirmEvent(eManager.GetConfirmEventHandle()), fileDll(fManager.fileDll), muts(eManager.GetMutexHandle(), fManager.getMutexHandle()), 
        hUniqueCommandEvent(eManager.GetLastCommandUniqueEvent()), hUniqueFileEvent(eManager.GetLastFileUniqueEvent()), i(threadIndex)
    { 
        thisThread = AfxBeginThread(body, (LPVOID*)this);
    }


    HANDLE GetThreadHandle() { return thisThread; }


    HANDLE GetCoutMutexHandle() const
    {
        return muts.cout;
    }


    HANDLE GetFileMutexHandle() const
    {
        return muts.file;
    }


    HANDLE GetConfirmEventHandle() const
    {
        return hConfirmEvent;
    }


    HANDLE GetCommandUniqueEventHandle() const
    {
        return hUniqueCommandEvent;
    }


    HANDLE GetFileUniqueEventHandle() const
    {
        return hUniqueFileEvent;
    }


    int GetIndex() const
    {
        return i;
    }


    void SyncConsoleWrite(std::string text)
    {
        WaitForSingleObject(GetCoutMutexHandle(), INFINITE);
        std::cout << text << std::endl;
        ReleaseMutex(GetCoutMutexHandle());
    }


    void SyncSharedFileRead(receiveHeader& h)
    {
        WaitForSingleObject(GetFileMutexHandle(), INFINITE);
        h = fileDll.mapreceive();
        ReleaseMutex(GetFileMutexHandle());
    }


private:
    struct hThreadMutexes
    {
        HANDLE cout;
        HANDLE file;

        hThreadMutexes(const HANDLE& hCoutMutex, const HANDLE& hFileMutex) : cout(hCoutMutex), file(hFileMutex) {}

        ~hThreadMutexes()
        {
            CloseHandle(cout);
            CloseHandle(file);
        }
    };
    
    hThreadMutexes muts;

    HANDLE hUniqueCommandEvent;
    HANDLE hUniqueFileEvent;

    HANDLE hConfirmEvent;

    const int i;

    HANDLE thisThread;

    const DllLoader fileDll;
};



class MainThread
{
public:

    MainThread() 
    {
        if (fm.getStatus())
            std::cout << "Start without file manager " << fm.getStatus() << std::endl;

        std::cout << "Main thread" << std::endl;
    }

    void start()
    {
        receiveHeader h;

        while (true)
        {
            DWORD dwWaitResult = WaitForMultipleObjects(4, hExtermalEvents, FALSE, INFINITE);

            switch (dwWaitResult)
            {
            case WAIT_OBJECT_0:
                StartEventHandler();
                break;

            case WAIT_OBJECT_0 + 1:
                StopEventHandler();
                break;

            case WAIT_OBJECT_0 + 2:
                End();
                break;
            
            case WAIT_OBJECT_0 + 3:
                std::cout << "i catch it" << std::endl;
                h = fm.fileDll.mapreceive();
                std::cout << h.h.addr << std::endl;
                std::cout << h.h.size << std::endl;
                std::cout << h.str << std::endl;
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
        HANDLE cEv = CreateEvent(NULL, FALSE, FALSE, std::to_string(ind).c_str());
        HANDLE fEv = CreateEvent(NULL, FALSE, FALSE, std::to_string(ind).append("f").c_str());
        manager.AddUniqueEvents(cEv, fEv);

        optionalThreads.push(SimpleThread(manager, fm, ind));

        SetEvent(cEv);

        WaitForSingleObject(manager.GetConfirmEventHandle(), INFINITE);
    }


    void StopEventHandler()
    {
        SetEvent(manager.GetLastCommandUniqueEvent());
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
    FileManager fm = FileManager();

    EventManager manager = EventManager();

    std::stack<SimpleThread> optionalThreads;
    
    HANDLE hExtermalEvents[4] = { CreateEvent(NULL, TRUE, TRUE, "Start"),
                                  CreateEvent(NULL, TRUE, TRUE, "Stop"),
                                  CreateEvent(NULL, TRUE, TRUE, "Close"),
                                  CreateEvent(NULL, TRUE, TRUE, "See file") };

    HANDLE hConfirmExOpEvent = CreateEvent(NULL, FALSE, FALSE, "Confirm");
};


UINT body(LPVOID lpParam)
{
    SimpleThread Thread = *static_cast<SimpleThread*>(lpParam);


    WaitForSingleObject(Thread.GetCommandUniqueEventHandle(), INFINITE);
    
    Thread.SyncConsoleWrite("start thread " + std::to_string(Thread.GetIndex()));

    SetEvent(Thread.GetConfirmEventHandle());


    WaitForSingleObject(Thread.GetCommandUniqueEventHandle(), INFINITE);

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
