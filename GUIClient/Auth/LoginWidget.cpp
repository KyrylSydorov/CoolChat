#include "LoginWidget.h"

#include "ui_LoginWidget.h"

#include "../Connection/Client.h"
#include "../../Server/Constants.h"

QLoginWidget::QLoginWidget(Client& client, QWidget* parent)
    : QWidget{ parent }
    , _ui{ new Ui::LoginWidget }
    , _client(client)
{
    _ui->setupUi(this);

    connect(_ui->loginButton, &QPushButton::clicked, this, &QLoginWidget::handleLoginButtonClicked);
    connect(_ui->backButton, &QPushButton::clicked, [this]() { emit finished(false); });

    setTabOrder({ _ui->nicknameEdit, _ui->passwordEdit, _ui->loginButton, _ui->backButton });
}

QLoginWidget::~QLoginWidget()
{
    delete _ui;
}

void QLoginWidget::handleLoginButtonClicked()
{
    _ui->errorLabel->setText("");

    int response = _client.login(_ui->nicknameEdit->text().toStdString(), _ui->passwordEdit->text().toStdString());
    if (response == Errors::success)
    {
        emit finished(true);
        return;
    }
    
    if (response == Errors::invalidCredentials)
    {
        _ui->errorLabel->setText("Invalid credentials");
        _ui->passwordEdit->setText("");
        return;
    }

    if (response == Errors::internalError)
    {
        _ui->errorLabel->setText("Internal error");
        _ui->passwordEdit->setText("");
        return;
    }
}
