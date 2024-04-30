// Kyryl Sydorov, 2024

#pragma once

#include <vector>
#include <winsock2.h>

#include "../../Server/UserManager.h"
#include "../../Server/MessageManager.h"

struct Dialog
{
    UserInfo userInfo;
    std::vector<Message> messages;
    int lastReadMessageId = -1;
};

class Client
{
public:
    Client();

    void start();

    int login(const std::string& username, const std::string& password);
    int registerUser(const UserInfo& userInfo);
    void logout();

    void updateDialogs();

    void newContact(const UserInfo& userInfo);

    const UserInfo& getCurrentUser() const;
    UserInfo getUserInfo(int id);
    UserInfo getUserInfo(const std::string& nickname);

    void sendMessage(int receiverId, const std::string& message);

    std::vector<Message> getMessages(int senderId, int fromId);

    const std::vector<Dialog>& getDialogs() const;
    
private:
    std::string serverRequest(std::string request);
    
    void initializeClientSocket();

    SOCKET _socket = INVALID_SOCKET;

    UserInfo _currentUser;
    
    std::vector<Dialog> _cachedDialogs;
};
