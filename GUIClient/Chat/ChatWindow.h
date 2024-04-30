#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>


class QVBoxLayout;

namespace Ui
{
class ChatWindow;
}

class Client;
class QInputWidget;

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

private:
    void setupHeader();
    
    void rebuildDialogs();
    void clearDialogs();

    void rebuildMessages();
    void clearMessages();
    
    Ui::ChatWindow* _ui;
    Client& _client;

    QInputWidget* _inputWidget;

    QVBoxLayout* _dialogsLayout = nullptr;
    QVBoxLayout* _messagesLayout = nullptr;
};

#endif // CHATWINDOW_H
