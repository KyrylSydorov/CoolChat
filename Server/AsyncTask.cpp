// Kyryl Sydorov, 2024

#include "AsyncTask.h"
#include "Server.h"

AsyncTask::AsyncTask()
{
}

bool AsyncTask::isFinished() const
{
    return _finished;
}

bool AsyncTask::isSucceeded() const
{
    return _succeeded;
}

void AsyncTask::finish(bool success)
{
    _succeeded = success;
    onFinished();
    _finished = true;
}

StreamAsyncTask::StreamAsyncTask(int userId, std::stringstream&& stream, TCallback callback)
    : _userId(userId)
    , _stream(std::move(stream))
    , _callback(std::move(callback))
{
}
