// Kyryl Sydorov, 2024

#include "Server.h"

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

    u_long mode = 1; // Enable non-blocking socket
    ioctlsocket(_listenSocket, FIONBIO, &mode);

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
}

void Server::runServer()
{
    if (_listenSocket == INVALID_SOCKET)
    {
        return;
    }
    
    fd_set readFds;
    FD_ZERO(&_masterSet);
    FD_ZERO(&_queuedSet);
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

            if (FD_ISSET(i, &_writeSet) && !_messages[i].empty())
            {
                int bytesSent = send(i, _messages[i].c_str(), _messages[i].size(), 0);
                if (bytesSent > 0)
                {
                    _messages[i].erase(0, bytesSent);
                }

                std::cout << i << " - Bytes sent: " << bytesSent << std::endl;
            }
        }
        std::cout << "Loop iteration" << std::endl;
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
    std::cout << "New connection from " << client_ip << ":" << ntohs(clientInfo.sin_port) << std::endl;

    u_long mode = 1; // Enable non-blocking mode for the new socket
    ioctlsocket(clientSocket, FIONBIO, &mode);
    FD_SET(clientSocket, &_masterSet);
    if (clientSocket > _maxSocket)
    {
        _maxSocket = clientSocket;
    }
    _messages[clientSocket] = ""; // Initialize message buffer for this socket
}

void Server::handleClientData(SOCKET clientSocket)
{
    // Marking this socket as queued for I/O
    FD_SET(clientSocket, &_queuedSet);
    
    char buffer[1024]; // Data buffer for incoming data
    const int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0)
    {
        _messages[clientSocket].append(buffer, bytesRead);
    }
    else
    {
        if (bytesRead == 0 || (bytesRead < 0 && WSAGetLastError() != WSAEWOULDBLOCK))
        {
            closeConnection(clientSocket);
        }
    }
}

void Server::closeConnection(SOCKET clientSocket)
{
    std::cout << "Closing connection " << clientSocket << std::endl;
    closesocket(clientSocket);
    FD_CLR(clientSocket, &_masterSet);
    _messages.erase(clientSocket);
}

void Server::cleanup()
{
    for (SOCKET i = 0; i <= _maxSocket; i++)
    {
        if (FD_ISSET(i, &_masterSet))
        {
            closesocket(i);
        }
    }
    WSACleanup();
}
