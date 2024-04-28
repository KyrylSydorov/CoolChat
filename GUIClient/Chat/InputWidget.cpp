#include "InputWidget.h"

#include "ui_InputWidget.h"

QInputWidget::QInputWidget(QWidget* parent)
    : QWidget{ parent }
    , _ui{ new Ui::InputWidget() }
{
    _ui->setupUi(this);
}

QInputWidget::~QInputWidget()
{
    delete _ui;
}
