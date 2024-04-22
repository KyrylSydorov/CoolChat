// Kyryl Sydorov, 2024

#pragma once

#include <vector>
#include <winsock2.h>

#include "../Server/UserManager.h"
#include "../Server/MessageManager.h"

struct UserInfo;

class Client
{
public:
    Client();

    void start();

    int login(const std::string& username, const std::string& password);
    int registerUser(const UserInfo& userInfo);

    void newContact(const UserInfo& userInfo);
    const std::vector<UserInfo>& getContacts();

    const UserInfo& getCurrentUser() const;
    UserInfo getUserInfo(int id);
    UserInfo getUserInfo(const std::string& nickname);

    void sendMessage(int receiverId, const std::string& message);

    std::vector<Message> getMessages(int senderId, int fromId);
    
private:
    
    std::string serverRequest(std::string request);
    
    void initializeClientSocket();

    SOCKET _socket = INVALID_SOCKET;

    UserInfo _currentUser;
    std::vector<UserInfo> _contacts;
};
