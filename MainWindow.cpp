#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    simulator = new MarkovSimulator();
    connect(simulator,SIGNAL(resultsReady(QVector<double>, QVector<double>)),this,SLOT(printResults(QVector<double>, QVector<double>)));
    steps = 100000;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete simulator;
}


//Botão de abrir arquivo:
//1 - Limpa as tabelas
//2 - Abre a caixa de diálogo que permite a busca do arquivo
//3 - Manda o caminho do arquivo para o método openFile()
//4 - Verifica se a matriz é quadrada: mesmo número de linhas e colunas
//5 - Coloca a matriz no Simulador
//6 - Chama os métodos fillCRMCTable() e fillDTMCTable() para preencher as duas tabelas de transições
//7 - Ajusta os cabeçalhos da terceira tabela para receber os resultados
void MainWindow::on_actionOpen_triggered()
{
	// 1
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->setColumnCount(0);
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->setColumnCount(0);
	
	//2
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("Text Files (*.txt)"));
	
	//3
    QVector<QVector<double*> > matrix = openFile(fileName);
    
	//4
	for (int x = 0; x < matrix.size(); ++x) {
        if(matrix[x].size() != matrix.size()){
            qDebug() << "ERROR";
            return;
        }
    }

    fileName=fileName.remove(0,fileName.lastIndexOf("/")+1);
    this->setWindowTitle(QString("Markov Chain Simulator - %1").arg(fileName));

	//5
	simulator->setCtmcMatrix(matrix);
    
	//6
	fillCTMCTable();
    fillDTMCTable();
	
	//7
    ui->tableWidget_3->setColumnCount(simulator->getSize()+1);	
    for (int x = 0; x < simulator->getSize(); ++x) {
        QTableWidgetItem *header = new QTableWidgetItem(QString::number(x+1));
        header->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_3->setHorizontalHeaderItem(x,header);
    }
    QTableWidgetItem *header = new QTableWidgetItem("Sum");
    header->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget_3->setHorizontalHeaderItem(simulator->getSize(),header);
    ui->tableWidget_3->setRowCount(2);
    header = new QTableWidgetItem("Visits");
    ui->tableWidget_3->setVerticalHeaderItem(0,header);
    header = new QTableWidgetItem("Percentages");
    ui->tableWidget_3->setVerticalHeaderItem(1,header);

}

//Método de abertura e leitura do arquivo
//Recebe o caminho do arquivo
//Retorna a matriz
//1 - Tenta abrir o arquivo, exibindo uma mensagem de erro caso não consiga
//2 - Abre uma stream para leitura do arquivo
//3 - Teste para verificar se a linha deve ser ignorada: comentário ou linha em branco
//4 - Guarda o tamanho da matriz, que se encontra sempre como a primeira linha do arquivo
//		Exibiando uma mensagem de erro caso não consiga ler o tamanho
//5 - Quebra a linha nos espaços em branco, pois os elementos da matriz são separados por espaço
//6 - Exibe mensagem de erro caso seja encontrado mais elementos dentro da linha da matriz do que o tamanho especificado
//7 - Converte o valor da matriz de QString para double para ser guardado em um vetor
//8 - Guarda o vetor em um vetor de vetores, para ser feita a matriz

QVector<QVector<double*> > MainWindow::openFile(QString fileName)
{
	//1
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox message;
        message.setText(tr("File could not be oppened!"));
        message.setIcon(QMessageBox::Warning);
        message.exec();
        return QVector<QVector<double*> >();
    }
	
	//2
    QTextStream stream(&file);
    int size = 0;
    QVector<QVector<double*> > matrix;
	
    while(!stream.atEnd()){
        QString line = stream.readLine();
		//3
        if(line == "" || line.startsWith("#")){ //Linha em branco ou comentário
            continue;
        }
		//4
        if(size == 0){
            size = line.toInt();
            if(size == 0){
                fileNotSupported(QString("Matrix size is not first line in file"));
                return QVector<QVector<double*> >();
            }
            continue;
        }
		//5
        QStringList values = line.split(" ");
        //qDebug() << values;
		//6
        if(values.size() != size){
            fileNotSupported(QString("Matrix line size is not iqual to size.\n"
										"Size: %1\n"
                                        "Current line:%2 ").arg(size).arg(values.size()));
            return QVector<QVector<double*> >();
        }
		//7
        QVector<double*> vector;
        for (int x = 0; x < size; ++x) {
            bool ok;
            double *value = new double();
            *value = values.at(x).toDouble(&ok);
            if(!ok){
                fileNotSupported();
                return QVector<QVector<double*> >();
            }
            vector << value;
        }
        matrix.push_back(vector);
    }
    return matrix;
}

//Método que mostra uma mensagem de erro ao usuário
//Sempre fala "File not supported!", mas aceita como parâmetro alguma informação extra
//Se o método for chamado sem ser mandado parâtro, mostra somente a mensagem padrão
void MainWindow::fileNotSupported(QString info)
{
    QMessageBox message;
    message.setText(tr("File is not supported!"));
	message.setInformativeText(info);
    message.setIcon(QMessageBox::Warning);
    message.exec();
}

//Preenche a tabela CTMC(tableWidget) da interface gráfica com a matriz contida no simulador
void MainWindow::fillCTMCTable()
{
    QVector<QVector<double> > matrix = simulator->getCtmcMatrix();

    int size = matrix.size();
    ui->tableWidget->setRowCount(size);
    ui->tableWidget->setColumnCount(size);
    QTableWidgetItem *header;
    for (int x = 0; x < size; ++x) {
        header = new QTableWidgetItem(QString::number(x+1));
        header->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setVerticalHeaderItem(x,header);
        header = new QTableWidgetItem(QString::number(x+1));
        header->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setHorizontalHeaderItem(x,header);
        for (int y = 0; y < size; ++y) {
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(matrix[x][y]));
            ui->tableWidget->setItem(x, y, newItem);
        }
    }
    ui->tableWidget->resizeColumnsToContents();
}


//Preenche a tabela DTMC(tableWidget_2) da interface gráfica com a matriz contida no simulador
void MainWindow::fillDTMCTable()
{
    QVector<QVector<double> > matrix = simulator->getDtmcMatrix();

    int size = matrix.size();
    ui->tableWidget_2->setRowCount(size);
    ui->tableWidget_2->setColumnCount(size);
    QTableWidgetItem *header;
    for (int x = 0; x < matrix.size(); ++x) {
        if(matrix[x].size() != matrix.size()){
            qDebug() << "ERROR";
            return;
        }
    }
    for (int x = 0; x < size; ++x) {
        header = new QTableWidgetItem(QString::number(x+1));
        header->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setVerticalHeaderItem(x,header);
        header = new QTableWidgetItem(QString::number(x+1));
        header->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setHorizontalHeaderItem(x,header);
        for (int y = 0; y < size; ++y) {
            QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(matrix[x][y]));
            ui->tableWidget_2->setItem(x, y, newItem);
        }
    }
    ui->tableWidget_2->resizeColumnsToContents();
}

//Ação do botão "Run": realiza a simulação e chama o método printResults() para preencher a tabela de resultados
void MainWindow::on_actionRun_triggered()
{
   QVector<double> results = simulator->simulate(steps);
   if(!results.isEmpty()){
       printResults(simulator->getNumberVisits(), results);
   }
}

//Ação do botão "Steps", define quantos passos/iterações a simulação deve fazer
void MainWindow::on_actionSteps_triggered()
{
    steps = QInputDialog::getInt(this, tr("Simulation Steps"),
								 tr("Enter the number of steps to simulate:"),steps);
}

//Ação do botão "About", mostra informações do aplicativo
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox message;
    message.setText(tr("Markov Chain Simulator program details."));
    message.setInformativeText(tr("Program made as part of an assignment for the\n"
                               "\"Simulação e Avaliação de Sistemas Computacionais\" course at Unisc\n\n"
                               "Authors: Alessandra Helena Jandrey and Henrique Jung\n"
                               "Professor: Ricardo M. Czekster"));
    message.exec();
}

//Ação do botão "Run with thread",
//Faz a simulção em uma thread, assim a interface não fica "travada" enquanto a simulação está acontecendo
void MainWindow::on_actionRun_with_Thread_triggered()
{
    simulator->simulateThreaded(steps);    
}

//Preenche a tabela de resultados(tableWidget_3) com o vetor recebido por parâmetro
void MainWindow::printResults(QVector<double> visits, QVector<double> results)
{
    for (int x = 0; x < results.size(); ++x) {
        QTableWidgetItem *newItem = new QTableWidgetItem(QString::number(results[x]));
        ui->tableWidget_3->setItem(1, x, newItem);        
        newItem = new QTableWidgetItem(QString::number(visits[x]));
        ui->tableWidget_3->setItem(0, x, newItem);
    }
}

void MainWindow::on_actionRun_Several_Threads_triggered()
{
    simulator->simulateMultipleThreads(steps);
}
