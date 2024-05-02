// Kyryl Sydorov, 2024

#pragma once

#include "AsyncTask.h"

/**
 * AsyncTaskLogin
 * Request layout:
 *  string Username
 *  string Password
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 *  UserInfo User (if ErrorCode == Errors::success)
 */
class AsyncTaskLogin : public StreamAsyncTask
{
public:
    AsyncTaskLogin(std::stringstream&& stream, TCallback callback);
    
protected:
    virtual void execute() override;
};

/**
 * AsyncTaskRegister
 * Request layout:
 *  UserInfo User
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 *  int UserId (if ErrorCode == Errors::success)
 */
class AsyncTaskRegister : public StreamAsyncTask
{
public:
    AsyncTaskRegister(std::stringstream&& stream, TCallback callback);
    
protected:
    virtual void execute() override;
};

/**
 * AsyncTaskGetUserInfo
 * Request layout:
 *  int UserId
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 *  UserInfo User
 */
class AsyncTaskGetUserInfo : public StreamAsyncTask
{
public:
    AsyncTaskGetUserInfo(int userId, std::stringstream&& stream, TCallback callback);

protected:
    virtual void execute() override;
};

/**
 * AsyncTaskSendMessage
 * Request layout:
 *  int ReceiverId
 *  String Message
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 */
class AsyncTaskSendMessage : public StreamAsyncTask
{
public:
    AsyncTaskSendMessage(int userId, std::stringstream&& stream, TCallback callback);

protected:
    virtual void execute() override;
};

/**
 * AsyncTaskGetMessage
 * Request layout:
 *  int ContactId
 *  int MessageId
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 *  Message Message (if ErrorCode == Errors::success)
 */
class AsyncTaskGetMessage : public StreamAsyncTask
{
public:
    AsyncTaskGetMessage(int userId, std::stringstream&& stream, TCallback callback);

protected:
    virtual void execute() override;
};

/**
 * AsyncTaskGetMessages
 * Request layout:
 *  int ContactId
 *  int FromId
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 *  int Messages num
 *      N of:
 *      Message Messages[Messages num]
 */
class AsyncTaskGetMessages : public StreamAsyncTask
{
public:
    AsyncTaskGetMessages(int userId, std::stringstream&& stream, TCallback callback);
    
protected:
    virtual void execute() override;
};

/**
 * AsyncTaskFetchDialogStates
 * Request layout:
 *  not used
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 *  int Dialogs num
 *      N of:
 *      DialogState state
 */
class AsyncTaskFetchDialogStates : public StreamAsyncTask
{
public:
    AsyncTaskFetchDialogStates(int userId, std::stringstream&& stream, TCallback callback);
protected:
    virtual void execute() override;
};

/**
 * AsyncTaskGetLastMessages
 * Request layout:
 *  int Dialogs num
 *      N of:
 *      int ContactId
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 *  int Messages num
 *      N of:
 *      int ContactId
 *      Message Message
 */
class AsyncTaskGetLastMessages : public StreamAsyncTask
{
public:
    AsyncTaskGetLastMessages(int userId, std::stringstream&& stream, TCallback callback);
protected:
    virtual void execute() override;
};

/**
 * AsyncTaskUpdateDialogState
 * Request layout:
 *  int ContactId
 *  int LastReadMessageId
 * Response layout:
 *  int RequestType
 *  int ErrorCode
 */
class AsyncTaskUpdateDialogState : public StreamAsyncTask
{
public:
    AsyncTaskUpdateDialogState(int userId, std::stringstream&& stream, TCallback callback);
protected:
    virtual void execute() override;
};
