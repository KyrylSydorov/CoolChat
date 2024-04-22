// Kyryl Sydorov, 2024

#pragma once

#include <winsock2.h>
#include <iostream>
#include <unordered_map>

#include "AsyncTaskManager.h"

class Session;

class Server
{
public:
    Server();
    ~Server();
    
    void start();
    void closeConnection(SOCKET clientSocket);

    AsyncTaskManager& getTaskManager() { return _taskManager; }
    
private:
    bool initializeWinsock();
    void createListeningSocket();
    void runServer();
    void handleNewConnection();
    void handleClientData(SOCKET clientSocket);
    void cleanup();

    std::unordered_map<SOCKET, std::shared_ptr<Session>> _sessions;
    
    fd_set _masterSet;
    fd_set _writeSet;
    
    SOCKET _listenSocket = INVALID_SOCKET;
    SOCKET _maxSocket = INVALID_SOCKET;

    AsyncTaskManager _taskManager;
};
