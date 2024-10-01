#ifndef SOUNDFILE_H
#define SOUNDFILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <QString>

#include <SFML/Audio.hpp>

#include "soundutils.h"
#include "pathdata.h"
#include "wavHeader.h"

namespace fs = std::filesystem;



class soundFile
{
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

    QString formatIssueTextGenerator();

    static fs::path generateClonedFilePath(fs::path originalFilePath);
    static fs::path generateOriginalFilePath(fs::path clonedFilePath);
    static fs::path generateWavFileClone(fs::path originalFilePath);

    ~soundFile();

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
};

#endif // SOUNDFILE_H

/* .WAV (16-bit)
 * 44100/22050//11025 hz
 * mono */
