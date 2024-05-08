// Kyryl Sydorov, 2024

#include <chrono>
#include <filesystem>
#include <fstream>

#include "Constants.h"
#include "MessageManager.h"

using namespace std;

void MessageManager::sendMessage(int sender, int receiver, const std::string& message)
{
    const time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    const Message messageObj{ sender, message, now };

    const int messageId = saveMessage(receiver, messageObj);
    if (messageId == -1)
    {
        std::cout << "Failed to save message" << std::endl;
        return;
    }
    
    if (messageId == 0)
    {
        newContact(sender, receiver);
        newContact(receiver, sender);
        saveDialogState({ receiver, sender, -1 });
    }

    saveDialogState({ sender, receiver, static_cast<int>(messageId) });    
}

Message MessageManager::getMessage(int sender, int receiver, int messageId)
{
    if (sender > receiver)
    {
        swap(sender, receiver);
    }

    const string messageDir = Paths::messagesPath + '\\' + to_string(sender) + '_' + to_string(receiver);
    
    ifstream in(messageDir + '\\' + to_string(messageId));
    if (!in.is_open())
    {
        return {};
    }

    Message message;
    in >> message;
    return message;
}

std::vector<Message> MessageManager::getMessages(int sender, int receiver, int fromId)
{
    vector<Message> messages;

    while (true)
    {
        Message message = getMessage(sender, receiver, fromId);
        if (message.sender == -1)
        {
            break;
        }

        messages.push_back(message);
        ++fromId;
    }

    return messages;
}

DialogState MessageManager::getDialogState(int userId, int contactId)
{
    DialogState dialogState{ userId, contactId };
    const string contactsDir = Paths::usersPath + "\\C" + to_string(dialogState.userId);
    std::filesystem::create_directories(contactsDir);

    ifstream stream(contactsDir + "\\" + to_string(dialogState.contactId));
    
    if (stream.is_open())
    {
        stream >> dialogState.lastReadMessageId;
    }
    dialogState.totalMessages = getMessagesNum(userId, contactId);
    
    return dialogState;
}

void MessageManager::saveDialogState(const DialogState& dialogState)
{
    const string contactsDir = Paths::usersPath + "\\C" + to_string(dialogState.userId);
    std::filesystem::create_directories(contactsDir);

    ofstream stream(contactsDir + "\\" + to_string(dialogState.contactId));
    if (stream.is_open())
    {
        stream << dialogState.lastReadMessageId;
    }
}

std::vector<DialogState> MessageManager::getDialogStates(int userId)
{
    const string contactsDir = Paths::usersPath + "\\C" + to_string(userId);

    ifstream contacts(contactsDir + "\\contacts");
    if (!contacts.is_open())
    {
        return {};
    }

    vector<DialogState> dialogs;
    
    int contactId;
    while (contacts >> contactId)
    {
        dialogs.emplace_back(getDialogState(userId, contactId));
    }

    return dialogs;
}

Message MessageManager::getLastMessage(int sender, int receiver)
{
    if (sender > receiver)
    {
        swap(sender, receiver);
    }

    const string messageDir = Paths::messagesPath + '\\' + to_string(sender) + '_' + to_string(receiver);
    std::filesystem::create_directories(messageDir);

    const size_t messagesNum = distance(filesystem::directory_iterator(messageDir), filesystem::directory_iterator{});
    return getMessage(sender, receiver, static_cast<int>(messagesNum) - 1);
}

int MessageManager::saveMessage(int receiver, const Message& message)
{
    const int dirFirst = std::min(message.sender, receiver);
    const int dirSecond = std::max(message.sender, receiver);
    
    const string messageDir = Paths::messagesPath + '\\' + to_string(dirFirst) + '_' + to_string(dirSecond);
    std::filesystem::create_directories(messageDir);

    const size_t messagesNum = distance(filesystem::directory_iterator(messageDir), filesystem::directory_iterator{});
    
    ofstream out(messageDir + '\\' + to_string(messagesNum));
    if (!out.is_open())
    {
        return -1;
    }

    out << message;

    return static_cast<int>(messagesNum);
}

void MessageManager::newContact(int sender, int receiver)
{
    const string contactsDir = Paths::usersPath + "\\C" + to_string(sender);
    std::filesystem::create_directories(contactsDir);

    ofstream contact(contactsDir + "\\contacts", ios::app);
    if (contact.is_open())
    {
        contact << receiver << '\n';
    }
}

int MessageManager::getMessagesNum(int userA, int userB)
{
    if (userA > userB)
    {
        swap(userA, userB);
    }

    const string messageDir = Paths::messagesPath + '\\' + to_string(userA) + '_' + to_string(userB);
    std::filesystem::create_directories(messageDir);

    return static_cast<int>(distance(filesystem::directory_iterator(messageDir), filesystem::directory_iterator{}));
}
