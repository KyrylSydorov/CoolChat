// Kyryl Sydorov, 2024

#pragma once

#include <functional>
#include <memory>
#include <thread>

using std::shared_ptr;
using std::thread;
using std::unique_ptr;

class AsyncTask;

struct ThreadWrapper
{
public:
    using TCallback = std::function<void()>;
    
    ThreadWrapper();

    void scheduleTask(shared_ptr<AsyncTask> task, TCallback callback);
    void waitUntilDone();
    
private:
    void run();
    
    unique_ptr<thread> _thread;
    std::atomic<shared_ptr<AsyncTask>> _task;
    TCallback _callback;
};

