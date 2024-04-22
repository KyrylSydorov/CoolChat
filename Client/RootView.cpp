// Kyryl Sydorov, 2024

#include <iostream>
#include <string>
#include <sstream>

#include "RootView.h"
#include "Client.h"
#include "../Server/Constants.h"
#include "../Server/UserManager.h"

using namespace std;

RootView::RootView(Client& client)
    : _client(client)
{
}

void RootView::start()
{
    cout << "Welcome to the chat application! \n";
    authentication();
}

void RootView::authentication()
{
    cout << "Please select an option: \n";
    cout << "1. Login \n";
    cout << "2. Register \n";
    cout << "3. Exit \n";

    int option;
    cin >> option;

    if (option == 1)
    {
        string login;
        cout << "Enter your login: ";
        cin >> login;

        string password;
        cout << "Enter your password: ";
        cin >> password;

        const int error = _client.login(login, password);
        if (error == Errors::success)
        {
            cout << "Successfully logged in! \n";
        }
        else if (error == Errors::invalidCredentials)
        {
            cout << "Invalid credentials! \n";
            authentication();
            return;
        }
        else
        {
            cout << "Internal error! \n";
            authentication();
            return;
        }
    }
    else if (option == 2)
    {
        string nickname;
        cout << "Enter your nickname: ";
        cin >> nickname;

        string password;
        cout << "Enter your password: ";
        cin >> password;

        string firstName;
        cout << "Enter your first name: ";
        cin >> firstName;

        string lastName;
        cout << "Enter your last name: ";
        cin >> lastName;

        string biography;
        cout << "Tell about yourself: ";
        cin >> biography;
        
        const int error = _client.registerUser(UserInfo{ nickname, firstName, lastName, biography, password });
        if (error == Errors::success)
        {
            cout << "Successfully registered! \n";
        }
        else if (error == Errors::invalidCredentials)
        {
            cout << "Invalid credentials! \n";
            authentication();
            return;
        }
        else if (error == Errors::userAlreadyExists)
        {
            cout << "User already exists! \n";
            authentication();
            return;
        }
        else
        {
            cout << "Internal error! \n";
            authentication();
            return;
        }
    }
    else if (option == 3)
    {
        cout << "Goodbye! \n";
        exit(0);
    }
    else
    {
        cout << "Invalid option! \n";
        authentication();
    }

    mainMenu();
}

void RootView::mainMenu()
{
    cout << "Welcome, " << _client.getCurrentUser().firstName << "! \n";

    while (true)
    {
        cout << "Please select an option: \n";
        cout << "1. Contacts \n";
        cout << "2. Open dialog \n";
        cout << "3. Exit \n";

        int option;
        cin >> option;

        if (option == 1)
        {
            contactsMenu();
        }
        else if (option == 2)
        {
            cout << "Enter contact nickname: ";
            string contactNickname;
            cin >> contactNickname;

            const UserInfo contact = _client.getUserInfo(contactNickname);
            if (!contact.isValid())
            {
                cout << "No such user! \n";
                continue;
            }

            _client.newContact(contact);
            chatMenu(_client.getContacts().size() - 1);
        }
        else if (option == 3)
        {
            cout << "Goodbye! \n";
            exit(0);
        }
        else
        {
            cout << "Invalid option! \n";
        }
    }
}

void RootView::contactsMenu()
{
    std::vector<UserInfo> contacts = _client.getContacts();
    if (contacts.empty())
    {
        cout << "You have no contacts!" << endl;
        return;
    }

    cout << "\nYour contacts:" << endl;
    
    for (size_t i = 0; i < contacts.size(); ++i)
    {
        const UserInfo& contact = contacts[i];
        cout << '[' << i << "]\t" << contact.nickname << " :\t" << contact.firstName << ' ' << contact.lastName << endl;
    }

    cout << "Press -1 to return back. Or press number to contact the person \n";

    int option;
    cin >> option;

    if (option == -1)
    {
        return;
    }

    if (option < 0 || option >= contacts.size())
    {
        cout << "Invalid option! Returning back \n";
        return;
    }

    chatMenu(option);
}

void RootView::printMessages(const vector<Message>& messages, const std::string& contactName)
{
    for (const Message& message : messages)
    {
        string time;
        time.resize(26);

        ctime_s(time.data(), time.size(), &message.time);

        string sender = message.sender == _client.getCurrentUser().hashedNickname ? "You" : contactName;
        
        cout
            << time << sender << ": " << message.message << "\n\n" << endl;
    }
}

void RootView::chatMenu(int contactId)
{
    const UserInfo& contact = _client.getContacts()[contactId];
    cout << "Chat with " << contact.firstName << ' ' << contact.lastName << " \n\n\n";

    cout << "Type message and press enter to submit. Type :q to return back. Type :r to refresh \n";

    vector<Message> messages = _client.getMessages(contact.hashedNickname, 0);
    int lastId = messages.size();
    
    printMessages(messages, contact.firstName);

    while (true)
    {
        string option;
        getline(cin, option);

        if (option == ":q")
        {
            return;
        }

        if (option == ":r")
        {
            messages = _client.getMessages(contact.hashedNickname, lastId);
            lastId += messages.size();
            printMessages(messages, contact.firstName);
            continue;
        }

        if (option.empty())
        {
            continue;
        }

        _client.sendMessage(contact.hashedNickname, option);
    }
}
