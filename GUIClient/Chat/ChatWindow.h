#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QThread>

class QMessagesUpdateWorker;
class QDialogUpdateWorker;

namespace Ui
{
class ChatWindow;
}

class QDialogWidget;
class QInputWidget;
class Client;
class QVBoxLayout;

class QChatWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit QChatWindow(Client& client, QWidget* parent = nullptr);
    ~QChatWindow();

signals:
    void logout();

private slots:
    void handleLogoutButtonPressed();
    void handleDialogClicked(QDialogWidget* dialog);
    void handleMessageSent(const std::string& message);
    void handleExitButtonPressed();
    void handleNewChatButtonPressed();

private:
    void setupUpdateThreads();
    
    void setupHeader();
    
    void rebuildDialogs();
    void clearDialogs();

    void rebuildMessages();
    void clearMessages();

    void createInputWidget();
    void destroyInputWidget();

    void showExitButton();
    void hideExitButton();

    void showNewChatWidget();
    void hideNewChatWidget();
    
    Ui::ChatWindow* _ui;
    Client& _client;

    QInputWidget* _inputWidget = nullptr;

    QVBoxLayout* _dialogsLayout = nullptr;
    QVBoxLayout* _messagesLayout = nullptr;

    QDialogWidget* _selectedDialog = nullptr;

    QThread* _dialogUpdateThread = nullptr;
    QDialogUpdateWorker* _dialogUpdateWorker = nullptr;

    QThread* _messageUpdateThread = nullptr;
    QMessagesUpdateWorker* _messageUpdateWorker = nullptr;
};

#endif // CHATWINDOW_H
