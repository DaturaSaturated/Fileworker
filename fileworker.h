#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <QObject>
#include <QString>
#include <QByteArray>

class FileWorker : public QObject
{
    Q_OBJECT
public:
    explicit FileWorker(QObject *parent = nullptr);

    void setInputFile(const QString &path);
    void setOutputFile(const QString &path);
    void setKey(const QByteArray &key);
    void setDeleteSource(bool value);

signals:
    void progress(qint64 processed, qint64 total);
    void finished();
    void error(const QString &msg);

public slots:
    void process();

private:
    QString inputFile;
    QString outputFile;
    QByteArray xorKey;
    bool deleteSource = false;
};

#endif
