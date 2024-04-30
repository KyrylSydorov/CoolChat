#include "DialogWidget.h"

#include "ui_DialogWidget.h"

#include "../Connection/Client.h"

namespace DialogWidget
{
    const char* deselectedStyle = "background : rgb(78,189,217);";
    const char* hoveredStyle = "background : rgb(153,223,231);";
    const char* selectedStyle = "background : rgb(17,127,176);";
    
}

QDialogWidget::QDialogWidget(const Dialog& dialog, const size_t id, QWidget* parent)
    : QWidget{ parent }
    , _ui{ new Ui::DialogWidget() }
    , _id{ id }
{
    _ui->setupUi(this);

    const UserInfo& user = dialog.userInfo;

    _ui->tagLabel->setText(("@" + user.nickname).c_str());
    _ui->messageLabel->setText(dialog.messages.back().message.c_str());

    std::string time;
    time.resize(26);
    ctime_s(time.data(), time.capacity(), &dialog.messages.back().time);
    
    _ui->timeLabel->setText(time.c_str());

    _ui->unreadLabel->setText("");

    _ui->nameLabel->setText((user.firstName + " " + user.lastName).c_str());

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
