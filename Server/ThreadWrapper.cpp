// Kyryl Sydorov, 2024

#include <assert.h>
#include <windows.h>

#include "AsyncTask.h"
#include "ThreadWrapper.h"

using namespace std;

ThreadWrapper::ThreadWrapper()
{
    _thread = make_unique<thread>(&ThreadWrapper::run, this);
}

void ThreadWrapper::scheduleTask(shared_ptr<AsyncTask> task, TCallback callback)
{
    assert(!_task.load() && "Task is already scheduled");
    _callback = std::move(callback);
    _task = std::move(task);
}

void ThreadWrapper::waitUntilDone()
{
    while (_task.load())
    {
        Sleep(10);
    }
}

void ThreadWrapper::run()
{
    while (true)
    {
        const std::shared_ptr<AsyncTask> task = _task.load();
        if (!task)
        {
            Sleep(10);
            continue;
        }
        
        task->execute();

        if (task->isFinished())
        {
            _task.store(nullptr);

            if (_callback)
            {
                _callback();
                _callback = nullptr;
            }
        }
    }
}