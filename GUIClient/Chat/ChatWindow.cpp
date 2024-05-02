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
    connect(_ui->exitButton, &QPushButton::clicked, this, &QChatWindow::handleExitButtonPressed);
    setupHeader();
    rebuildDialogs();

    hideExitButton();
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

    _client.updateDialogs();
    const std::vector<Dialog>& dialogs = _client.getDialogs();

    _dialogsLayout = new QVBoxLayout(_ui->dialogsScrollWidget);
    
    for (size_t i = 0; i < dialogs.size(); ++i)
    {
        if (dialogs[i].state.totalMessages <= 0 || dialogs[i].messages.empty())
        {
            continue;
        }

        QDialogWidget* dialog = new QDialogWidget(dialogs[i], i, this);
        _dialogsLayout->addWidget(dialog);

        connect(dialog, &QDialogWidget::onClicked, this, &QChatWindow::handleDialogClicked);
    }

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _dialogsLayout->addSpacerItem(verticalSpacer);
}

void QChatWindow::clearDialogs()
{
    if (_dialogsLayout)
    {
        while (QLayoutItem* item = _dialogsLayout->takeAt(0))
        {
            delete item->widget();
            delete item;
        }
    }
    
    delete _dialogsLayout;
    _dialogsLayout = nullptr;
}

void QChatWindow::rebuildMessages()
{
    clearMessages();

    const int contactLocalId = static_cast<int>(_selectedDialog->getId());

    _client.cleanCachedMessages(contactLocalId);
    _client.updateMessages(contactLocalId);
    const Dialog& dialog = _client.getDialogs()[contactLocalId];

    _messagesLayout = new QVBoxLayout(_ui->messagesScrollWidget);

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    _messagesLayout->addSpacerItem(verticalSpacer);

    for (size_t i = 0; i < dialog.messages.size(); ++i)
    {
        const Message& message = dialog.messages[i];
        const bool isOutgoing = message.sender == _client.getCurrentUser().hashedNickname;
        
        QWidget* widget = new QWidget(this);
        QMessageWidget* messageWidget = new QMessageWidget(message, widget);

        QHBoxLayout* layout = new QHBoxLayout(widget);
        
        if (isOutgoing)
        {
            QSpacerItem* horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout->addSpacerItem(horizontalSpacer);
            messageWidget->stylizeAsOutgoing();
        }
        
        layout->addWidget(messageWidget);

        if (!isOutgoing)
        {
            QSpacerItem* horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            layout->addSpacerItem(horizontalSpacer);
            messageWidget->stylizeAsIncoming();
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
    if (_messagesLayout)
    {
        while (QLayoutItem* item = _messagesLayout->takeAt(0))
        {
            delete item->widget();
            delete item;
        }
    }
    
    delete _messagesLayout;
    _messagesLayout = nullptr;
}

void QChatWindow::createInputWidget()
{
    _inputWidget = new QInputWidget(this);
    _ui->inputLayout->addWidget(_inputWidget, 1);

    connect(_inputWidget, &QInputWidget::messageSent, this, &QChatWindow::handleMessageSent);
}

void QChatWindow::destroyInputWidget()
{
    delete _inputWidget;
    _inputWidget = nullptr;
}

void QChatWindow::showExitButton()
{
    _ui->exitButton->setVisible(true);
}

void QChatWindow::hideExitButton()
{
    _ui->exitButton->setVisible(false);
}

void QChatWindow::handleDialogClicked(QDialogWidget* dialog)
{
    if (_selectedDialog)
    {
        _selectedDialog->deselect();
    }

    dialog->select();
    _selectedDialog = dialog;
    rebuildMessages();

    if (!_inputWidget)
    {
        createInputWidget();
    }

    showExitButton();
}

void QChatWindow::handleMessageSent(const std::string& message)
{
    if (!_selectedDialog)
    {
        return;
    }

    const int localId = static_cast<int>(_selectedDialog->getId());
    const int receiverId = _client.getDialogs()[localId].state.contactId;
    _client.sendMessage(receiverId, message);

    rebuildMessages();
}

void QChatWindow::handleExitButtonPressed()
{
    if (_selectedDialog)
    {
        _selectedDialog->deselect();
        _selectedDialog = nullptr;
    }

    clearMessages();
    destroyInputWidget();
    hideExitButton();
}
