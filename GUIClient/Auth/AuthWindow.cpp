#include "../Connection/Client.h"
#include "AuthWindow.h"
#include "LoginWidget.h"
#include "RegistrationWidget.h"

#include "ui_AuthWindow.h"

#include <QPushButton>

QAuthWindow::QAuthWindow(Client& client, QWidget* parent)
    : QMainWindow{ parent }
    , _ui{ new Ui::AuthWindow() }
    , _client(client)
{
    _ui->setupUi(this);

    connect(_ui->loginButton, &QPushButton::clicked, this, &QAuthWindow::handleLoginButtonClicked);
    connect(_ui->registerButton, &QPushButton::clicked, this, &QAuthWindow::handleRegisterButtonClicked);
}

QAuthWindow::~QAuthWindow()
{
    delete _ui;

    delete _currentWidget;
}

void QAuthWindow::handleLoginButtonClicked()
{
    _ui->buttonsWidget->setVisible(false);

    QLoginWidget* loginWidget = new QLoginWidget(_client, this);
    _currentWidget = loginWidget;
    loginWidget->showNormal();

    connect(loginWidget, &QLoginWidget::finished, this, &QAuthWindow::handleLoginFinished);
}

void QAuthWindow::handleRegisterButtonClicked()
{
    _ui->buttonsWidget->setVisible(false);

    QRegistrationWidget* registrationWidget = new QRegistrationWidget(_client, this);
    _currentWidget = registrationWidget;
    registrationWidget->showNormal();

    connect(registrationWidget, &QRegistrationWidget::finished, this, &QAuthWindow::handleLoginFinished);
}

void QAuthWindow::handleLoginFinished(const bool success)
{
    delete _currentWidget;
    _currentWidget = nullptr;

    if (success)
    {
        // trigger auth done
    }
    else
    {
        _ui->buttonsWidget->setVisible(true);
    }
}
