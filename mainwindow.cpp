#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fileworker.h"

#include <QMessageBox>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->startButton, &QPushButton::clicked,
            this, &MainWindow::startProcessing);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startProcessing()
{
    if (busy)
        return;

    QString input = ui->inputPathEdit->text();
    QString outputDir = ui->outputDirEdit->text();

    if (input.isEmpty() || outputDir.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пустой путь");
        return;
    }

    QFileInfo info(input);
    QString outPath = outputDir + "/" + info.fileName();

    int counter = 1;
    while (QFile::exists(outPath)) {
        outPath = outputDir + "/" +
                  info.completeBaseName() + "_" +
                  QString::number(counter++) + "." +
                  info.suffix();
    }

    QByteArray key = QByteArray::fromHex(ui->keyEdit->text().toLatin1());
    if (key.size() != 8) {
        QMessageBox::critical(this, "Ошибка", "Ключ должен быть восьмибайтным (16 символов)");
        return;
    }

    auto *worker = new FileWorker;
    auto *thread = new QThread;

    worker->setInputFile(input);
    worker->setOutputFile(outPath);
    worker->setKey(key);
    worker->setDeleteSource(ui->deleteCheckBox->isChecked());
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &FileWorker::process);
    connect(worker, &FileWorker::progress, this, &MainWindow::updateProgress);
    connect(worker, &FileWorker::finished, this, &MainWindow::processingFinished);
    connect(worker, &FileWorker::error, this, &MainWindow::processingError);

    connect(worker, &FileWorker::finished, worker, &QObject::deleteLater);
    connect(worker, &FileWorker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    busy = true;
    thread->start();
}

void MainWindow::updateProgress(qint64 processed, qint64 total)
{
    if (total <= 0) return;
    ui->progressBar->setValue(int(processed * 100 / total));
}

void MainWindow::processingFinished()
{
    busy = false;
    QMessageBox::information(this, "Завершено", "Процесс завершен");
}

void MainWindow::processingError(const QString &msg)
{
    busy = false;
    QMessageBox::critical(this, "Ошибка", msg);
}
