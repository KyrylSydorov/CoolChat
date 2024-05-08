// Kyryl Sydorov, 2024

#include "AsyncTaskManager.h"

#include <functional>
#include <iostream>

using namespace std;

AsyncTaskManager::AsyncTaskManager()
    : _threadPool(32)
{
}

AsyncTaskManager::~AsyncTaskManager()
{
    queue<shared_ptr<AsyncTask>> empty;
    {
        lock_guard lock(_queueMutex);
        swap(_queuedTasks, empty);
    }
    
    // The thread is already running, so we have to wait for it to finish to avoid UB
    lock_guard lock(_activeMutex);
    for (auto& [taskId, taskProxy] : _activeTasks)
    {
        taskProxy->waitUntilDone();
    }
    _activeTasks.clear();
}

void AsyncTaskManager::addTask(std::shared_ptr<AsyncTask> task)
{
    auto threadWrapper = _threadPool.pull();
    if (!threadWrapper)
    {
        lock_guard lock(_queueMutex);
        _queuedTasks.emplace(std::move(task));
        return;
    }

    static uint64 taskId = 0;
    const uint64 currentTaskId = taskId++;

    ThreadWrapper::TCallback callback = bind(&AsyncTaskManager::handleTaskFinished, this, currentTaskId);
    threadWrapper->scheduleTask(std::move(task), std::move(callback));
    
    _activeTasks.insert({ currentTaskId, std::move(threadWrapper) });
    
#ifdef _DEBUG
    cout << "Task " << taskId << " scheduled" << endl;
#endif   
}

void AsyncTaskManager::handleTaskFinished(uint64 taskId)
{
#ifdef _DEBUG
    cout << "Task " << taskId << " finished" << endl;
#endif
    
    {
        lock_guard lock(_activeMutex);
        _activeTasks.erase(taskId);
    }

    shared_ptr<AsyncTask> nextTask;
    {
        lock_guard lock(_queueMutex);
        if (!_queuedTasks.empty())
        {
            nextTask = std::move(_queuedTasks.front());
            _queuedTasks.pop();
        }
    }

    if (nextTask)
    {
        addTask(std::move(nextTask));
    }
}
