#include "DialogUpdateWorker.h"

#include "Client.h"

#include <QThread>

QDialogUpdateWorker::QDialogUpdateWorker(Client& client, QObject* parent)
    : QObject(parent)
    , _client(client)
{
}

void QDialogUpdateWorker::requestStop()
{
    _stopRequested = 1;
}

void QDialogUpdateWorker::setLocalContactId(int localContactId)
{
    _localContactId = localContactId;
}

void QDialogUpdateWorker::run()
{
    while (_stopRequested == 0)
    {
        QThread::msleep(100);
        
        _client.updateDialogs();
        emit dialogsUpdated();
        
        const int localContactId = _localContactId;
        if (localContactId == -1)
        {
            continue;
        }

        if (_client.getDialogs()[localContactId].messages.size() <= 1)
        {
            _client.cleanCachedMessages(localContactId);
        }
        _client.updateMessages(localContactId);
        emit messagesUpdated();
    }
    emit threadStopped();
}
