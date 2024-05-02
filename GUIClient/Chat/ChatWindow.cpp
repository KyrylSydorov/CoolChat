#include "ChatWindow.h"
#include "DialogWidget.h"
#include "InputWidget.h"
#include "MessageWidget.h"
#include "../Connection/Client.h"
#include "../Connection/DialogUpdateWorker.h"
#include "../Connection/MessagesUpdateWorker.h"
#include "../../Server/Constants.h"

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
    connect(_ui->newChatButton, &QPushButton::clicked, this, &QChatWindow::handleNewChatButtonPressed);
    setupHeader();

    hideExitButton();

    setupUpdateThreads();
}

QChatWindow::~QChatWindow()
{
    delete _ui;

    if (_dialogUpdateThread)
    {
        _dialogUpdateWorker->requestStop();
    }
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
    int selectedLocalId = _selectedDialog ? _selectedDialog->getId() : -1;
    clearDialogs();

    _client.updateDialogs();
    const std::vector<Dialog>& dialogs = _client.getDialogs();

    _dialogsLayout = new QVBoxLayout(_ui->dialogsScrollWidget);
    
    for (size_t i = 0; i < dialogs.size(); ++i)
    {
        QDialogWidget* dialog = new QDialogWidget(dialogs[i], i, this);
        _dialogsLayout->addWidget(dialog);

        connect(dialog, &QDialogWidget::onClicked, this, &QChatWindow::handleDialogClicked);

        if (static_cast<int>(i) == selectedLocalId)
        {
            dialog->select();
            _selectedDialog = dialog;
        }
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

    if (!_selectedDialog)
    {
        return;
    }
    
    const int contactLocalId = static_cast<int>(_selectedDialog->getId());
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

void QChatWindow::showNewChatWidget()
{
    _ui->newChatWidget->setVisible(true);
}

void QChatWindow::hideNewChatWidget()
{
    _ui->newChatWidget->setVisible(false);
}

void QChatWindow::handleDialogClicked(QDialogWidget* dialog)
{
    if (_selectedDialog)
    {
        _selectedDialog->deselect();
    }

    dialog->select();
    _selectedDialog = dialog;
    _dialogUpdateWorker->setLocalContactId(static_cast<int>(dialog->getId()));

    if (!_inputWidget)
    {
        createInputWidget();
    }

    showExitButton();
    hideNewChatWidget();
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
}

void QChatWindow::handleExitButtonPressed()
{
    if (_selectedDialog)
    {
        _selectedDialog->deselect();
        _selectedDialog = nullptr;
    }

    _dialogUpdateWorker->setLocalContactId(-1);

    clearMessages();
    destroyInputWidget();
    hideExitButton();

    showNewChatWidget();
}

void QChatWindow::handleNewChatButtonPressed()
{
    _ui->newChatErrorLabel->setText("");
    const std::string nickname = _ui->newChatEdit->toPlainText().toStdString();
    if (nickname.size() < Constants::minNameLength)
    {
        _ui->newChatErrorLabel->setText("Nickname is too short");
        return;
    }

    const UserInfo user = _client.getUserInfo(nickname);
    if (!user.isValid())
    {
        _ui->newChatErrorLabel->setText("User not found");
        return;
    }

    _client.newContact(user);
}

void QChatWindow::setupUpdateThreads()
{
    _dialogUpdateThread = new QThread();
    _dialogUpdateWorker = new QDialogUpdateWorker(_client);
    _dialogUpdateWorker->moveToThread(_dialogUpdateThread);
    connect(_dialogUpdateWorker, &QDialogUpdateWorker::dialogsUpdated, this, &QChatWindow::rebuildDialogs);
    connect(_dialogUpdateWorker, &QDialogUpdateWorker::messagesUpdated, this, &QChatWindow::rebuildMessages);
    connect(_dialogUpdateThread, &QThread::started, _dialogUpdateWorker, &QDialogUpdateWorker::run);
    connect(_dialogUpdateThread, &QThread::finished, _dialogUpdateWorker, &QObject::deleteLater);
    connect(_dialogUpdateWorker, &QDialogUpdateWorker::threadStopped, _dialogUpdateThread, &QThread::quit);
    _dialogUpdateThread->start();
}
