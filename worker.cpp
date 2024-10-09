#include "worker.h"
#include <QRandomGenerator>

void Worker::doWork()
{
    if(waveFormWidget->activeItemFile)
    {
        connect(this, &Worker::askForReplot, waveFormWidget, &waveForm::handleOffsetData);
        connect(waveFormWidget, &waveForm::progressBarReplotted, this, &Worker::rebuildConfirm);

        connect(waveFormWidget, &waveForm::soundFilePrepared, this, &Worker::setupSoundFileConnections);
        //connect(waveFormWidget, &waveForm::soundFileDestroyed)
        setupSoundFileConnections();
    }
}

void Worker::setupSoundFileConnections()
{
    connect(this, &Worker::askForOffset, waveFormWidget->activeItemFile.get(), &soundFile::handleOffsetRequest);
    connect(waveFormWidget->activeItemFile.get(), &soundFile::handleOffsetAnwser, this, &Worker::handleOffset);
    emit(askForOffset());
}

void Worker::handleOffset(int64_t currentOffset)
{
    currentOffset = (int)(currentOffset*0.000001 * 44100.0);
    QThread::sleep(0.03);
    QRandomGenerator rnd(1234);
    qDebug()<<"rand "<<rnd.generate();
    emit(askForReplot(currentOffset));
}
void Worker::rebuildConfirm()
{
    if(waveFormWidget->activeItemFile)
        emit(askForOffset());
    else
        emit(resultReady());

}
