#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QWidget>

namespace Ui
{
    class InputWidget;
}

class QInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QInputWidget(QWidget* parent = nullptr);
    ~QInputWidget();

private:
    Ui::InputWidget* _ui;

signals:
};

#endif // INPUTWIDGET_H
