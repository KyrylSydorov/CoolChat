#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>

namespace Ui
{
    class MessageWidget;
}

struct Message;

class QMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMessageWidget(const Message& message, QWidget* parent = nullptr);
    ~QMessageWidget();

    void stylizeAsIncoming();
    void stylizeAsOutgoing();

private:
    Ui::MessageWidget* _ui;

signals:
};

#endif // MESSAGEWIDGET_H
