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

shared_ptr<Session> Session::create(Server& server, SOCKET clientSocket)
{
    Session* session = new Session(server, clientSocket);
    return shared_ptr<Session>(session);
}

bool Session::isAuthorized() const
{
    return _currentUser.isValid();
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

    std::string message;
    message.resize(msgLen, '\0');
    
    bytesRead = recv(_clientSocket, message.data(), msgLen, 0);
    if (bytesRead == 0 || (bytesRead < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
    {
        _server.closeConnection(_clientSocket);
        return;
    }
    
    stringstream ss(message);
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
        case RequestType::Login:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskLogin>(move(ss), move(callback)));
            break;
        }
        case RequestType::Register:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskRegister>(move(ss), move(callback)));
            break;
        }
        case RequestType::GetUserInfo:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetUserInfo>(move(ss), move(callback)));
            break;
        }
        case RequestType::GetMessages:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetMessages>(move(ss), move(callback)));
            break;
        }
        case RequestType::SendMessageTo:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskSendMessage>(move(ss), move(callback)));
            break;
        }
        case RequestType::GetContacts:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetContacts>(move(ss), move(callback)));
            break;
        }
        case RequestType::GetMessageTo:
        {
            _server.getTaskManager().addTask(make_shared<AsyncTaskGetMessage>(move(ss), move(callback)));
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

    _receivingInProgress = false;
}

void Session::send(string message)
{
    _sendingInProgress = true;

    int msgLen = message.size();
    
    int bytesSent = ::send(_clientSocket, (char*) &msgLen, sizeof(msgLen), 0);
    if (bytesSent == 0 || (bytesSent < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
    {
        _server.closeConnection(_clientSocket);
        return;
    }

    bytesSent = ::send(_clientSocket, message.c_str(), message.size(), 0);
    if (bytesSent == 0 || (bytesSent < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
    {
        _server.closeConnection(_clientSocket);
        return;
    }
    
    _sendingInProgress = false;
}
