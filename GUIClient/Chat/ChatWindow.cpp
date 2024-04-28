#include "ChatWindow.h"
#include "../Connection/Client.h"

#include "ui_ChatWindow.h"

#include <QPushButton>

QChatWindow::QChatWindow(Client& client, QWidget *parent)
    : QMainWindow{ parent }
    , _ui{ new Ui::ChatWindow }
    , _client(client)
{
    _ui->setupUi(this);

    connect(_ui->logoutButton, &QPushButton::clicked, this, &QChatWindow::handleLogoutButtonPressed);
}

QChatWindow::~QChatWindow()
{
    delete _ui;
}

void QChatWindow::handleLogoutButtonPressed()
{
    _client.logout();
    emit logout();
}
