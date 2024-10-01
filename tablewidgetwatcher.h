#ifndef TABLEWIDGETWATCHER_H
#define TABLEWIDGETWATCHER_H

#include <QObject>
#include <QStringList>

class TableWidgetWatcher : public QObject {
    Q_OBJECT
public:
    explicit TableWidgetWatcher(QObject* parent = nullptr);
public slots:
    void processFile(QStringList& fileList);
    void stop();
signals:
    void finished();

private:
    bool unprocessedFileExists;
    void checkFile(const QString& filePath);
};

#endif // TABLEWIDGETWATCHER_H
