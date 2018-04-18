#include "RunnableSimulation.h"

RunnableSimulation::RunnableSimulation(QVector<QVector<double*> > matrix, int steps, int startPosition)
{
    this->matrix = matrix;
    this->steps = steps;
    this->startPosition = startPosition;
    this->setAutoDelete(true);
}

void RunnableSimulation::run()
{
    int size = matrix.size();
    QVector<double> results;
    //1
    if(size == 0){
        emit resultsReady(results,results);
    }

    //2
    for (int x = 0; x < size; ++x) {
        results << 0;
    }
    int currentPosition = startPosition;
    for (int remainingSteps = steps; remainingSteps > 0; --remainingSteps) {
        //3
        double random = QRandomGenerator::global()->generateDouble();
        //4
        QVector<double*> possibilities = matrix[currentPosition];
        //5
        QVector<double> cumulative;
        QVector<int> positions;
        double sum = 0;
        for (int x = 0; x < size && sum < 1; ++x) {
            //6
            if(*possibilities[x]==0){
                continue;
            }
            sum+=*possibilities[x];
            cumulative << sum;
            positions << x;

        }
        for (int x = 0; x < positions.size(); ++x) {
            //5
            if(random<cumulative[x]){
                results[positions[x]] ++;
                currentPosition = positions[x];
                break;
            }
        }

    }
    QVector<double> numberVisits = results;


    for (int x = 0; x < size; ++x) {
        results[x] =results[x]/steps;
    }

    emit resultsReady(numberVisits,results);
}
