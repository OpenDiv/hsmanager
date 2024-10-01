#ifndef PATHDATA_H
#define PATHDATA_H

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QStringList>

#include <fstream>
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

class pathData
{
public:
    pathData();
    fs::path getTfPath() const; // получение из переменной
    QString qGetTfPath() const;

    void openSoundFolder() const;
    bool addNewFilesIntoSoundFolder(QStringList newFileList) const;

    bool setTfPath(const fs::path& newPath); // устанавливает переменную
    bool locateTfPath(); // находит путь

    static fs::path getAudioPath();
    static QString qGetAudioPath();
    bool setAudioPath(const fs::path& newPath);

    static QString pathToQString(const fs::path& path);
    static fs::path qStringToPath(const QString& qPath);

    ~pathData();

private:
    static fs::path audioPath;
    fs::path tfPath;
};



#endif // PATHDATA_H
