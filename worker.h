#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>

#include "waveform.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    waveForm* waveFormWidget;

public slots:
    void setupSoundFileConnections();
    void doWork();
    void handleOffset(int64_t);
    void rebuildConfirm();

signals:
    void askForOffset();
    void askForReplot(int64_t);
    void resultReady();
};



#endif // WORKER_H
