#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QGraphicsTextItem"
#include "QListWidget"
#include <queue>

//можем контролировать радиус вершины и радиус в т.н. уравнении круга
#define ellipseRadius 30
#define radius 120

//кисти, ручки
QBrush whiteBrush(Qt::white);

QPen bluepen(Qt::blue);
QPen redpen(Qt::red);
QPen graypen(Qt::gray);

QPen bluepenDot(Qt::blue);
QPen redpenDot(Qt::red);

std::vector <Connections> edges;
std::queue <int> queueForDFS;
std::vector <int> visitedVertices;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->spinBox->setMinimum(0);
    ui->spinBoxBegin->setMinimum(1);

    scene = new QGraphicsScene(this);
    scene->clearFocus();
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);//сглаживание в сцене (кайф)

    graypen.setWidth(3);

    bluepen.setWidth(2);
    redpen.setWidth(2);

    bluepenDot.setWidth(2);
    redpenDot.setWidth(2);

    bluepen.setCapStyle(Qt::RoundCap);
    redpen.setCapStyle(Qt::RoundCap);

    bluepenDot.setStyle(Qt::DashLine);
    redpenDot.setStyle(Qt::DashLine);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//функция для построения вершин
void MainWindow::paintVerticies(int numberOfverticies){
    if (numberOfverticies==0)
        return;
    for (int i=1; i<=numberOfverticies; ++i){
        scene->addEllipse(radius*cos(i),radius*sin(i),ellipseRadius,ellipseRadius,graypen,whiteBrush);
        QGraphicsTextItem *text = scene->addText(QString::number(i));
        text->setPos(radius*cos(i)+7,radius*sin(i)+3);
    }
};


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    ui->tableSum->setRowCount(arg1);
    ui->tableSum->setColumnCount(arg1);
    ui->spinBoxBegin->setMaximum(arg1);

    for (int i=0;i<ui->tableSum->rowCount();++i){
        for (int j=0;j<ui->tableSum->columnCount();++j) {
            ui->tableSum->setColumnWidth(i,10);
            QTableWidgetItem *Zero = new QTableWidgetItem();
            Zero->setText(QString::fromStdString("0"));
            Zero->setTextAlignment(Qt::AlignHCenter);
            ui->tableSum->setItem(i,j,Zero);
        }
    }
}




void MainWindow::on_pushButton_clicked()
{
    ui->tableSum->clear();
    ui->spinBox->setValue(0);
}


void MainWindow::on_pushButton_2_clicked()
{
    edges.clear();
    scene->clear();

    //проходим по таблице, запоминаем связи между вершинами
    for(int i=0; i<ui->tableSum->rowCount(); ++i){
        for(int j=0; j<ui->tableSum->rowCount(); ++j){
            if (i>j){//проход только выше главной диагонали, т.к. граф неориентированный
                continue;
            }
            if(ui->tableSum->item(i,j)->text()!="0"){
                Connections pair;
                pair.first=i+1;
                pair.second=j+1;
                edges.push_back(pair);
            }
        }
    }

    //для каждой связи в векторе, строим рёбра
    for (Connections item : edges) {
        if(item.first==item.second){
            scene->addEllipse(radius*cos(item.first)-5,radius*sin(item.second)-5,ellipseRadius+5,ellipseRadius+5,bluepenDot);
        }else{
            scene->addLine(radius*cos(item.first)+7,radius*sin(item.first)+3,
                           radius*cos(item.second)+7,radius*sin(item.second)+3,bluepen);
        }
    }
    //после рёбер строим поверх вершины
    paintVerticies(ui->tableSum->columnCount());
}

int numberOfEdge;
void MainWindow::buildEdge(int x, int y, int n){
    ++numberOfEdge;
    QGraphicsTextItem *text = scene->addText("Y" + QString::number(n));
    if(x==y){
        scene->addEllipse(radius*cos(x)-5,radius*sin(y)-5,ellipseRadius+5,ellipseRadius+5,redpenDot);
        text->setPos(radius*cos(x)-20,radius*sin(y)-20);
    }else{
        scene->addLine(radius*cos(x)+7,radius*sin(x)+3,
                       radius*cos(y)+7,radius*sin(y)+3,redpen);
        text->setPos((radius*cos(x)+7+radius*cos(y)+7)/2,
                     (radius*sin(x)+3+radius*sin(y)+3)/2);
    }
}

void MainWindow::on_tableSum_cellClicked(int row, int column)
{
    QTableWidgetItem *mirrored = new QTableWidgetItem();
    mirrored->setText(ui->tableSum->item(row,column)->text());
    mirrored->setTextAlignment(Qt::AlignHCenter);
    ui->tableSum->setItem(column,row,mirrored);
}

bool include(std::vector <int> myVector, int element){
    for (int i=0; i<(int)myVector.size(); ++i){
        if(myVector[i]==element){
            return true;
        }
    }
    return false;
}


//проход в глубь
void MainWindow::on_pushButton_3_clicked()
{
    scene->clear();
    numberOfEdge=1;
    visitedVertices.clear();
    int begin = ui->spinBoxBegin->value()-1;
    findNextEdge(begin);
    paintVerticies(ui->tableSum->columnCount());
}

void MainWindow::findNextEdge(int first){
    visitedVertices.push_back(first);
    for (int j=0;j<ui->tableSum->rowCount();++j){
        if(first==j && ui->tableSum->item(first,j)->text()=="1"){
            buildEdge(first+1,j+1,numberOfEdge);
        }
        if(ui->tableSum->item(first,j)->text()=="1" && !include(visitedVertices,j)){
            buildEdge(first+1,j+1,numberOfEdge);
            findNextEdge(j);
        }
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    scene->clear();
    numberOfEdge=1;
    visitedVertices.clear();
    queueForDFS.push(ui->spinBoxBegin->value()-1);

    do{
        BFS(queueForDFS.front());
    }while(!queueForDFS.empty());
    paintVerticies(ui->tableSum->columnCount());
}

void MainWindow::BFS(int first){
    visitedVertices.push_back(first);
    buildAllOnCurrentRow(first);
    for (int j=0; j<ui->tableSum->rowCount(); ++j){
        if(ui->tableSum->item(first,j)->text()=="1" && !include(visitedVertices,j)){
            queueForDFS.push(j);
            visitedVertices.push_back(j);
        }
    }
    queueForDFS.pop();
}

void MainWindow::buildAllOnCurrentRow(int currentRow){
    for (int j=0; j<ui->tableSum->rowCount(); ++j){
        if(ui->tableSum->item(currentRow,j)->text()=="1" && !include(visitedVertices,j)){
            buildEdge(currentRow+1,j+1,numberOfEdge);
        }
    }
    visitedVertices.erase(std::unique(visitedVertices.begin(),visitedVertices.end()),visitedVertices.end());
}

