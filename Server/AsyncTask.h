// Kyryl Sydorov, 2024

#pragma once

class AsyncTask
{
public:
    AsyncTask();

    AsyncTask(const AsyncTask&) = delete;
    AsyncTask& operator=(const AsyncTask&) = delete;

    AsyncTask(AsyncTask&&) = delete;
    AsyncTask& operator=(AsyncTask&&) = delete;

    bool isFinished() const;

private:
    bool _finished = false;
};
