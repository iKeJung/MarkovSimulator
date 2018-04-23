#ifndef MARKOVSIMULATOR_H
#define MARKOVSIMULATOR_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QRandomGenerator>
#include <QThread>
#include <QTextStream>
#include <QThreadPool>

#include "SimulationThread.h"
#include "RunnableSimulation.h"

//#include "WorkThread.h"

class MarkovSimulator : public QObject
{
    Q_OBJECT
public:
    explicit MarkovSimulator(QObject *parent = 0);
    ~MarkovSimulator();

    QVector<QVector<double> > getCtmcMatrix();
    void setCtmcMatrix(const QVector<QVector<double *> > value);
    QVector<QVector<double> > getDtmcMatrix();
    int getSize() const;
    QVector<double> getLastVisits() const;

    QVector<double> simulate(int steps, int startPosition = 0);
    void simulateThreaded(int steps, int startPosition = 0);
    void simulateMultipleThreads(int steps, int startPosition = 0);

    QVector<double> getLastResults() const;

    int getLastNsteps() const;

signals:
    void resultsReady(QVector<double> visits, QVector<double> results);
public slots:
    void getResults(QVector<double> visits, QVector<double> results);
private:    
    QVector<QVector<double*> > ctmcMatrix;
    QVector<QVector<double*> > dtmcMatrix;
    QVector<QVector<double> > partialResults;
    QVector<QVector<double> > partialVisits;
    QVector<double> lastResults;
    QVector<double> lastVisits;

    int size;
    int currentPosition;
    int remainingSteps;
    int NThreads;
    int lastNsteps;

    void deleteMatrix(QVector<QVector<double*> > *matrix);
    QVector<QVector<double> > toStaticMatrix(QVector<QVector<double*> > matrix);
    void makeMainDiagonal(QVector<QVector<double*> > matrix);
    void convertToDTMC();

    int runningThreads;
    //QThread thread;
};



#endif // MARKOVSIMULATOR_H
