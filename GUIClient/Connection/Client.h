// Kyryl Sydorov, 2024

#pragma once

#include <vector>
#include <winsock2.h>
#include <shared_mutex>

#include "../../Server/UserManager.h"
#include "../../Server/MessageManager.h"

struct Dialog
{
    UserInfo contact;
    DialogState state;
    std::vector<Message> messages;
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

    void updateMessages(int contactLocalId);
    std::vector<Message> getMessages(int senderId, int fromId);

    void cleanCachedMessages(int contactLocalId);

    const std::vector<Dialog>& getDialogs() const;
    
private:
    std::string serverRequest(std::string request);
    
    void initializeClientSocket();

    void fetchDialogs();
    void fetchLastMessages();
    void fetchContacts();

    SOCKET _socket = INVALID_SOCKET;

    UserInfo _currentUser;
    
    std::vector<Dialog> _cachedDialogs;

    mutable std::shared_mutex _rwLock;
};
