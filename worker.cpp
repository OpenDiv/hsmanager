#include "worker.h"

void Worker::doWork()
{
    if(waveFormWidget->activeItemFile)
    {
        connect(this, &Worker::askForOffset, waveFormWidget->activeItemFile.get(), &soundFile::handleOffsetRequest);
        connect(waveFormWidget->activeItemFile.get(), &soundFile::handleOffsetAnwser, this, &Worker::handleOffset);

        connect(this, &Worker::askForReplot, waveFormWidget, &waveForm::handleOffsetData);
        connect(waveFormWidget, &waveForm::progressBarReplotted, this, &Worker::rebuildConfirm);

        emit(askForOffset());
    }
    else
        qDebug()<<"error";

}

void Worker::handleOffset(int64_t currentOffset)
{
    currentOffset = (int)(currentOffset*0.000001 * 44100.0);
    QThread::sleep(0.05);
    emit(askForReplot(currentOffset));
}
void Worker::rebuildConfirm()
{
    emit(askForOffset());
}
