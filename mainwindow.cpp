#include "mainwindow.h"
#include "ui_mainwindow.h"

QColor colorNotWavFormat("#f0b2b2");
QColor colorNotTfFormat("#fdffa1");

MainWindow::MainWindow(QWidget *parent, pathData *data)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , data(data)
{
    ui->setupUi(this);

    QWidget::setWindowTitle("TfSound Manager");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    showSoundWidgets(false);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->label_audioPath->setText(data->qGetAudioPath());

    setupWatcher(this);
    setupWrongFormatIcon();
}

void MainWindow::setupWatcher(MainWindow* MainWindow)
{
    watcher.addPath(data->qGetAudioPath());
    connect(&watcher, &QFileSystemWatcher::directoryChanged, MainWindow, &MainWindow::onDirectoryChanged);
    updateTableItemTitles();
    updateCurrentSoundLabels();
}

void MainWindow::setupFilter(MainWindow* MainWindow)
{
    keyFilter = new selectedSoundKeyFilter(ui->pushButton_closeFile, MainWindow);
    this->installEventFilter(keyFilter);
}

void MainWindow::setupWrongFormatIcon()
{   
    QPixmap pixmap("C:\\Users\\Sema\\Downloads\\New Microsoft Icon Set for Windows 10 (10.0.10125)\\imageres_84.ico");

    if (pixmap.isNull())
        qDebug() << "failed to load icon";
    else
    {
        QIcon buttonIcon(pixmap);
        ui->pushButton_icon->setIcon(buttonIcon);
        ui->pushButton_icon->setIconSize(QSize(16, 16));
        ui->pushButton_icon->setFixedSize(48, 48);
        ui->pushButton_icon->setText("");
        ui->pushButton_icon->setStyleSheet("background-color: transparent;");
    }
}

void MainWindow::updateCurrentSoundLabels()
{
    std::string soundPath = data->getTfPath().string()+"\\tf\\custom\\hitsoundmanager\\sound\\ui\\";
    updateLabelFromFile(soundPath + "killsound.txt", "Current killsound: ", ui->label_curKillSound);
    updateLabelFromFile(soundPath + "hitsound.txt", "Current hitsound: ", ui->label_curHitSound);
}

void MainWindow::updateLabelFromFile(std::string filePath, QString prefix, QLabel* curLabel)
{
    std::ifstream curFile(filePath);
    if(curFile.is_open())
    {
        std::string fileName;
        getline(curFile, fileName);
        curLabel->setText(prefix+QString::fromStdString(fileName));
    }
    else
        curLabel->setText(prefix+"none");
}

void MainWindow::updateTableItemTitles()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    QDir audioFolder(data->qGetAudioPath());
    QStringList fileList = audioFolder.entryList(QDir::Files);

    for(auto& fileName : fileList)
    {
        fs::path filePath = (data->getAudioPath().string()+"\\"+fileName.toStdString());
        std::string fileNameStr = fileName.toStdString();
        if (filePath.extension().string() == ".wav" || filePath.extension().string() ==".mp3")
        {
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            QTableWidgetItem *item = new QTableWidgetItem(fileName);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget->setItem(row, 0, item);

            if (!soundUtils::getIsTfFormat(filePath))
                item->setBackground(colorNotTfFormat);
            if (!soundUtils::getIsWav(filePath))
                item->setBackground(colorNotWavFormat);
        }
    }
}

void MainWindow::onDirectoryChanged(const QString &path)
{
    updateTableItemTitles();
}

void MainWindow::on_pushButton_openFolder_clicked()
{
    data->openSoundFolder();
}

void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    openSelectedItemUI(item);
}

void MainWindow::openSelectedItemUI(QTableWidgetItem *item)
{
    std::string itemFilePathStr = (data->qGetAudioPath()+"\\"+item->text()).toStdString();
    fs::path itemFileClonePath = soundFile::generateClonedFilePath(itemFilePathStr);
    soundFile::generateWavFileClone(itemFilePathStr);
    if(!itemFileClonePath.empty())
    {
        std::ifstream itemFileStream(itemFileClonePath);
        if(getActiveTableItem() == item && itemFileStream.is_open())
            { return; }
        setActiveTableItem(item);
        activeItemFile = std::make_unique<soundFile>(itemFileClonePath.string());
        if(activeItemFile->getIsWavState())
        {
            setSoundSelectedState(true);
            showSoundWidgets(getSoundSelectedState());

            ui->label_openName->setText(item->text());
            ui->label_openSize->setText(QString::number(activeItemFile->getSizeBytes())+" bytes");
            ui->label_openLength->setText(QString::number(activeItemFile->getLengthMs())+" ms");
        }
    }
}


void MainWindow::showSoundWidgets(bool needToShow)
{
    if(needToShow)
    {
        ui->label_openName->show();
        ui->label_openLength->show();
        ui->label_openSize->show();

        if(activeItemFile!=nullptr)
        {
            QString tooltipText = activeItemFile->formatIssueTextGenerator();
            if(!tooltipText.isEmpty())
            {
                ui->pushButton_icon->setToolTip(tooltipText);
                ui->pushButton_icon->show();
            }
            else
                ui->pushButton_icon->hide();
        }
    }
    else
    {
        ui->pushButton_icon->hide();
        ui->label_openName->hide();
        ui->label_openLength->hide();
        ui->label_openSize->hide();
    }

    ui->pushButton_setHitsound->setEnabled(needToShow);
    ui->pushButton_setKillsound->setEnabled(needToShow);
    ui->pushButton_closeFile->setEnabled(needToShow);

}

bool MainWindow::moveSound(QTableWidgetItem *item, bool isHitsound)
{
    fs::path fromSoundPath = (data->qGetAudioPath()+"\\"+item->text()).toStdString();
    fs::path toTfPath;
    if(isHitsound)
        toTfPath = (data->qGetTfPath()+"\\tf\\custom\\hitsoundmanager\\sound\\ui\\hitsound.wav").toStdString();
    else
        toTfPath = (data->qGetTfPath()+"\\tf\\custom\\hitsoundmanager\\sound\\ui\\killsound.wav").toStdString();

    if(fs::exists(toTfPath))
        fs::remove(toTfPath);
    fs::copy(fromSoundPath, toTfPath, fs::copy_options::overwrite_existing);

    std::string logTextPath = toTfPath.string();
    logTextPath.replace(logTextPath.end()-4, logTextPath.end(), ".txt");
    std::ofstream logTextFile(logTextPath);
    if(logTextFile)
    {
        logTextFile<<item->text().toStdString();
        logTextFile.close();
    }
    return true;
}

void MainWindow::on_pushButton_addItems_clicked()
{
    QStringList newSoundFiles = QFileDialog::getOpenFileNames(nullptr, tr("Add files"), data->qGetAudioPath(), tr("Audio files (*.wav *.mp3)"));
    data->addNewFilesIntoSoundFolder(newSoundFiles);
}

bool MainWindow::getSoundSelectedState()
{
    return MainWindow::isSoundSelected;
}

void MainWindow::setSoundSelectedState(bool state)
{
    MainWindow::isSoundSelected = state;
}

void MainWindow::on_pushButton_setHitsound_clicked()
{
    moveSound(getActiveTableItem(), true);
    updateCurrentSoundLabels();
}

void MainWindow::on_pushButton_setKillsound_clicked()
{
    moveSound(getActiveTableItem(), false);
    updateCurrentSoundLabels();
}

void MainWindow::on_pushButton_closeFile_clicked()
{
    setActiveTableItem(nullptr);
    setSoundSelectedState(false);
    showSoundWidgets(getSoundSelectedState());
    activeItemFile = nullptr;
    ui->tableWidget->setFocus(); // костыль inactive selection fix l8r
}

QTableWidgetItem* MainWindow::getActiveTableItem()
{
    return activeTableItem;
}

void MainWindow::setActiveTableItem(QTableWidgetItem* newItem)
{
    activeTableItem = newItem;
}

void MainWindow::on_pushButton_icon_clicked()
{
    if(activeItemFile->getIfExistsState() && activeItemFile->getIsWavState())
    {
        activeItemFile->setTfFormat();
    }
}

QPushButton* MainWindow::getPlayButton()
{
    return ui->pushButton_closeFile;
}

std::shared_ptr<soundFile> MainWindow::getActiveItemFile()
{
    return std::shared_ptr<soundFile>(activeItemFile);
}

void MainWindow::on_pushButton_clicked()
{
    soundUtils::setVolume(fs::path("C:\\Users\\Sema\\Desktop\\viper_orig.wav"), 4);
}

MainWindow::~MainWindow()
{
    delete ui;
}



