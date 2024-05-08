// Kyryl Sydorov, 2024

#pragma once

#include <queue>
#include <unordered_map>

#include "Pool/SimplestPoolTS.h"
#include "ThreadWrapper.h"

using std::mutex;
using std::queue;
using std::shared_ptr;
using std::unordered_map;
using uint64 = std::uint64_t;

class AsyncTask;

class AsyncTaskManager
{
public:
    AsyncTaskManager();
    ~AsyncTaskManager();
    
    void addTask(shared_ptr<AsyncTask> task);

    void run();
    
private:
    void handleTaskFinished(uint64 taskId);
    
    queue<shared_ptr<AsyncTask>> _queuedTasks;

    unordered_map<uint64, SimplestPoolTS<ThreadWrapper>::Proxy> _activeTasks;

    SimplestPoolTS<ThreadWrapper> _threadPool;

    mutex _queueMutex;
    mutex _activeMutex;
};
