#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startProcessing();
    void updateProgress(qint64 processed, qint64 total);
    void processingFinished();
    void processingError(const QString &msg);

private:
    Ui::MainWindow *ui;
    bool busy = false;
};

#endif
