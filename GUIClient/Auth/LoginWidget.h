#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui
{
    class LoginWidget;
}

class Client;

class QLoginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QLoginWidget(Client& client, QWidget* parent = nullptr);
    ~QLoginWidget();

public slots:
    void handleLoginButtonClicked();

signals:
    void finished(const bool success);

private:
    Ui::LoginWidget* _ui;
    Client& _client;
};

#endif // LOGINWIDGET_H
