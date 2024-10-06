#include "waveform.h"

waveForm::waveForm(QWidget* parent) : QCustomPlot(parent), verticalLine(nullptr)
{

    addGraph();
    setBackground(Qt::transparent);
    xAxis->setTicks(false);
    yAxis->setTicks(false);

    verticalLine = new QCPCurve(this->xAxis, this->yAxis);
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::red);
    verticalLine->setPen(pen);
    connect(this, &QCustomPlot::mousePress, this, &waveForm::slotMousePress);
    connect(this, &QCustomPlot::mouseMove, this, &waveForm::slotMouseMove);
}



bool waveForm::plotWaveForm(fs::path filePath)
{
    std::ifstream plotStream(filePath, std::ios::binary);
    if (!plotStream.is_open())
        return false;

    if (!plotStream.read(reinterpret_cast<char*>(&wavHead), sizeof(wavHeader)))
        return false;

    int numSamples = getSamplesAmount();

    if (numSamples <= 0 || wavHead.bitsPerSample != 16)
        return false;

    samples.resize(numSamples);

    if (!plotStream.read(reinterpret_cast<char*>(samples.data()), wavHead.subchunk2Size))
        return false;

    QVector<double> x(numSamples), y(numSamples);

    for (int i = 0; i < numSamples; i++)
    {
        x[i] = static_cast<double>(i);
        y[i] = static_cast<double>(samples[i]) / (samples[i] < 0 ? 32768.0 : 32767.0);  // Нормализация для диапазона [-32768, 32767]
    }

    // Установка данных графика
    if (graphCount() == 0) {
        addGraph();  // Добавляем график, если он еще не создан
    }

    graph(0)->setData(x, y);
    xAxis->setRange(0, wavHead.subchunk2Size/2);
    yAxis->setRange(-1.05, 1.05);
    replot();

    return true;
}
void waveForm::setVerticalLine(int coordX)
{
    QVector<double> x(2), y(2);
    y[0] = -1.05;
    y[1] = 1.05;

    x[0] = static_cast<double>(coordX);
    x[1] = static_cast<double>(coordX);

    verticalLine->setData(x, y);
    replot();
}

int waveForm::getSamplesAmount()
{
    if(wavHead.bitsPerSample>0)
        return wavHead.subchunk2Size / (wavHead.bitsPerSample / 8);
    else
        return -1;
}

int waveForm::getMsLength()
{
    if(wavHead.sampleRate>0)
        return getSamplesAmount() / wavHead.sampleRate;
    else
        return -1;
}

void waveForm::slotMousePress(QMouseEvent *event)
{
    double coordX = xAxis->pixelToCoord(event->pos().x());

    QVector<double> x(2), y(2);

    int numSamples = getSamplesAmount();

    y[0] = -1.05;
    y[1] = 1.05;

    if(coordX<=0)
        { x[0] = 0; x[1] = 0; }
    else if (coordX>numSamples)
        { x[0] = numSamples - 1; x[1] = numSamples - 1; }
    else
        { x[0] = coordX; x[1] = coordX; }
    verticalLine->setData(x, y);
        replot();
}

void waveForm::slotMouseMove(QMouseEvent *event)
{
    if(QApplication::mouseButtons()) slotMousePress(event);
}

void waveForm::slotSwitchProgressBarRenderLoop(bool isPlaying)
{
    qDebug()<<"#SIGNAL switchProgressBarRenderLoop# ACTIVATED";
}
void waveForm::slotPlotWaveForm(std::shared_ptr<soundFile> newObject)
{
    activeItemFile = nullptr;
    activeItemFile = newObject;
    qDebug()<<"#SIGNAL plotWaveForm# ACTIVATED";
    if(activeItemFile)
    plotWaveForm(activeItemFile->getFilePath());
    else
        qDebug()<<"#SIGNAL PLOTWAVEFORM# itemFile is nullptr";
}
