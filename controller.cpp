#include "controller.h"

#include <QMetaType>

Controller::Controller(waveForm* waveFormObj)
{
    qRegisterMetaType<int64_t>("int64_t");
    Worker *worker = new Worker;
    worker->waveFormWidget = waveFormObj;
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &Controller::operate, worker, &Worker::doWork);
    connect(worker, &Worker::resultReady, this, [=](){qDebug()<<"finished worker thread";});

    workerThread.start();
}

Controller::~Controller()
{
    workerThread.quit();
    workerThread.wait();
}
