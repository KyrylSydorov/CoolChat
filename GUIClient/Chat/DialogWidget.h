#ifndef DIALOGWIDGET_H
#define DIALOGWIDGET_H

#include <QWidget>

struct Dialog;

namespace Ui
{
    class DialogWidget;
}

class QDialogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDialogWidget(const Dialog& dialog, const size_t id, QWidget* parent = nullptr);
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

    size_t _id;
};

#endif // DIALOGWIDGET_H
