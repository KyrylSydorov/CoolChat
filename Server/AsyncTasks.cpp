// Kyryl Sydorov, 2024

#include "AsyncTasks.h"
#include "Constants.h"
#include "SerializationHelpers.h"
#include "Server.h"
#include "UserManager.h"
#include "MessageManager.h"

using namespace std;

AsyncTaskLogin::AsyncTaskLogin(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(-1, std::move(stream), std::move(callback))
{
}

void AsyncTaskLogin::execute()
{
    string username;
    string password;
    _stream >> username >> password;

    const int id = UserManager::getIdByNickname(username);
    
    stringstream ss;
    ss << static_cast<int>(RequestType::Login) << ' ';

    UserInfo user = UserManager::getUserInfo(id);
    if (!user.isValid() || user.password != password)
    {
        ss << Errors::invalidCredentials;
    }
    else
    {
        ss << Errors::success << ' ' << user;
    }

    _callback(move(ss).str());
    finish(true);

    cout << "User logged in\n";
}

AsyncTaskRegister::AsyncTaskRegister(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(-1, std::move(stream), std::move(callback))
{
}

void AsyncTaskRegister::execute()
{
    UserInfo user;
    _stream >> user;

    const int errorCode = UserManager::registerUser(user);
    
    stringstream ss;
    ss << static_cast<int>(RequestType::Register) << ' ' << errorCode;

    if (errorCode == Errors::success)
    {
        ss << ' ' << user.hashedNickname;
    }
    
    _callback(move(ss).str());
    finish(true);
    cout << "User registered\n";
}

AsyncTaskGetUserInfo::AsyncTaskGetUserInfo(int userId, std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(userId, std::move(stream), std::move(callback))
{
}

void AsyncTaskGetUserInfo::execute()
{
    int id;
    _stream >> id;

    UserInfo user = UserManager::getUserInfo(id);
    user.password.clear(); // don't send password to the client

    stringstream ss;
    ss << static_cast<int>(RequestType::GetUserInfo) << ' ' << Errors::success << ' ' << user;

    _callback(move(ss).str());
    finish(true);

    cout << "User get info\n";
}

AsyncTaskSendMessage::AsyncTaskSendMessage(int userId, std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(userId, std::move(stream), std::move(callback))
{
}

void AsyncTaskSendMessage::execute()
{
    int receiverId;
    string message;
    _stream >> receiverId;
    getline(_stream, message);

    UserInfo receiver = UserManager::getUserInfo(receiverId);
    if (!receiver.isValid())
    {
        _callback(to_string(static_cast<int>(RequestType::SendMessageTo)) + ' ' + to_string(Errors::invalidCredentials));
        finish(false);
        return;
    }

    MessageManager::sendMessage(_userId, receiverId, message);

    _callback(to_string(static_cast<int>(RequestType::SendMessageTo)) + ' ' + to_string(Errors::success));
    finish(true);
    
    //cout << "User send message\n";
}

AsyncTaskGetMessage::AsyncTaskGetMessage(int userId, std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(userId, std::move(stream), std::move(callback))
{
}

void AsyncTaskGetMessage::execute()
{
    int contactId;
    int messageId;
    _stream >> contactId >> messageId;

    const Message message = MessageManager::getMessage(_userId, contactId, messageId);

    stringstream ss;
    ss << static_cast<int>(RequestType::GetMessageTo) << ' ';
    
    if (message.sender == -1)
    {
        ss << Errors::noSuchMessage;
    }
    else
    {
        ss << Errors::success << ' ' << message;
    }

    _callback(move(ss).str());
    finish(true);

    cout << "User get message\n";
}

AsyncTaskGetMessages::AsyncTaskGetMessages(int userId, std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(userId, std::move(stream), std::move(callback))
{
}

void AsyncTaskGetMessages::execute()
{
    int contactId;
    int fromId;
    _stream >> contactId >> fromId;

    const vector<Message> messages = MessageManager::getMessages(_userId, contactId, fromId);

    stringstream ss;
    ss << static_cast<int>(RequestType::GetMessages) << ' ' << Errors::success << ' ' << messages.size() << '\n';

    for (const Message& message : messages)
    {
        ss << message << '\n';
    }

    _callback(move(ss).str());
    finish(true);

    cout << "User get all messages\n";
}

AsyncTaskFetchDialogStates::AsyncTaskFetchDialogStates(int userId, std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(userId, std::move(stream), std::move(callback))
{
}

void AsyncTaskFetchDialogStates::execute()
{
    const vector<DialogState> dialogStates = MessageManager::getDialogStates(_userId);

    stringstream ss;
    ss  << static_cast<int>(RequestType::FetchDialogStates) << ' '
        << Errors::success << ' '
        << dialogStates;
    
    _callback(move(ss).str());
    finish(true);

    cout << "User fetch dialog states\n";
}

AsyncTaskGetLastMessages::AsyncTaskGetLastMessages(int userId, std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(userId, std::move(stream), std::move(callback))
{
}

void AsyncTaskGetLastMessages::execute()
{
    std::vector<int> contactIds;
    _stream >> contactIds;

    std::vector<std::pair<int, Message>> result;
    for (int contactId : contactIds)
    {
        Message message = MessageManager::getLastMessage(_userId, contactId);
        if (message.sender != -1)
        {
            result.emplace_back(contactId, move(message));
        }
    }
    
    stringstream ss;
    ss << static_cast<int>(RequestType::GetLastMessages) << ' ' << Errors::success << ' ' << result.size() << '\n';
    for (const auto& [contactId, message] : result)
    {
        ss << contactId << ' ' << message << '\n';
    }

    _callback(move(ss).str());
    finish(true);

    cout << "User get last messages\n";
}

AsyncTaskUpdateDialogState::AsyncTaskUpdateDialogState(int userId, std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(userId, std::move(stream), std::move(callback))
{
}

void AsyncTaskUpdateDialogState::execute()
{
    int contactId;
    int lastReadMessageId;
    _stream >> contactId >> lastReadMessageId;

    MessageManager::saveDialogState({ _userId, contactId, lastReadMessageId });

    stringstream ss;
    ss << static_cast<int>(RequestType::UpdateDialogState) << ' ' << Errors::success;
    
    _callback(move(ss).str());
    finish(true);

    cout << "User update dialog state\n";
}
