#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QRandomGenerator>

class SimulationThread : public QThread
{
    Q_OBJECT
public:
    explicit SimulationThread(QObject *parent = nullptr);void run() override;

    void setSteps(int value);

    void setStartPosition(int value);

    void setMatrix(const QVector<QVector<double *> > &value);

signals:
    void resultsReady(QVector<double> numberVisits, QVector<double> results);
public slots:

private:
    int steps;
    int startPosition;
    QVector<QVector<double*> > matrix;
    void simulate(int steps, int startPosition = 0);
};

#endif // SIMULATIONTHREAD_H
