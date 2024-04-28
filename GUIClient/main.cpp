#include "Auth/AuthWindow.h"
#include "Chat/ChatWindow.h"
#include "Connection/Client.h"

#include <QApplication>

static QAuthWindow* gAuthWindow = nullptr;
static QChatWindow* gChatWindow = nullptr;

void showChatWindow(Client& client);

void showAuthWindow(Client& client)
{
    gAuthWindow = new QAuthWindow(client);
    gAuthWindow->show();

    gAuthWindow->connect(gAuthWindow, &QAuthWindow::authDone, [&]()
    {
        gAuthWindow->hide();
        delete gAuthWindow;
        gAuthWindow = nullptr;

        showChatWindow(client);
    });
}

void showChatWindow(Client& client)
{
    gChatWindow = new QChatWindow(client);
    gChatWindow->show();

    gChatWindow->connect(gChatWindow, &QChatWindow::logout, [&]()
    {
        gChatWindow->hide();
        delete gChatWindow;
        gChatWindow = nullptr;

        showAuthWindow(client);
    });
}

int main(int argc, char *argv[])
{
    Client client;
    client.start();

    QApplication application(argc, argv);
    showAuthWindow(client);

    return application.exec();
}
