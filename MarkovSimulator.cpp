#include "MarkovSimulator.h"

MarkovSimulator::MarkovSimulator(QObject *parent) : QObject(parent)
{
    size = 0;    
    runningThreads = 0;
    lastNsteps = 0;
    NThreads = 1;
    qRegisterMetaType<QVector<double> >("QVector<double>");
}


MarkovSimulator::~MarkovSimulator()
{
    if(size!=0){
        deleteMatrix(&ctmcMatrix);
        deleteMatrix(&dtmcMatrix);
    }
    //thread.quit();
    //thread.wait();
}

QVector<QVector<double> > MarkovSimulator::getCtmcMatrix()
{
    return toStaticMatrix(ctmcMatrix);
}

//Método para entrada da matriz no simulador
//Não é um método de set simples, pois há a necessidade de se deletar a matriz anterior(caso exista)
//Também já define o tamanho da matriz em um atributo da classe e converte a CTMC para DTMC
void MarkovSimulator::setCtmcMatrix(const QVector<QVector<double *> > value)
{

    if(size!=0){
        deleteMatrix(&ctmcMatrix);
        deleteMatrix(&dtmcMatrix);
    }

    ctmcMatrix = value;
    size = ctmcMatrix.size();
    currentPosition = -1;
    makeMainDiagonal(ctmcMatrix);
    convertToDTMC();
}

QVector<QVector<double> > MarkovSimulator::getDtmcMatrix()
{
    return toStaticMatrix(dtmcMatrix);
}


int MarkovSimulator::getSize() const
{
    return size;
}

//Método que faz a simulação
//Retorna o vetor de resultados(porcentagem de visitas em cada estado)
//Recebe o número de passos/iterações e a posição inicial
//1 - Verifica se existe matriz guardada
//2 - Zera o vetor de resultados
//3 - gera o número aleatório entre [0;1)
//4 - pega a linha da matriz(são os pesos das conexões para cada estado)
//5 - vetor acumulativo que faz a divisão de possibilidades para cada estado
//		exemplo: (0;0,25;0,75) - as tres conexões são de 25% 50% e 25%
//					se o número ficar entre [0;0,25) ele vai pelo primeiro
//					se for [0,25;0,75) vai pelo segundo
//					e se for [0,75;1) vai pelo terceiro caminho		
//6 - se o valor é zero quer dizer que não há conexão então esse estado não é uma possibilidade
//7 - soma uma visita ao estado que vai ser visitado
QVector<double> MarkovSimulator::simulate(int steps, int startPosition)
{		
    QVector<double> results;
	//1
    if(size == 0){
        return results;
    }
	
	//2
    for (int x = 0; x < size; ++x) {
        results << 0;
    }
    currentPosition = startPosition;
    for (remainingSteps = steps; remainingSteps > 0; --remainingSteps) {
		//3
        double random = QRandomGenerator::global()->generateDouble();
		//4
        QVector<double*> possibilities = dtmcMatrix[currentPosition];
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
    lastVisits = results;


    for (int x = 0; x < size; ++x) {
        results[x] =results[x]/steps;
    }
    lastResults = results;
    lastNsteps = steps;
    currentPosition = -1;
    return results;

}

//Método que chama a simulação em uma thread
void MarkovSimulator::simulateThreaded(int steps, int startPosition)
{
    if(size == 0){
        return;
    }
    NThreads =1;
    RunnableSimulation *sim = new RunnableSimulation(&dtmcMatrix,steps,startPosition);
    connect(sim, SIGNAL(resultsReady(QVector<double>,QVector<double>)),this,SLOT(getResults(QVector<double>,QVector<double>)));
    QThreadPool::globalInstance()->start(sim);
    runningThreads++;
    lastNsteps = steps;
}

void MarkovSimulator::simulateMultipleThreads(int steps, int startPosition)
{
    if(size == 0){
        return;
    }
    lastNsteps = steps;
    NThreads = 8;
    int stepsParcial = steps/NThreads;
    for (int x = 0; x < NThreads; ++x) {
        RunnableSimulation *sim = new RunnableSimulation(&dtmcMatrix,stepsParcial,startPosition);
        connect(sim, SIGNAL(resultsReady(QVector<double>,QVector<double>)),this,SLOT(getResults(QVector<double>,QVector<double>)));
        QThreadPool::globalInstance()->start(sim);
        runningThreads++;
    }

}

QVector<double> MarkovSimulator::getLastVisits() const
{
    return lastVisits;
}

//faz parte da simulação com thread
void MarkovSimulator::getResults(QVector<double> visits, QVector<double> results)
{
    //qDebug() << "Thread Finished!";
    runningThreads--;
    //qDebug() << "Running threads: " << runningThreads;
    //qDebug() << QThreadPool::globalInstance()->activeThreadCount();

    partialResults.push_back(results);
    partialVisits.push_back(visits);

    if (runningThreads == 0) {
        lastResults = partialResults[0];
        lastVisits = partialVisits[0];
        for (int x = 1; x < partialResults.size(); ++x) {
            for (int y = 0; y < partialResults[x].size(); ++y) {
                lastResults[y]+=partialResults[x][y];
                lastVisits[y]+=partialVisits[x][y];
            }
        }
        if(NThreads>1){
            for (int x = 0; x < lastResults.size(); ++x) {
                lastResults[x]/=NThreads;
            }
        }
        emit resultsReady(lastVisits, lastResults);
        qDebug() << "Finished";
        partialResults.clear();
        partialVisits.clear();
    }

}

int MarkovSimulator::getLastNsteps() const
{
    return lastNsteps;
}

QVector<double> MarkovSimulator::getLastResults() const
{
    return lastResults;
}

//Método para deleção das matrizes
void MarkovSimulator::deleteMatrix(QVector<QVector<double *> >* matrix)
{
    for (int x = 0; x < matrix->size(); ++x) {
        for (int y = 0; y < (*matrix)[x].size(); ++y) {
            double* value = (*matrix)[x][y];
            delete value;
        }
    }    
    matrix->clear();
}

//Método que retorna uma cópia da matriz entrada como parâmetro
//Importante: a entrada é uma matriz de ponteiros para double,
//				MAS ela retorna uma matriz de double normal, assim não é necessário deletar
//					as matrizes criadas por esse método, entretando consome memória do programa 	
QVector<QVector<double> > MarkovSimulator::toStaticMatrix(QVector<QVector<double *> > matrix)
{
    QVector<QVector<double> > newMatrix;
    int mSize = matrix.size();
    for (int x = 0; x < mSize; ++x) {
        QVector<double> vector;
        for (int y = 0; y < mSize; ++y) {
            vector << *matrix[x][y];
        }
        newMatrix << vector;
    }
    return newMatrix;
}

//Faz a diagonal principal da matriz CTMC
void MarkovSimulator::makeMainDiagonal(QVector<QVector<double *> > matrix)
{
    int mSize = matrix.size();
    for (int x = 0; x < mSize; ++x) {
        double sum = 0;
        for (int y = 0; y < mSize; ++y) {
            if(y != x){
                sum += *matrix[x][y];
            }
        }
        *matrix[x][x] = -sum;
    }
}

//Converte a matriz CTMC para  DTMC
void MarkovSimulator::convertToDTMC()
{
    double max = 0;
    for (int x = 0; x < size; ++x) {
        if(max > *ctmcMatrix[x][x]){ // procura maior valor
            max = *ctmcMatrix[x][x];
        }
    }
    for (int x = 0; x < size; ++x) {
        QVector<double*> vector;
        for (int y = 0; y < size; ++y) {
            double *value = new double();
            if(x == y){
                *value = 1 - *ctmcMatrix[x][y]/max;
            }else{
                *value = 0 - *ctmcMatrix[x][y]/max;
            }
            vector << value;
        }
        dtmcMatrix << vector;
    }
}

