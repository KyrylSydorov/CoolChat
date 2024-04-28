#include "Auth/AuthWindow.h"
#include "Connection/Client.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    Client client;
    client.start();

    QApplication application(argc, argv);

    QAuthWindow authWindow(client);
    authWindow.show();

    return application.exec();
}
