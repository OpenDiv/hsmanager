#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <filesystem>
#include <QThread>

#include "qcustomplot.h"
#include "soundfile.h"

namespace fs = std::filesystem;

class waveForm : public QCustomPlot
{
    Q_OBJECT
public:
    explicit waveForm(QWidget *parent = nullptr);
    bool plotWaveForm(fs::path filePath);
    void setVerticalLine(int coordX);

    int getSamplesAmount();
    int getMsLength();

    std::shared_ptr<soundFile> activeItemFile;
    fs::path getFilePath();
public slots:
    void slotGetSoundFile(std::shared_ptr<soundFile> newSoundFile);
    void slotPlotWaveForm(std::shared_ptr<soundFile> newObject);
    void handleOffsetData(int64_t);
    void slotSoundFileDestroyed();
private slots:
    void slotMousePress(QMouseEvent *event);
    void slotMouseMove(QMouseEvent *event);
signals:
    void progressBarReplotted();
    void soundFilePrepared();
    void soundFileDestroyed();
    void waveFormReady();

private:
    QCPCurve *verticalLine;
    QVector<int16_t> samples;
    wavHeader wavHead;

};

#endif // WAVEFORM_H
