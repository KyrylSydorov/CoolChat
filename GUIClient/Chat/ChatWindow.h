#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>

namespace Ui
{
class ChatWindow;
}

class Client;

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
    Ui::ChatWindow* _ui;
    Client& _client;
};

#endif // CHATWINDOW_H
