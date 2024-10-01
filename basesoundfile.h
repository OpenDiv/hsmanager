#ifndef BASESOUNDFILE_H
#define BASESOUNDFILE_H

class baseSoundFile
{
public:
    baseSoundFile();
    long getSizeBytes();
    bool getIsTfFormat();
    bool checkIfFileExists();
    bool checkIsWavState();
    //std::string fileExtension;
};

#endif // BASESOUNDFILE_H
