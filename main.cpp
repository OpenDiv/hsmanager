#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    pathData data;

    MainWindow w(nullptr, &data);
    w.show();

    selectedSoundKeyFilter* keyFilter = new selectedSoundKeyFilter(w.getPlayButton(), &w);
    a.installEventFilter(keyFilter);

    w.generateSoundFolders();

    return a.exec();

}

bool MainWindow::generateSoundFolders()
{
    fs::path tfPath = data->getTfPath();
    tfPath += "\\tf\\custom\\hitsoundmanager\\sound\\ui";
    fs::create_directories(data->getTfPath());

    fs::create_directories("hs_manager\\sound");
    fs::create_directories("hs_manager\\temp");
    return true;
}


