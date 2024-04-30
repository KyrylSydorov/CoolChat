#include "ChatWindow.h"
#include "DialogWidget.h"
#include "InputWidget.h"
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

    _inputWidget = new QInputWidget(this);
    _ui->inputLayout->addWidget(_inputWidget, 1);
    
    rebuildDialogs();
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

void QChatWindow::rebuildDialogs()
{
    clearDialogs();

    _dialogsLayout = new QVBoxLayout(_ui->scrollAreaWidgetContents);
    
    for (int i = 0; i < 100; ++i)
    {
        QDialogWidget* dialog = new QDialogWidget(this);
        _dialogsLayout->addWidget(dialog);
    }
}

void QChatWindow::clearDialogs()
{
    delete _dialogsLayout;
    _dialogsLayout = nullptr;
}
