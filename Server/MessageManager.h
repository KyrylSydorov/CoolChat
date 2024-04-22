// Kyryl Sydorov, 2024

#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

struct Message
{
    int sender = -1;
    std::string message;
    time_t time;
};

inline std::ostream& operator<<(std::ostream &out, const Message &message)
{
    return out << message.sender << ' ' << message.time << message.message;
}

inline std::istream& operator>>(std::istream &in, Message &message)
{
    in >> message.sender >> message.time;
    std::getline(in, message.message);
    return in;
}

class MessageManager
{
public:
    static void sendMessage(int sender, int receiver, const std::string& message);
    static Message getMessage(int sender, int receiver, int messageId);
    static std::vector<Message> getMessages(int sender, int receiver, int fromId);
    static std::vector<int> getContacts(int userId);

private:
    static void newContact(int sender, int receiver);
};
