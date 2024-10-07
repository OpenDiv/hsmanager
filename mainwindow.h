#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <SFML/Audio.hpp>

//TEMP
#include <chrono>
//

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QThread>

#include <filesystem>
#include <string>
#include <windows.h>
#include <mmsystem.h>
#include <memory.h>
#include <iostream>

#include "selectedsoundkeyfilter.h"
#include "soundfile.h"
#include "qcustomplot.h"
#include "waveform.h"
#include "pathData.h"
#include "controller.h"

 /* .WAV (16-bit)
 * 44100/22050//11025 hz
 * mono */

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
namespace fs = std::filesystem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, pathData *data = nullptr);
    bool generateSoundFolders();

    void tempPlot(fs::path path);

    bool getSoundSelectedState();
    void setSoundSelectedState(bool state);

    void openSelectedItemUI(QTableWidgetItem* item);

    void updateCurrentSoundLabels();
    void updateLabelFromFile(std::string filePath, QString prefix, QLabel* curLabel);

    void setActiveTableItem(QTableWidgetItem* newItem);

    QTableWidgetItem* getActiveTableItem();
    std::shared_ptr<soundFile> getActiveItemFile();

    Controller* ctrl;
    QPushButton* getPlayButton();
    ~MainWindow();

private:
    pathData *data;
    Ui::MainWindow *ui;
    QFileSystemWatcher watcher;
    QTableWidgetItem* activeTableItem = nullptr;
    std::shared_ptr<soundFile> activeItemFile = nullptr;
    selectedSoundKeyFilter *keyFilter;
    fs::path tfPath; // сам путь

    void updateTableItemTitles();
    void updateTableItem();

    void updateItemColor();
    void setItemColor(int row, const QColor &color);

    bool moveSound(QTableWidgetItem* item, bool isHitsound);
    void showSoundWidgets(bool needToShow);
    bool isSoundSelected = false;

    void setupWatcher(MainWindow* MainWindow);
    void setupFilter(MainWindow* MainWindow);
    void setupWrongFormatIcon();



private slots:

    void onDirectoryChanged(const QString &path);

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_pushButton_openFolder_clicked();
    void on_pushButton_addItems_clicked();
    void on_pushButton_setHitsound_clicked();
    void on_pushButton_setKillsound_clicked();
    void on_pushButton_closeFile_clicked();
    void on_pushButton_icon_clicked();
    void on_pushButton_clicked();
};
#endif // MAINWINDOW_H
