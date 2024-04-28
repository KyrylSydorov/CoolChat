// Kyryl Sydorov, 2024

#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "Client.h"
#include "../../Server/Constants.h"

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

void Client::newContact(const UserInfo& userInfo)
{
    _contacts.push_back(userInfo);
}

const std::vector<UserInfo>& Client::getContacts()
{
    stringstream ss;
    ss << static_cast<int>(RequestType::GetContacts) << ' ' << _currentUser.hashedNickname;
    string response = serverRequest(ss.str());
    if (response.empty())
    {
        return _contacts;
    }

    stringstream responseStream(response);
    int responseType;
    responseStream >> responseType;
    if (responseType != static_cast<int>(RequestType::GetContacts))
    {
        return _contacts;
    }

    int errorCode;
    responseStream >> errorCode;
    if (errorCode != Errors::success)
    {
        return _contacts;
    }

    int contactsNum = 0;
    responseStream >> contactsNum;
    
    for (int i = 0; i < contactsNum; ++i)
    {
        int contactId;
        responseStream >> contactId;

        // find in _contacts
        bool found = false;
        for (const UserInfo& contact : _contacts)
        {
            if (contact.hashedNickname == contactId)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            _contacts.push_back(getUserInfo(contactId));
        }
    }

    return _contacts;
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
    ss << static_cast<int>(RequestType::SendMessageTo) << ' ' << _currentUser.hashedNickname << ' ' << receiverId << ' ' << message;
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

std::vector<Message> Client::getMessages(int senderId, int fromId)
{
    stringstream ss;
    ss << static_cast<int>(RequestType::GetMessages) << ' ' << _currentUser.hashedNickname << ' ' << senderId << ' ' << fromId;
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

std::string Client::serverRequest(std::string request)
{
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

    //DWORD timeout = 10000;
    //setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    //setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
}
