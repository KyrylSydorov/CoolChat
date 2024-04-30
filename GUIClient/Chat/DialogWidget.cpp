#include "DialogWidget.h"

#include "ui_DialogWidget.h"

namespace DialogWidget
{
    const char* deselectedStyle = "background : rgb(78,189,217);";
    const char* hoveredStyle = "background : rgb(153,223,231);";
    const char* selectedStyle = "background : rgb(17,127,176);";
    
}

QDialogWidget::QDialogWidget(QWidget* parent)
    : QWidget{ parent }
    , _ui{ new Ui::DialogWidget() }
{
    _ui->setupUi(this);

    _ui->groupBox->setStyleSheet(DialogWidget::deselectedStyle);
}

QDialogWidget::~QDialogWidget()
{
    delete _ui;
}

void QDialogWidget::select()
{
    _selected = true;
    _ui->groupBox->setStyleSheet(DialogWidget::selectedStyle);
}

void QDialogWidget::deselect()
{
    _selected = false;
    _ui->groupBox->setStyleSheet(DialogWidget::deselectedStyle);
}

void QDialogWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);

    if (!_selected)
    {
        _ui->groupBox->setStyleSheet(DialogWidget::hoveredStyle);
    }
}

void QDialogWidget::leaveEvent(QEvent* event)
{
    if (!_selected)
    {
        _ui->groupBox->setStyleSheet(DialogWidget::deselectedStyle);
    }
    
    QWidget::leaveEvent(event);
}
