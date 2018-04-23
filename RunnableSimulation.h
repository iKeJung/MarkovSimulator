#ifndef RUNNABLESIMULATION_H
#define RUNNABLESIMULATION_H

#include <QRunnable>
#include <QVector>
#include <QRandomGenerator>
#include <QObject>

class RunnableSimulation : public QObject, public QRunnable
{
    Q_OBJECT
public:
    RunnableSimulation(QVector<QVector<double*> > *matrix, int steps, int startPosition);
    void run();
private:
    QVector<QVector<double*> > *matrix;
    int steps;
    int startPosition;
signals:
    void resultsReady(QVector<double> numberVisits, QVector<double> results);
};

#endif // RUNNABLESIMULATION_H
