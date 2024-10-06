#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <filesystem>
#include <QThread>

#include "qcustomplot.h"
#include "soundutils.h"
#include "wavHeader.h"
#include "soundfile.h"

namespace fs = std::filesystem;

class waveForm : public QCustomPlot
{
    Q_OBJECT
public:
    explicit waveForm(QWidget *parent = nullptr);

    std::shared_ptr<soundFile> activeItemFile;

    void setVerticalLine(int coordX);
    bool plotWaveForm(fs::path filePath);

    int getSamplesAmount();
    int getMsLength();


    fs::path getFilePath();
public slots:
    void slotSwitchProgressBarRenderLoop(bool isPlaying);
    void slotPlotWaveForm(std::shared_ptr<soundFile> newObject);
private slots:
    void slotMousePress(QMouseEvent *event);
    void slotMouseMove(QMouseEvent *event);
signals:
    void waveFormReady();

private:
    QCPCurve *verticalLine;
    QVector<int16_t> samples;
    wavHeader wavHead;

};

#endif // WAVEFORM_H
