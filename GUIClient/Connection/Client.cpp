// Kyryl Sydorov, 2024

#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Client.h"
#include "../../Server/Constants.h"
#include "../../Server/SerializationHelpers.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

Client::Client()
{
}

void Client::start()
{
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return;
    }

    initializeClientSocket();
    if (_socket == INVALID_SOCKET)
    {
        WSACleanup();
        return;
    }
}

int Client::login(const string& username, const string& password)
{
    std::unique_lock<std::shared_mutex> lock(_rwLock);
    stringstream ss;
    ss << static_cast<int>(RequestType::Login) << ' ' << username << ' ' << password;
    string response = serverRequest(ss.str());
    if (response.empty())
    {
        return Errors::internalError;
    }
    
    stringstream responseStream(response);
    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::Login))
    {
        return Errors::internalError;
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode == Errors::success)
    {
        responseStream >> _currentUser;
    }

    return errorCode;
}

int Client::registerUser(const UserInfo& userInfo)
{
    std::unique_lock<std::shared_mutex> lock(_rwLock);
    _currentUser = userInfo;
    
    stringstream ss;
    ss << static_cast<int>(RequestType::Register) << ' ' << userInfo;
    string response = serverRequest(ss.str());
    if (response.empty())
    {
        return Errors::internalError;
    }

    stringstream responseStream(response);
    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::Register))
    {
        return Errors::internalError;
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode == Errors::success)
    {
        responseStream >> _currentUser.hashedNickname;
    }

    return errorCode;
}

void Client::logout()
{
    std::unique_lock<std::shared_mutex> lock(_rwLock);
    _cachedDialogs.clear();
    _currentUser = UserInfo();

    if (_socket != INVALID_SOCKET)
    {
        shutdown(_socket, SD_SEND);
        closesocket(_socket);
        WSACleanup();
    }

    start();
}

void Client::updateDialogs()
{
    fetchDialogs();
    fetchLastMessages();
    fetchContacts();
}

void Client::newContact(const UserInfo& userInfo)
{
    std::unique_lock<std::shared_mutex> lock(_rwLock);
    _cachedDialogs.emplace_back(Dialog{ userInfo, { _currentUser.hashedNickname, userInfo.hashedNickname }, {} });
}

const UserInfo& Client::getCurrentUser() const
{
    return _currentUser;
}

UserInfo Client::getUserInfo(int id)
{
    stringstream ss;
    ss << static_cast<int>(RequestType::GetUserInfo) << ' ' << id;
    string response = serverRequest(ss.str());
    if (response.empty())
    {
        return UserInfo();
    }

    stringstream responseStream(response);
    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::GetUserInfo))
    {
        return UserInfo();
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode != Errors::success)
    {
        return UserInfo();
    }

    UserInfo userInfo;
    responseStream >> userInfo;

    return userInfo;
}

UserInfo Client::getUserInfo(const std::string& nickname)
{
    const int id = hashString(nickname);
    return getUserInfo(id);
}

void Client::sendMessage(int receiverId, const std::string& message)
{
    stringstream ss;
    ss << static_cast<int>(RequestType::SendMessageTo) << ' ' << receiverId << ' ' << message;
    string response = serverRequest(ss.str());
    if (response.empty())
    {
        return;
    }

    stringstream responseStream(response);

    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::SendMessageTo))
    {
        return;
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode != Errors::success)
    {
        return;
    }

    //cout << "Sent!\n\n";
}

void Client::updateMessages(int contactLocalId)
{
    std::unique_lock<std::shared_mutex> lock(_rwLock);
    if (contactLocalId < 0 || contactLocalId >= _cachedDialogs.size())
    {
        return;
    }

    Dialog& dialog = _cachedDialogs[contactLocalId];
    
    const size_t fromId = dialog.messages.size();

    if (fromId >= dialog.state.totalMessages)
    {
        // No new messages
        return;
    }
    
    vector<Message> messages = getMessages(dialog.state.contactId, fromId);
    for (Message& message : messages)
    {
        dialog.messages.push_back(move(message));
    }

    const int lastReadId = dialog.state.lastReadMessageId;
    dialog.state.lastReadMessageId = static_cast<int>(dialog.messages.size()) - 1;

    if (lastReadId == dialog.state.lastReadMessageId)
    {
        return;
    }
    
    stringstream ss;
    ss << static_cast<int>(RequestType::UpdateDialogState) << ' ' << dialog.contact.hashedNickname << ' ' << dialog.state.lastReadMessageId;
    serverRequest(ss.str());
}

std::vector<Message> Client::getMessages(int senderId, int fromId)
{
    stringstream ss;
    ss << static_cast<int>(RequestType::GetMessages) << ' ' << senderId << ' ' << fromId;
    string response = serverRequest(ss.str());
    if (response.empty())
    {
        return {};
    }
    
    stringstream responseStream(response);

    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::GetMessages))
    {
        return {};
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode != Errors::success)
    {
        return {};
    }

    int messagesNum;
    responseStream >> messagesNum;

    vector<Message> messages;
    for (int i = 0; i < messagesNum; ++i)
    {
        Message message;
        responseStream >> message;
        messages.push_back(message);
    }

    return messages;
}

void Client::cleanCachedMessages(int contactLocalId)
{
    std::unique_lock<std::shared_mutex> lock(_rwLock);
    if (contactLocalId < 0 || contactLocalId >= _cachedDialogs.size())
    {
        return;
    }
    _cachedDialogs[contactLocalId].messages.clear();
}

const std::vector<Dialog>& Client::getDialogs() const
{
    std::shared_lock<std::shared_mutex> lock(_rwLock);
    return _cachedDialogs;
}

std::string Client::serverRequest(std::string request)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    int msgLen = request.size();
    int iResult = send(_socket, (char*)&msgLen, sizeof(msgLen), 0);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(_socket);
        WSACleanup();
        exit(0);
    }
    
    iResult = send(_socket, request.c_str(), msgLen, 0);
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(_socket);
        WSACleanup();
        exit(0);
    }
    
    int respLen;
    iResult = recv(_socket, (char*)&respLen, sizeof(respLen), 0);
    if (iResult == 0)
    {
        std::cout << "Connection closed\n";
        exit(0);
    }
    if (iResult < 0)
    {
        std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
        exit(0);
    }

    string response;
    response.resize(respLen);
    
    iResult = recv(_socket, response.data(), respLen, 0);
    if (iResult > 0)
    {
        return response;
    }
    if (iResult == 0)
    {
        std::cout << "Connection closed\n";
        exit(0);
    }
    
    std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
    exit(0);
}

void Client::initializeClientSocket()
{
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket == INVALID_SOCKET)
    {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverInfo;
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(12345); // Server's listening port
    if (inet_pton(AF_INET, "127.0.0.1", &serverInfo.sin_addr) <= 0)
    {  
        std::cerr << "Invalid address/ Address not supported " << std::endl;
        closesocket(_socket);
        return;
    }

    if (connect(_socket, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) < 0)
    {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(_socket);
    }
}

void Client::fetchDialogs()
{
    stringstream ss;
    ss << static_cast<int>(RequestType::FetchDialogStates);
    string response = serverRequest(ss.str());
    if (response.empty())
    {
        return;
    }

    stringstream responseStream(response);
    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::FetchDialogStates))
    {
        return;
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode != Errors::success)
    {
        return;
    }

    std::vector<DialogState> dialogStates;
    responseStream >> dialogStates;

    std::unique_lock<std::shared_mutex> lock(_rwLock);
    for (DialogState& dialogState : dialogStates)
    {
        auto iter = std::find_if(_cachedDialogs.begin(), _cachedDialogs.end(),
            [&dialogState](const Dialog& dialog)
            {
                return dialog.state.contactId == dialogState.contactId;
            });
        if (iter != _cachedDialogs.end())
        {
            iter->state = dialogState;
        }
        else
        {
            _cachedDialogs.emplace_back(Dialog{ {}, dialogState, {} });
        }
    }
}

void Client::fetchLastMessages()
{
    stringstream ss;
    ss << static_cast<int>(RequestType::GetLastMessages) << ' ';

    vector<int> contactIds;
    {
        std::shared_lock<std::shared_mutex> lock(_rwLock);
        for (const Dialog& dialog : _cachedDialogs)
        {
            if (dialog.state.lastReadMessageId + 1 < dialog.state.totalMessages || dialog.messages.empty())
            {
                contactIds.push_back(dialog.state.contactId);
            }
        }
    }
    ss << contactIds;
    
    string response = serverRequest(ss.str());

    stringstream responseStream(response);
    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::GetLastMessages))
    {
        return;
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode != Errors::success)
    {
        return;
    }

    int messagesNum;
    responseStream >> messagesNum;

    std::unique_lock<std::shared_mutex> lock(_rwLock);
    for (int i = 0; i < messagesNum; ++i)
    {
        int contactId;
        responseStream >> contactId;

        Message message;
        responseStream >> message;

        auto iter = std::find_if(_cachedDialogs.begin(), _cachedDialogs.end(),
            [&contactId](const Dialog& dialog)
            {
                return dialog.state.contactId == contactId;
            });
        if (iter != _cachedDialogs.end())
        {
            if (iter->messages.size() == 0)
            {
                iter->messages.push_back(move(message));
            }
            else
            {
                iter->messages[0] = move(message);
            }
        }
    }
}

void Client::fetchContacts()
{
    std::unique_lock<std::shared_mutex> lock(_rwLock);
    for (Dialog& dialog : _cachedDialogs)
    {
        if (dialog.contact.isValid())
        {
            continue;
        }

        dialog.contact = getUserInfo(dialog.state.contactId);
    }
}
