#ifndef SELECTEDSOUNDKEYFILTER_H
#define SELECTEDSOUNDKEYFILTER_H

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

class MainWindow;

class selectedSoundKeyFilter : public QObject
{
    Q_OBJECT
public:
    explicit selectedSoundKeyFilter(QPushButton* button, MainWindow* mainWnd, QObject* parent = nullptr);
private:
    QPushButton* playSoundButton;
    bool eventFilter(QObject* obj, QEvent* event) override;
    MainWindow* mainWindow;
};

#endif // SELECTEDSOUNDKEYFILTER_H
