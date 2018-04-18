#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTextStream>
#include <QInputDialog>

#include "MarkovSimulator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionRun_triggered();

    void on_actionSteps_triggered();

    void on_actionAbout_triggered();

    void on_actionRun_with_Thread_triggered();

    void printResults(QVector<double> visits, QVector<double> results);

    void on_actionRun_Several_Threads_triggered();

    void on_actionClose_triggered();

    void on_actionSave_triggered();

private:
    Ui::MainWindow *ui;
    MarkovSimulator *simulator;

    QVector<QVector<double*> > openFile(QString fileName);
    void saveFile(QString fileName);
    void fileNotSupported(QString info = QString(""));
    void fillCTMCTable();
    void fillDTMCTable();

    int steps;
};

#endif // MAINWINDOW_H
