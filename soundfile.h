#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include <string>
#include <filesystem>

#include <QObject>
#include <cstring>
#include <cstdlib>
#include <QString>

#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "wavHeader.h"

namespace fs = std::filesystem;

class soundFile : public QObject, public std::enable_shared_from_this<soundFile>
{
    Q_OBJECT
public:
    soundFile(std::string path);
    wavHeader wavHeader;

    bool getIsWavState();
    bool getIfExistsState();

    bool readFile(); // reading into wavHeader structure
    bool getAudioData(); // reading after 44 byte В ПРОЦЕССЕ Я НЕ ЕБУ ПОКА КАК НА СЕМПЛЫ ДЕЛИТЬ ЭТО ВЕКТОР ИЛИ ЧЁ

    long getSizeBytes();
    void updateSizeBytes();

    long getLengthMs();
    void updateLengthMs();

    fs::path getFilePath();
    std::string getFileName();

    void setTfFormat();
    bool getIsTfFormat();

    void convertIntoWav();
    bool updateFile();

    bool startPlayingAudioFile();

    void fileUpdated();

    QString formatIssueTextGenerator();

    static fs::path generateClonedFilePath(fs::path originalFilePath);
    static fs::path generateOriginalFilePath(fs::path clonedFilePath);
    static fs::path generateWavFileClone(fs::path originalFilePath);

    ~soundFile();

    //TEMP
    int64_t getPlayingOffsetMethod();
    //TEMP
private:
    bool checkIfFileExists();
    bool checkIsWavState();

    bool isWav = false;
    bool isFileExists = false;
    unsigned long sizeBytes = 0;
    unsigned long lengthMs = 0;

    void mp3IntoWav();

    sf::SoundBuffer soundBuffer;
    sf::Sound sound;

    fs::path filePath;
    std::string fileExtension;

signals:
    void audioStateChanged(bool isPlaying);
    void audioUpdated(std::shared_ptr<soundFile> soundFilePtr);

    void handleOffsetAnwser(int64_t);
public slots:
    void handleOffsetRequest();
};

#endif // SOUNDFILE_H

/* .WAV (16-bit)
 * 44100/22050//11025 hz
 * mono */
