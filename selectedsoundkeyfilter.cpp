#include "selectedsoundkeyfilter.h"
#include "mainwindow.h"

selectedSoundKeyFilter::selectedSoundKeyFilter(QPushButton* button, MainWindow* mainWnd, QObject* parent)
    : QObject(parent), playSoundButton(button)
    {

    mainWindow = mainWnd;
    qDebug()<<"soundFilter event created";

    }

bool selectedSoundKeyFilter::eventFilter(QObject* obj, QEvent* event)
{
        if (event->type() == QEvent::KeyPress &&
        playSoundButton->isEnabled() &&
        static_cast<QKeyEvent*>(event)->key() == Qt::Key_Space)
        {
                mainWindow->getActiveItemFile()->startPlayingAudioFile();
                return true;
        }
    return false;
}
