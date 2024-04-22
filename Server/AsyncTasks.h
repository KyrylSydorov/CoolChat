// Kyryl Sydorov, 2024

#pragma once

#include "AsyncTask.h"

class AsyncTaskLogin : public StreamAsyncTask
{
public:
    AsyncTaskLogin(std::stringstream&& stream, TCallback callback);
    
protected:
    virtual void execute() override;
};

class AsyncTaskRegister : public StreamAsyncTask
{
public:
    AsyncTaskRegister(std::stringstream&& stream, TCallback callback);
    
protected:
    virtual void execute() override;
};

class AsyncTaskGetUserInfo : public StreamAsyncTask
{
public:
    AsyncTaskGetUserInfo(std::stringstream&& stream, TCallback callback);

protected:
    virtual void execute() override;
};

class AsyncTaskGetContacts : public StreamAsyncTask
{
public:
    AsyncTaskGetContacts(std::stringstream&& stream, TCallback callback);

protected:
    virtual void execute() override;
};

class AsyncTaskSendMessage : public StreamAsyncTask
{
public:
    AsyncTaskSendMessage(std::stringstream&& stream, TCallback callback);

protected:
    virtual void execute() override;
};

class AsyncTaskGetMessage : public StreamAsyncTask
{
public:
    AsyncTaskGetMessage(std::stringstream&& stream, TCallback callback);

protected:
    virtual void execute() override;
};

class AsyncTaskGetMessages : public StreamAsyncTask
{
public:
    AsyncTaskGetMessages(std::stringstream&& stream, TCallback callback);
    
protected:
    virtual void execute() override;
};