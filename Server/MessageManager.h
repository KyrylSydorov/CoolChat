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

struct DialogState
{
    int userId = -1;
    int contactId = -1;
    int lastReadMessageId = -1;
    int totalMessages = 0;
};

inline std::ostream& operator<<(std::ostream &out, const DialogState& dialogState)
{
    return out << dialogState.userId << ' ' << dialogState.contactId << ' ' << dialogState.lastReadMessageId << ' ' << dialogState.totalMessages;
}

inline std::istream& operator>>(std::istream &in, DialogState& dialogState)
{
    return in >> dialogState.userId >> dialogState.contactId >> dialogState.lastReadMessageId >> dialogState.totalMessages;
}

class MessageManager
{
public:
    static void sendMessage(int sender, int receiver, const std::string& message);
    static Message getMessage(int sender, int receiver, int messageId);
    static std::vector<Message> getMessages(int sender, int receiver, int fromId);

    static DialogState getDialogState(int userId, int contactId);
    static void saveDialogState(const DialogState& dialogState);

    static std::vector<DialogState> getDialogStates(int userId);

    static Message getLastMessage(int sender, int receiver);

private:
    static int saveMessage(int receiver, const Message& message);
    static void newContact(int sender, int receiver);
    static int getMessagesNum(int userA, int userB);
};
