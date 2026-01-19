#include "fileworker.h"
#include <QFile>

static const qint64 BUFFER_SIZE = 64 * 1024;

FileWorker::FileWorker(QObject *parent) : QObject(parent) {}

void FileWorker::setInputFile(const QString &path) { inputFile = path; }
void FileWorker::setOutputFile(const QString &path) { outputFile = path; }
void FileWorker::setKey(const QByteArray &key) { xorKey = key; }
void FileWorker::setDeleteSource(bool value) { deleteSource = value; }

void FileWorker::process()
{
    if (xorKey.size() != 8) {
        emit error("Ключ должен иметь размер 8 байт (16 символов)");
        emit finished();
        return;
    }

    QFile in(inputFile);
    QFile out(outputFile);

    if (!in.open(QIODevice::ReadOnly)) {
        emit error("Не получилось открыть входной файл");
        emit finished();
        return;
    }

    if (!out.open(QIODevice::WriteOnly)) {
        emit error("Не получилось открыть выходной файл");
        emit finished();
        return;
    }

    qint64 total = in.size();
    qint64 processed = 0;
    qint64 keyIndex = 0;

    QByteArray buffer(BUFFER_SIZE, 0);

    while (!in.atEnd()) {
        qint64 read = in.read(buffer.data(), BUFFER_SIZE);
        if (read <= 0) break;

        for (qint64 i = 0; i < read; ++i) {
            buffer[i] ^= xorKey[keyIndex % 8];
            ++keyIndex;
        }

        out.write(buffer.constData(), read);
        processed += read;
        emit progress(processed, total);
    }

    in.close();
    out.close();

    if (deleteSource)
        QFile::remove(inputFile);

    emit finished();
}
