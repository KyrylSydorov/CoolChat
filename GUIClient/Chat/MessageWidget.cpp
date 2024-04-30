#include "MessageWidget.h"

#include "ui_MessageWidget.h"

QMessageWidget::QMessageWidget(QWidget *parent)
    : QWidget{ parent }
    , _ui{ new Ui::MessageWidget() }
{
    _ui->setupUi(this);
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
