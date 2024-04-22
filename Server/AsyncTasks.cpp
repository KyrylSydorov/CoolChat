// Kyryl Sydorov, 2024

#include "AsyncTasks.h"
#include "Constants.h"
#include "Server.h"
#include "UserManager.h"
#include "MessageManager.h"

using namespace std;

AsyncTaskLogin::AsyncTaskLogin(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(std::move(stream), std::move(callback))
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
    : StreamAsyncTask(std::move(stream), std::move(callback))
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

AsyncTaskGetUserInfo::AsyncTaskGetUserInfo(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(std::move(stream), std::move(callback))
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

AsyncTaskGetContacts::AsyncTaskGetContacts(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(std::move(stream), std::move(callback))
{
}

void AsyncTaskGetContacts::execute()
{
    int id;
    _stream >> id;

    vector<int> contacts = MessageManager::getContacts(id);

    stringstream ss;
    ss << static_cast<int>(RequestType::GetContacts) << ' ' << Errors::success << ' ' << contacts.size();

    for (const int contactId : contacts)
    {
        ss << ' ' << contactId;
    }

    _callback(move(ss).str());
    finish(true);

    cout << "User get contacts\n";
}

AsyncTaskSendMessage::AsyncTaskSendMessage(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(std::move(stream), std::move(callback))
{
}

void AsyncTaskSendMessage::execute()
{
    int senderId;
    int receiverId;
    string message;
    _stream >> senderId >> receiverId;
    getline(_stream, message);

    UserInfo receiver = UserManager::getUserInfo(receiverId);
    if (!receiver.isValid())
    {
        _callback(to_string(static_cast<int>(RequestType::SendMessageTo)) + ' ' + to_string(Errors::invalidCredentials));
        finish(false);
        return;
    }

    MessageManager::sendMessage(senderId, receiverId, message);

    _callback(to_string(static_cast<int>(RequestType::SendMessageTo)) + ' ' + to_string(Errors::success));
    finish(true);
    
    //cout << "User send message\n";
}

AsyncTaskGetMessage::AsyncTaskGetMessage(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(std::move(stream), std::move(callback))
{
}

void AsyncTaskGetMessage::execute()
{
    int senderId;
    int receiverId;
    int messageId;
    _stream >> senderId >> receiverId >> messageId;

    Message message = MessageManager::getMessage(senderId, receiverId, messageId);

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

AsyncTaskGetMessages::AsyncTaskGetMessages(std::stringstream&& stream, TCallback callback)
    : StreamAsyncTask(std::move(stream), std::move(callback))
{
}

void AsyncTaskGetMessages::execute()
{
    int senderId;
    int receiverId;
    int fromId;
    _stream >> senderId >> receiverId >> fromId;

    vector<Message> messages = MessageManager::getMessages(senderId, receiverId, fromId);

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
