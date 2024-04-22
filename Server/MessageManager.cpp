// Kyryl Sydorov, 2024

#include <chrono>
#include <filesystem>
#include <fstream>

#include "Constants.h"
#include "MessageManager.h"

using namespace std;

void MessageManager::sendMessage(int sender, int receiver, const std::string& message)
{
    const int originalSender = sender;
    
    if (sender > receiver)
    {
        swap(sender, receiver);
    }
    
    const string messageDir = Paths::messagesPath + '\\' + to_string(sender) + '_' + to_string(receiver);
    std::filesystem::create_directories(messageDir);

    const size_t messagesNum = distance(filesystem::directory_iterator(messageDir), filesystem::directory_iterator{});

    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    Message messageObj{ originalSender, message, now };
    
    ofstream out(messageDir + '\\' + to_string(messagesNum));
    if (!out.is_open())
    {
        return;
    }

    out << messageObj;

    if (messagesNum == 0)
    {
        newContact(sender, receiver);
        newContact(receiver, sender);
    }
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
    if (sender > receiver)
    {
        swap(sender, receiver);
    }

    const string messageDir = Paths::messagesPath + '\\' + to_string(sender) + '_' + to_string(receiver);

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

std::vector<int> MessageManager::getContacts(int userId)
{
    const string contactsDir = Paths::usersPath + "\\C" + to_string(userId);

    ifstream contacts(contactsDir + "\\contacts");
    if (!contacts.is_open())
    {
        return {};
    }

    vector<int> contactsVec;
    int contact;
    while (contacts >> contact)
    {
        contactsVec.push_back(contact);
    }

    return contactsVec;
}

void MessageManager::newContact(int sender, int receiver)
{
    const string contactsDir = Paths::usersPath + "\\C" + to_string(sender);
    std::filesystem::create_directories(contactsDir);

    ofstream contact1(contactsDir + "\\contacts", ios::app);
    if (contact1.is_open())
    {
        contact1 << receiver << '\n';
    }
}
