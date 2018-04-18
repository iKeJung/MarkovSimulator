#ifndef SIMULATIONTHREAD_H
#define SIMULATIONTHREAD_H

#include <QObject>
#include <QVector>
#include <QRandomGenerator>

class SimulationThread : public QObject
{
    Q_OBJECT
public:
    explicit SimulationThread(QObject *parent = nullptr);

    void setSteps(int value);

    void setStartPosition(int value);

    void setMatrix(const QVector<QVector<double *> > &value);


signals:
    void resultsReady(QVector<double> numberVisits, QVector<double> results);
public slots:
    void simulate(int steps, int startPosition = 0);
private:

    int steps;
    int startPosition;
    QVector<QVector<double*> > matrix;

};

#endif // SIMULATIONTHREAD_H
