#ifndef DIALOGWIDGET_H
#define DIALOGWIDGET_H

#include <QWidget>

namespace Ui
{
    class DialogWidget;
}

class QDialogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDialogWidget(QWidget* parent = nullptr);
    ~QDialogWidget();

    void select();
    void deselect();

protected:
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

signals:

private:
    bool _selected = false;
    
    Ui::DialogWidget* _ui;
};

#endif // DIALOGWIDGET_H
