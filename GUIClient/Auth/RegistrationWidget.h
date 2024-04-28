#ifndef REGISTRATIONWIDGET_H
#define REGISTRATIONWIDGET_H

#include <QWidget>

namespace Ui
{
    class RegistrationWidget;
}

class Client;

class QRegistrationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QRegistrationWidget(Client& client, QWidget* parent = nullptr);
    ~QRegistrationWidget();

public slots:
    void handleRegisterButtonClicked();

signals:
    void finished(const bool success);

private:
    Ui::RegistrationWidget* _ui;
    Client& _client;
};

#endif // REGISTRATIONWIDGET_H
