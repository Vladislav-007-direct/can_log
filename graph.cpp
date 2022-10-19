#include "graph.h"
#include "ui_graph.h"

Graph::Graph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Graph)
{
    ui->setupUi(this);
}

Graph::~Graph()
{
    delete ui;
}

void Graph::wheelEvent(QWheelEvent *event) {
    if (event->delta() > 0) emit signalZoom(1);
    else emit signalZoom(2);
}