// Kyryl Sydorov, 2024

#include <ws2tcpip.h>

#include "Server.h"

#include "Session.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

Server::Server()
{
}

Server::~Server()
{
}

void Server::start()
{
    if (!initializeWinsock())
    {
        return;
    }
    
    createListeningSocket();
    runServer();
    
    cleanup();
}

bool Server::initializeWinsock()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return false;
    }
    return true;
}

void Server::createListeningSocket()
{
    _listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_listenSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(12345); // Listening port
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);

    if (bind(_listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(_listenSocket);
        WSACleanup();
        return;
    }

    if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(_listenSocket);
        WSACleanup();
    }

    DWORD timeout = 10000;
    setsockopt(_listenSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(_listenSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
}

void Server::runServer()
{
    if (_listenSocket == INVALID_SOCKET)
    {
        return;
    }
    
    fd_set readFds;
    FD_ZERO(&_masterSet);
    FD_SET(_listenSocket, &_masterSet);
    _maxSocket = _listenSocket;

    while (true)
    {
        readFds = _masterSet;
        _writeSet = _masterSet;

        timeval timeout;
        timeout.tv_sec = 1; // 1 second timeout
        timeout.tv_usec = 0;
        
        const int activity = select(_maxSocket + 1, &readFds, &_writeSet, NULL, &timeout);

        if (activity < 0 && errno != EINTR)
        {
            std::cerr << "Select error: " << WSAGetLastError() << std::endl;
            break;
        }

        for (SOCKET i = 0; i <= _maxSocket; i++)
        {
            if (FD_ISSET(i, &readFds))
            {
                if (i == _listenSocket)
                {
                    handleNewConnection();
                }
                else
                {
                    handleClientData(i);
                }
            }
        }
    }
}

void Server::handleNewConnection()
{
    sockaddr_in clientInfo;
    int addrlen = sizeof(clientInfo);
    const SOCKET clientSocket = accept(_listenSocket, (struct sockaddr*)&clientInfo, &addrlen);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        return;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientInfo.sin_addr), client_ip, INET_ADDRSTRLEN);
    //std::cout << "New connection from " << client_ip << ":" << ntohs(clientInfo.sin_port) << std::endl;
    
    FD_SET(clientSocket, &_masterSet);
    if (clientSocket > _maxSocket)
    {
        _maxSocket = clientSocket;
    }

    _sessions[clientSocket] = Session::create(*this, clientSocket);

    DWORD timeout = 10000;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
}

void Server::handleClientData(SOCKET clientSocket)
{
    const std::shared_ptr<Session> session = _sessions[clientSocket];
    if (!session)
    {
        return;
    }

    if (session->isReceivingInProgress())
    {
        return;
    }

    session->receive();
}

void Server::closeConnection(SOCKET clientSocket)
{
    std::cout << "Closing connection " << clientSocket << std::endl;
    closesocket(clientSocket);
    FD_CLR(clientSocket, &_masterSet);
    _sessions.erase(clientSocket);
}

void Server::cleanup()
{
    for (SOCKET i = 0; i <= _maxSocket; i++)
    {
        if (FD_ISSET(i, &_masterSet))
        {
            closeConnection(i);
        }
    }
    WSACleanup();
}
