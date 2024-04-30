#include "ChatWindow.h"
#include "DialogWidget.h"
#include "InputWidget.h"
#include "MessageWidget.h"
#include "../Connection/Client.h"

#include "ui_ChatWindow.h"

#include <QPushButton>
#include <QScrollBar>
#include <QTimer>

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
    rebuildMessages();

    setupHeader();
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

void QChatWindow::setupHeader()
{
    const UserInfo& userInfo = _client.getCurrentUser();
    _ui->nameLabel->setText((userInfo.firstName + " " + userInfo.lastName).c_str());
    _ui->nicknameLabel->setText(("@" + userInfo.nickname).c_str());
}

void QChatWindow::rebuildDialogs()
{
    clearDialogs();

    const std::vector<Dialog>& dialogs = _client.getDialogs();

    _dialogsLayout = new QVBoxLayout(_ui->dialogsScrollWidget);
    
    for (size_t i = 0; i < dialogs.size(); ++i)
    {
        QDialogWidget* dialog = new QDialogWidget(dialogs[i], i, this);
        _dialogsLayout->addWidget(dialog);
    }

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _dialogsLayout->addSpacerItem(verticalSpacer);
}

void QChatWindow::clearDialogs()
{
    delete _dialogsLayout;
    _dialogsLayout = nullptr;
}

void QChatWindow::rebuildMessages()
{
    clearMessages();

    _messagesLayout = new QVBoxLayout(_ui->messagesScrollWidget);

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _messagesLayout->addSpacerItem(verticalSpacer);

    for (int i = 0; i < 10; ++i)
    {
        QWidget* widget = new QWidget(this);
        QMessageWidget* message = new QMessageWidget(this);

        QHBoxLayout* layout = new QHBoxLayout(widget);
        
        if (i % 2 == 0)
        {
            QSpacerItem* horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout->addSpacerItem(horizontalSpacer);
            message->stylizeAsOutgoing();
        }
        
        layout->addWidget(message);

        if (i % 2 == 1)
        {
            QSpacerItem* horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout->addSpacerItem(horizontalSpacer);
            message->stylizeAsIncoming();
        }
        
        _messagesLayout->addWidget(widget);
    }

    // Scroll messages to the bottom
    QTimer::singleShot(0, this, [&]()
    {
        QScrollBar* bar = _ui->messagesScrollArea->verticalScrollBar();
        bar->setValue(bar->maximum());
    });
}

void QChatWindow::clearMessages()
{
    delete _messagesLayout;
    _messagesLayout = nullptr;
}
