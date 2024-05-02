#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>

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

private:
    void setupHeader();
    
    void rebuildDialogs();
    void clearDialogs();

    void rebuildMessages();
    void clearMessages();

    void createInputWidget();
    void destroyInputWidget();

    void showExitButton();
    void hideExitButton();
    
    Ui::ChatWindow* _ui;
    Client& _client;

    QInputWidget* _inputWidget = nullptr;

    QVBoxLayout* _dialogsLayout = nullptr;
    QVBoxLayout* _messagesLayout = nullptr;

    QDialogWidget* _selectedDialog = nullptr;
};

#endif // CHATWINDOW_H
