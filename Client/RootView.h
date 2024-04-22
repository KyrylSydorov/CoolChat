// Kyryl Sydorov, 2024

#pragma once

#include <vector>

class Client;
struct Message;

class RootView
{
public:
    RootView(Client& client);

    void start();

private:
    void authentication();
    void mainMenu();
    void contactsMenu();
    void chatMenu(int contactId);

    void printMessages(const std::vector<Message>& messages, const std::string& contactName);
    
    Client& _client;
};
