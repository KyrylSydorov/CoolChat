#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class AuthWindow;
}

class Client;

class QAuthWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit QAuthWindow(Client& client, QWidget* parent = nullptr);
    ~QAuthWindow();

signals:
    void authDone();

private slots:
    void handleLoginButtonClicked();
    void handleRegisterButtonClicked();

    void handleLoginFinished(const bool success);
    
private:
    Ui::AuthWindow* _ui;
    Client& _client;

    QWidget* _currentWidget;
};

#endif // AUTHWINDOW_H
