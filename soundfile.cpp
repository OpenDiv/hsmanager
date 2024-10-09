#include "soundfile.h"

#include <iostream>
#include <fstream>
#include <cstdint>

#include <QDebug>
#include <QMetaType>

soundFile::soundFile(std::string path) : filePath(path)
{
    qDebug()<<"#SOUNDFILE# [+] Constructor worked: "<<QString::fromStdString(fs::path(path).filename().string());
    updateFile();

    fileExtension = filePath.extension().string();
}

void soundFile::fileUpdated()
{
    emit objectCreated(shared_from_this());
}

bool soundFile::updateFile() // fix l8r with extension issues
{
    if(checkIfFileExists() && checkIsWavState())
    {
        readFile();
        updateSizeBytes();
        updateLengthMs();

        soundBuffer.loadFromFile(filePath.string());
        sound.setBuffer(soundBuffer);
        return true;
    }

    return false;
}

bool soundFile::getIsWavState()
{
    return isWav;
}

bool soundFile::getIfExistsState()
{
     return isFileExists;
}

bool soundFile::checkIsWavState()
{
    std::ifstream thisFile(filePath, std::ios::binary | std::ios::ate);
    if(thisFile.is_open() && thisFile.tellg()>44)
    {
        thisFile.seekg(0, std::ios::beg);
        char header[12];
        thisFile.read(header, sizeof(header));

        if(std::memcmp(header, "RIFF", 4) == 0 && std::memcmp(header+8, "WAVE", 4) == 0)
            isWav = true;
        return isWav;
    }

    return false;
}

bool soundFile::checkIfFileExists()
{
    std::ifstream thisFile(filePath);
    isFileExists = thisFile.is_open();
    return isFileExists;
}

bool soundFile::readFile()
{
    std::ifstream thisFile(filePath, std::ios::binary);
    if(!thisFile.is_open())
        return false;
    thisFile.seekg(0);
    thisFile.read(reinterpret_cast<char*>(&wavHeader), sizeof(wavHeader));
    return true;
}

void soundFile::updateSizeBytes()
{
    std::ifstream thisFile(filePath, std::ios::binary | std::ios::ate);
    if(thisFile.is_open())
        sizeBytes = thisFile.tellg();
}

long soundFile::getSizeBytes()
{
    return sizeBytes;
}

void soundFile::updateLengthMs()
{
    if(wavHeader.byteRate!=0)
    {
        if(wavHeader.subchunk2Size < std::numeric_limits<unsigned long>::max()/1000)
        {
            lengthMs = (wavHeader.subchunk2Size*1000)/wavHeader.byteRate;
        }
    }
    else
    {
        lengthMs = ((wavHeader.subchunk2Size)/wavHeader.byteRate)*1000;
    }
}

long soundFile::getLengthMs()
{
    return lengthMs;
}

fs::path soundFile::getFilePath()
{
    return filePath;
}

std::string soundFile::getFileName()
{
    return filePath.filename().string();
}

void soundFile::setTfFormat()
{
    uint16_t requiedBitsPerSample = 16;
    uint16_t requiedNumChannels = 1;

    if(wavHeader.bitsPerSample != requiedBitsPerSample)
    {
        soundUtils::setBitsPerSample(this->getFilePath(), 16);
        qDebug()<<"fixed bits per sample";
    }

    if(wavHeader.numChannels != requiedNumChannels)
    {
        soundUtils::setNumChannelsToMono(this->getFilePath());
        qDebug()<<"fixed amount of channels";
    }

    if(wavHeader.sampleRate != 44100 && wavHeader.sampleRate != 22050 && wavHeader.sampleRate != 11025)
    {
        soundUtils::setSampleRate(this->getFilePath(), 44100);
        qDebug()<<"fixed samplerate";
    }

    updateFile();

}
bool soundFile::getIsTfFormat()
{
    if (wavHeader.audioFormat == 1 &&
        wavHeader.bitsPerSample == 16 &&
        wavHeader.numChannels == 1 &&
        (wavHeader.sampleRate == 44100 || wavHeader.sampleRate == 22050 || wavHeader.sampleRate == 11025)
        )
        return true;
    else
        return false;
}

void soundFile::convertIntoWav()
{ // into switch l8r
    if(getIfExistsState())
    {
        if(fileExtension==".mp3")
            mp3IntoWav();
    }
}

void soundFile::mp3IntoWav()
{

    /*
    fileExtension = ".wav";
    filePath.replace_extension(".wav");

    readFile();
    isWav = true;
    updateSizeBytes();
    updateLengthMs();
*/

}

bool soundFile::startPlayingAudioFile()
{
    //qDebug()<<"playing "<<QString::fromStdString(this->getFileName());
    sf::Sound currentSound;

    if(sound.getBuffer() == nullptr)
        return false;

    sf::Sound::Status status = sound.getStatus();

    emit audioStateChanged(status != sf::Sound::Status::Playing);

    if(status == sf::Sound::Status::Stopped)
        sound.play();
    else if(status == sf::Sound::Status::Paused)
        sound.play();
    else if(status == sf::Sound::Status::Playing)
        sound.pause();

    sf::Time t1;
    t1 = sound.getPlayingOffset();
    qDebug()<<QString::number(t1.asSeconds());

    return true;
}

QString soundFile::formatIssueTextGenerator()
{
    QString tooltipText;

    if(!isFileExists)
    {
        tooltipText = "Unknown issue";
        return tooltipText;
    }
    else
    {
        if(!isWav)
        {
            tooltipText = "Not a wav format";
            return tooltipText;
        }
        else
            if(!getIsTfFormat())
        {
            tooltipText = "Not a TF2 hitsound format";
            if(wavHeader.audioFormat != 1)
                { tooltipText+="\n* Requied AudioFormat : PCM"; }
            if(wavHeader.bitsPerSample != 16)
                { tooltipText+="\n* Requied BitsPerSample : 16 ("+QString::number(wavHeader.bitsPerSample)+")"; }
            if(wavHeader.numChannels != 1)
                { tooltipText+="\n* Requied AmountOfChannels : 1 ("+QString::number(wavHeader.numChannels)+")"; }
            if(!(wavHeader.sampleRate == 44100 || wavHeader.sampleRate == 22050 || wavHeader.sampleRate == 11025))
                { tooltipText+="\n* Requied SampleRate : 44100/22050/11025 ("+QString::number(wavHeader.sampleRate)+")"; }
            return tooltipText;
        }
        else
            return "";
    }
}

fs::path soundFile::generateClonedFilePath(fs::path originalFilePath)
{
    std::string tempFolderPath = pathData::getAudioPath().parent_path().string()+"\\temp\\";
    if(originalFilePath.string().find(tempFolderPath) == std::string::npos)
    {
        return fs::path(originalFilePath.string().replace(
            originalFilePath.string().find("\\sound\\"), 7, "\\temp\\").insert(
            originalFilePath.string().find_last_of(".")-1, "_clone"));
    }
    else
    { return originalFilePath; }
}

fs::path soundFile::generateOriginalFilePath(fs::path clonedFilePath)
{
    std::string soundFolderPath = pathData::getAudioPath().string();
    if(clonedFilePath.string().find(soundFolderPath) == std::string::npos && clonedFilePath.string().find("_clone") != std::string::npos)
    {
        return fs::path(clonedFilePath.string().replace(
            clonedFilePath.string().find("\\temp\\"), 6, "\\sound\\").replace(
            clonedFilePath.string().find_last_of("_clone")-4, 6, ""));
    }
    else
    { return clonedFilePath; }
}

fs::path soundFile::generateWavFileClone(fs::path originalFilePath)
{
    fs::path clonedFilePath = generateClonedFilePath(originalFilePath);
    try {
        fs::copy(originalFilePath, clonedFilePath, fs::copy_options::overwrite_existing);
        return clonedFilePath;
    } catch (fs::filesystem_error& e)
    {
        return fs::path();
    }
}

//TEMP
int64_t soundFile::getPlayingOffsetMethod()
{
    return sound.getPlayingOffset().asMicroseconds();
}
//TEMP

void soundFile::handleOffsetRequest()
{
    qRegisterMetaType<int64_t>("int64_t");
    int64_t offset = getPlayingOffsetMethod();
    emit(handleOffsetAnwser(offset));
}


soundFile::~soundFile()
{
    qDebug()<<"#SOUNDFILE# [-] destructor worked: "<<QString::fromStdString(getFileName());
    if(filePath.string().find("_clone") != std::string::npos ) // if soundFile = clone
    {
        fs::path originalFilePath = generateOriginalFilePath(filePath);
        if(fs::exists(originalFilePath))
        {
            try{
                if(fs::exists(filePath))
                {
                    fs::remove(originalFilePath);
                    fs::copy(filePath, originalFilePath, fs::copy_options::overwrite_existing);
                    fs::remove(filePath);
                }
            } catch (const fs::filesystem_error& e)
            {
                qDebug()<<"cant remove original soundFile: "<<e.what();
            }
        }
    }
    else // if soundFile == original
    {
        qDebug()<<"orig begin";
        fs::path clonedFilePath = generateClonedFilePath(filePath);
        if (fs::exists(clonedFilePath))
        {
            try{
                if(fs::exists(filePath))
                    fs::remove(filePath);
                fs::copy(clonedFilePath, filePath, fs::copy_options::overwrite_existing);
                fs::remove(clonedFilePath);
            } catch (const fs::filesystem_error& e)
            {
                qDebug()<<"cant remove cloned soundFile: "<<e.what();
            }
        }
    }
    emit objectDestroyed();
}


