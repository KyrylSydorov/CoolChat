// Kyryl Sydorov, 2024

#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <unordered_map>

class Server
{
public:
    Server();
    ~Server();
    
    void start();
    
private:
    bool initializeWinsock();
    void createListeningSocket();
    void runServer();
    void handleNewConnection();
    void handleClientData(SOCKET clientSocket);
    void closeConnection(SOCKET clientSocket);
    void cleanup();

    std::unordered_map<SOCKET, std::string> _messages;
    
    fd_set _masterSet;
    fd_set _writeSet;
    
    SOCKET _listenSocket = INVALID_SOCKET;
    SOCKET _maxSocket = INVALID_SOCKET;

    fd_set _queuedSet;
};