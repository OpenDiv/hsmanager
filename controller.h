#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QThread>

#include "worker.h"

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller(waveForm* waveFormObj);
    ~Controller();
public slots:
signals:
    void operate();
};

#endif //
