#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>

namespace Ui
{
    class MessageWidget;
}

class QMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMessageWidget(QWidget* parent = nullptr);
    ~QMessageWidget();

    void stylizeAsIncoming();
    void stylizeAsOutgoing();

private:
    Ui::MessageWidget* _ui;

signals:
};

#endif // MESSAGEWIDGET_H
