#include "MessageWidget.h"

#include "../../Server/MessageManager.h"

#include "ui_MessageWidget.h"

QMessageWidget::QMessageWidget(const Message& message, QWidget *parent)
    : QWidget{ parent }
    , _ui{ new Ui::MessageWidget() }
{
    _ui->setupUi(this);

    _ui->messageLabel->setText(message.message.c_str());
    
    std::string time;
    time.resize(26);
    ctime_s(time.data(), time.capacity(), &message.time);
    
    _ui->timeLabel->setText(time.c_str());
}

QMessageWidget::~QMessageWidget()
{
    delete _ui;
}

void QMessageWidget::stylizeAsIncoming()
{
    _ui->groupBox->setStyleSheet("background : rgb(255,255,255);");
    _ui->messageLabel->setAlignment(Qt::AlignLeft);
}

void QMessageWidget::stylizeAsOutgoing()
{
    _ui->groupBox->setStyleSheet("background : rgb(17,127,176);");
    _ui->messageLabel->setAlignment(Qt::AlignRight);
}
