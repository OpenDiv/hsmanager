#include "tablewidgetwatcher.h"

#include <QDebug>
#include <QThread>
#include <QFile>

TableWidgetWatcher::TableWidgetWatcher(QObject* parent)
    : QObject(parent), unprocessedFileExists(false) {}

void TableWidgetWatcher::processFile(QStringList& fileList)
{
    for(auto filePath : fileList)
    {
        if(unprocessedFileExists)
            return;
        checkFile(filePath);
    }

    emit finished();
}
