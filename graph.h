#ifndef GRAPH_H
#define GRAPH_H

#include <QDialog>
#include <QtCharts/QtCharts>

namespace Ui {
class Graph;
}

class Graph : public QDialog
{
    Q_OBJECT

public:
    explicit Graph(QWidget *parent = nullptr);
    ~Graph();

    void setDataToGraph(QVector<QString> a);
    static int hexToDec(QString b);


private:
    Ui::Graph *ui;
signals:
    void signalZoom(int zoom1);
public slots:
    virtual void wheelEvent(QWheelEvent *event);
   // virtual void mousePressEvent(QMouseEvent *event);
};

#endif // GRAPH_H
