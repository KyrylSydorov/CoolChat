// Kyryl Sydorov, 2024

#pragma once

#include <string>
#include <sstream>

#include "UserManager.h"

class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
    static std::shared_ptr<Session> create(Server& server, SOCKET clientSocket);
    
    bool isAuthorized() const;

    void receive();
    void send(std::string message);

    bool isReceivingInProgress() const { return _receivingInProgress; }
    bool isSendingInProgress() const { return _sendingInProgress; }
    
private:
    Session(Server& server, SOCKET clientSocket);

    void processAuthorizedRequest(std::stringstream&& ss);
    void processUnauthorizedRequest(std::stringstream&& ss);
    
    int _currentUserId = -1;

    bool _receivingInProgress = false;
    bool _sendingInProgress = false;

    Server& _server;
    SOCKET _clientSocket = INVALID_SOCKET;
};
