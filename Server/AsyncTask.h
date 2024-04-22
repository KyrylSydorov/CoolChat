// Kyryl Sydorov, 2024

#pragma once

#include <functional>
#include <sstream>

class Server;

class AsyncTask
{
public:
    AsyncTask();

    AsyncTask(const AsyncTask&) = delete;
    AsyncTask& operator=(const AsyncTask&) = delete;

    AsyncTask(AsyncTask&&) = delete;
    AsyncTask& operator=(AsyncTask&&) = delete;

    virtual ~AsyncTask() = default;

    virtual void execute() = 0;
    
    bool isFinished() const;
    bool isSucceeded() const;

protected:
    void finish(bool success);

    virtual void onFinished() {}
    
private:
    bool _finished = false;
    bool _succeeded = false;
};

class StreamAsyncTask : public AsyncTask
{
public:
    using TCallback = std::function<void(std::string)>;
    
    StreamAsyncTask(std::stringstream&& stream, TCallback callback);

protected:
    std::stringstream _stream;
    TCallback _callback;
};