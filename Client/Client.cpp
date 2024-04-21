// Kyryl Sydorov, 2024

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

SOCKET initializeClientSocket()
{
    const SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    sockaddr_in serverInfo;
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(12345); // Server's listening port
    if (inet_pton(AF_INET, "127.0.0.1", &serverInfo.sin_addr) <= 0)
    {  
        std::cerr << "Invalid address/ Address not supported " << std::endl;
        closesocket(connectSocket);
        return INVALID_SOCKET;
    }

    if (connect(connectSocket, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) < 0)
    {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(connectSocket);
        return INVALID_SOCKET;
    }

    return connectSocket;
}

int main(int argc, char* argv[])
{
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    const SOCKET connectSocket = initializeClientSocket();
    if (connectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        return 1;
    }

    while (true)
    {
        const char* sendMessage = "Hello, Server!";
        int iResult = send(connectSocket, sendMessage, (int)strlen(sendMessage), 0);
        if (iResult == SOCKET_ERROR)
        {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "Bytes Sent: " << iResult << std::endl;

        char recvbuf[512];
        int recvbuflen = 512;
        iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            std::cout << "Bytes received: " << iResult << "\n";
            std::cout << "Message received: " << std::string(recvbuf, iResult) << std::endl;
        }
        else if (iResult == 0)
        {
            std::cout << "Connection closed\n";
        }
        else
        {
            std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
        }

        Sleep(5000);
    }
    
    // Cleanup
    closesocket(connectSocket);
    WSACleanup();
    return 0;

}
