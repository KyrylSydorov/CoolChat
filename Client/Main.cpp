// Kyryl Sydorov, 2024

#include <future>
#include <vector>
#include "Client.h"
#include "RootView.h"

using namespace std;

void startAsUser()
{
    Client client;
    client.start();

    RootView rootView(client);
    rootView.start();
}

int randomReceiver()
{
    const string login = "login" + to_string(rand() % 1000);
    return hashString(login);
}

void startTests()
{
    static int seed = 0;
    for (int k = 0; k < 1000; ++k)
    {
        vector<future<void>> futures;
        for (int i = 0; i < 50; ++i)
        {
            futures.push_back(async(launch::async, [i]()
            {
                // random string
                string login = "login" + to_string(seed++);
                {
                    Client client;
                    client.start();
                    client.registerUser({ login, "login", login, "biography", login });

                    for (int j = 0; j < 100; ++j)
                    {
                        client.sendMessage(randomReceiver(), "Hello, world!");
                        client.sendMessage(randomReceiver(), "Message2!");
                        client.sendMessage(randomReceiver(), "Message3!");
                        client.sendMessage(randomReceiver(), "Message4!");
                        client.sendMessage(randomReceiver(), "Message5!");
                        client.sendMessage(randomReceiver(), "Message6!");
                        client.sendMessage(randomReceiver(), "Message7!");
                        client.sendMessage(randomReceiver(), "Message8!");
                        client.sendMessage(randomReceiver(), "Message9!");
                        client.sendMessage(randomReceiver(), "Message10!");
                    }
                }
                Sleep(rand() % 500);
                {
                    Client client;
                    client.start();
                    client.login(login, login);
                    for (int j = 0; j < 100; ++j)
                    {
                        client.getContacts();
                        Sleep(rand() % 100);
                        client.getMessages(client.getContacts()[rand() % 10].hashedNickname, 0);
                        client.getMessages(client.getContacts()[rand() % 10].hashedNickname, 0);
                        client.getMessages(client.getContacts()[rand() % 10].hashedNickname, 0);
                        client.getMessages(client.getContacts()[rand() % 10].hashedNickname, 0);
                        client.getMessages(client.getContacts()[rand() % 10].hashedNickname, 0);
                        client.getMessages(client.getContacts()[rand() % 10].hashedNickname, 0);
                    }
                
                }
            }));
        }
    }

//for (auto& future : futures)
//{
//    future.get();
//}

    cout << "All tests passed!" << endl;
}

int main(int argc, char* argv[])
{
#if 0
    startAsUser();
#else
    startTests();
#endif
}
