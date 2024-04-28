#include "RegistrationWidget.h"

#include "ui_RegistrationWidget.h"

#include "../Connection/Client.h"
#include "../../Server/Constants.h"

QRegistrationWidget::QRegistrationWidget(Client& client, QWidget* parent)
    : QWidget{ parent }
    , _ui{ new Ui::RegistrationWidget }
    , _client(client)
{
    _ui->setupUi(this);

    connect(_ui->registerButton, &QPushButton::clicked, this, &QRegistrationWidget::handleRegisterButtonClicked);
    connect(_ui->backButton, &QPushButton::clicked, [this]() { emit finished(false); });

    setTabOrder({
        _ui->nicknameEdit,
        _ui->passwordEdit,
        _ui->password2Edit,
        _ui->fnameEdit,
        _ui->lnameEdit,
        _ui->bioEdit,
        _ui->registerButton,
        _ui->backButton
    });
}

QRegistrationWidget::~QRegistrationWidget()
{
    delete _ui;
}

void QRegistrationWidget::handleRegisterButtonClicked()
{
    _ui->errorLabel->setText("");

    if (_ui->passwordEdit->text() != _ui->password2Edit->text())
    {
        _ui->errorLabel->setText("Passwords do not match");
        return;
    }

    if (_ui->passwordEdit->text().length() < Constants::minPasswordLength)
    {
        _ui->errorLabel->setText("Password should have at least 8 characters");
        return;
    }

    if (_ui->nicknameEdit->text().length() < Constants::minNameLength)
    {
        _ui->errorLabel->setText("Nickname is too short");
        return;
    }

    if (_ui->fnameEdit->text().length() < Constants::minNameLength)
    {
        _ui->errorLabel->setText("First name is too short");
        return;
    }

    if (_ui->lnameEdit->text().length() < Constants::minNameLength)
    {
        _ui->errorLabel->setText("Last name is too short");
        return;
    }

    if (_ui->bioEdit->text().length() == 0)
    {
        _ui->errorLabel->setText("Tell us more about yourself, please");
        return;
    }

    UserInfo userInfo;
    userInfo.nickname = _ui->nicknameEdit->text().toStdString();
    userInfo.password = _ui->passwordEdit->text().toStdString();
    userInfo.firstName = _ui->fnameEdit->text().toStdString();
    userInfo.lastName = _ui->lnameEdit->text().toStdString();
    userInfo.biography = _ui->bioEdit->text().toStdString();

    int response = _client.registerUser(userInfo);
    if (response == Errors::success)
    {
        emit finished(true);
        return;
    }
    
    if (response == Errors::invalidCredentials)
    {
        _ui->errorLabel->setText("Invalid credentials!");
        _ui->passwordEdit->setText("");
        _ui->password2Edit->setText("");
        return;
    }

    if (response == Errors::internalError)
    {
        _ui->errorLabel->setText("Internal error");
        _ui->passwordEdit->setText("");
        _ui->password2Edit->setText("");
        return;
    }

    if (response == Errors::userAlreadyExists)
    {
        _ui->errorLabel->setText("User already exists");
        _ui->nicknameEdit->setText("");
        _ui->passwordEdit->setText("");
        _ui->password2Edit->setText("");
        return;
    }
}
