#ifndef DIALOGUPDATEWORKER_H
#define DIALOGUPDATEWORKER_H

#include <QObject>
#include <QAtomicInt>

class Client;

class QDialogUpdateWorker : public QObject
{
    Q_OBJECT
public:
    explicit QDialogUpdateWorker(Client& client, QObject* parent = nullptr);

    void requestStop();
    void setLocalContactId(int localContactId);

public slots:
    void run();
    
signals:
    void dialogsUpdated();
    void messagesUpdated();
    void threadStopped();
    
private:
    Client& _client;

    QAtomicInt _stopRequested = 0;
    QAtomicInt _localContactId = -1;
};

#endif // DIALOGUPDATEWORKER_H
