#include "InputWidget.h"

#include "ui_InputWidget.h"

QInputWidget::QInputWidget(QWidget* parent)
    : QWidget{ parent }
    , _ui{ new Ui::InputWidget() }
{
    _ui->setupUi(this);

    connect(_ui->pushButton, &QPushButton::clicked, [this]()
    {
        emit messageSent(_ui->messageEdit->toPlainText().toStdString());
        _ui->messageEdit->clear();
    });
}

QInputWidget::~QInputWidget()
{
    delete _ui;
}
