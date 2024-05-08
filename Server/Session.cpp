// Kyryl Sydorov, 2024

#include <sstream>
#include <winsock2.h>

#include "Constants.h"
#include "Server.h"
#include "Session.h"

#include "AsyncTasks.h"

using namespace std;

Session::Session(Server& server, SOCKET clientSocket)
    : _server(server)
    , _clientSocket(clientSocket)
{
}

void Session::processAuthorizedRequest(std::stringstream&& ss)
{
    int requestTypeInt = static_cast<int>(RequestType::Invalid);
    ss >> requestTypeInt;
    
    function callback = [wThis = weak_from_this()](string response)
    {
        if (wThis.expired())
        {
            return;
        }

        const shared_ptr<Session> This = wThis.lock();        
        This->send(move(response));
    };

    switch (static_cast<RequestType>(requestTypeInt))
    {
    case RequestType::GetUserInfo:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetUserInfo>(_currentUserId, move(ss), move(callback)));
            break;
        }
    case RequestType::GetMessages:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetMessages>(_currentUserId, move(ss), move(callback)));
            break;
        }
    case RequestType::SendMessageTo:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskSendMessage>(_currentUserId, move(ss), move(callback)));
            break;
        }
    case RequestType::GetMessageTo:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetMessage>(_currentUserId, move(ss), move(callback)));
            break;
        }
    case RequestType::FetchDialogStates:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskFetchDialogStates>(_currentUserId, move(ss), move(callback)));
            break;
        }
    case RequestType::GetLastMessages:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetLastMessages>(_currentUserId, move(ss), move(callback)));
            break;
        }
    case RequestType::UpdateDialogState:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskUpdateDialogState>(_currentUserId, move(ss), move(callback)));
            break;
        }
    default:
        {
            stringstream response;
            response << static_cast<int>(RequestType::Invalid) << ' ' << Errors::internalError;
            send(move(response).str());
            break;
        }
    }
}

void Session::processUnauthorizedRequest(std::stringstream&& ss)
{
    int requestTypeInt = static_cast<int>(RequestType::Invalid);
    ss >> requestTypeInt;

    function loginCallback = [wThis = weak_from_this()](string response)
    {
        if (wThis.expired())
        {
            return;
        }

        const shared_ptr<Session> This = wThis.lock();

        stringstream ss(response);
        
        int responseType;
        ss >> responseType;

        int errorCode;
        ss >> errorCode;

        if (errorCode == Errors::success)
        {
            // Authorization successful
            UserInfo userInfo;
            ss >> userInfo;

            This->_currentUserId = userInfo.hashedNickname;
        }
        
        This->send(move(response));
    };
    
    function registerCallback = [wThis = weak_from_this()](string response)
    {
        if (wThis.expired())
        {
            return;
        }

        const shared_ptr<Session> This = wThis.lock();

        stringstream ss(response);
        
        int responseType;
        ss >> responseType;

        int errorCode;
        ss >> errorCode;

        if (errorCode == Errors::success)
        {
            // Authorization successful
            ss >> This->_currentUserId;
        }
        
        This->send(move(response));
    };

    switch (static_cast<RequestType>(requestTypeInt))
    {
    case RequestType::Login:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskLogin>(move(ss), move(loginCallback)));
            break;
        }
    case RequestType::Register:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskRegister>(move(ss), move(registerCallback)));
            break;
        }
    default:
        {
            stringstream response;
            response << requestTypeInt << ' ' << Errors::unauthorized;
            send(move(response).str());
            break;
        }
    }
}

shared_ptr<Session> Session::create(Server& server, SOCKET clientSocket)
{
    Session* session = new Session(server, clientSocket);
    return shared_ptr<Session>(session);
}

bool Session::isAuthorized() const
{
    return _currentUserId != -1;
}

void Session::receive()
{
    _receivingInProgress = true;
    
    int msgLen = 0;
    int bytesRead = recv(_clientSocket, (char*)&msgLen, sizeof(msgLen), 0);
    if (bytesRead == 0 || (bytesRead < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
    {
        _server.closeConnection(_clientSocket);
        return;
    }

    constexpr int MaxMessageSize = 128 * 1024; // 128 KiB
    if (msgLen < 0 || msgLen > MaxMessageSize)
    {
        _server.closeConnection(_clientSocket);
        return;
    }

    std::string message;
    message.resize(msgLen, '\0');
    
    bytesRead = recv(_clientSocket, message.data(), msgLen, 0);
    if (bytesRead == 0 || (bytesRead < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
    {
        _server.closeConnection(_clientSocket);
        return;
    }
    
    stringstream ss(message);

    if (isAuthorized())
    {
        processAuthorizedRequest(move(ss));
    }
    else
    {
        processUnauthorizedRequest(move(ss));
    }

    _receivingInProgress = false;
}

void Session::send(string message)
{
    _sendingInProgress = true;

    int msgLen = static_cast<int>(message.size());
    
    int bytesSent = ::send(_clientSocket, (char*) &msgLen, sizeof(msgLen), 0);
    if (bytesSent == 0 || (bytesSent < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
    {
        _server.closeConnection(_clientSocket);
        return;
    }

    bytesSent = ::send(_clientSocket, message.c_str(), msgLen, 0);
    if (bytesSent == 0 || (bytesSent < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
    {
        _server.closeConnection(_clientSocket);
        return;
    }
    
    _sendingInProgress = false;
}
