#include "mainwindow.h"
#include "hexvalidator.h"
#include <QDebug>

void MainWindow::createGraphObject() {
    graphic=new Graph(this);
    graphic->setMinimumWidth(1250);
    graphic->setMinimumHeight(800);
    graphic->setWindowTitle("График");
    graphic->setWindowFlags(graphic->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDialogButtonBox *btn_box = new QDialogButtonBox();
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, graphic, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, graphic, &QDialog::reject);
    QGridLayout *layout = new QGridLayout();
    chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::HorizontalRubberBand);
    QObject::connect(graphic, SIGNAL(signalZoom(int)), this, SLOT(zoomAll(int)));
    QStringList red=checkCanDialogText(can1_mdiUI->log,can2_mdiUI->log);
    QVector<QString>datas=parseToGraph(red,"0x18FFD53D");
    qDebug()<<datas;
    setDataToGraph(datas);
    chart->createDefaultAxes();
    layout->addWidget(chartView);
    layout->addWidget(btn_box);
    graphic->setLayout(layout);
    switch(graphic->exec()) {
       case QDialog::Accepted:
            graphic->deleteLater();
            break;
       case QDialog::Rejected:
            graphic->deleteLater();
            break;
     }
}

void MainWindow::setDialogGraph() {
    createGraphdlg=new QDialog(this);
    createGraphdlg->setMinimumWidth(250);
    createGraphdlg->setWindowTitle("Graph");
    createGraphdlg->setWindowFlags(createGraphdlg->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDialogButtonBox *btn_box = new QDialogButtonBox(createGraphdlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, createGraphdlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, createGraphdlg, &QDialog::reject);
    QLabel *lb=new QLabel("ID");
    QLabel *lb1=new QLabel("Start Bit");
    QLabel *lb2=new QLabel("Length");
    QLabel *lb3=new QLabel("Ratio");
    QLineEdit *ed=new QLineEdit(this);
    QLineEdit *ed1=new QLineEdit(this);
    QLineEdit *ed2=new QLineEdit(this);
    QLineEdit *ed3=new QLineEdit(this);
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(lb,0,0);
    layout->addWidget(ed,0,1);
    layout->addWidget(lb1,1,0);
    layout->addWidget(ed1,1,1);
    layout->addWidget(lb2,2,0);
    layout->addWidget(ed2,2,1);
    layout->addWidget(lb3,3,0);
    layout->addWidget(ed3,3,1);
    layout->addWidget(btn_box,4,1);
    createGraphdlg->setLayout(layout);

    switch( createGraphdlg->exec()) {
       case QDialog::Accepted:
            createGraphObject();
            createGraphdlg->deleteLater();
           break;
       case QDialog::Rejected:
           createGraphdlg->deleteLater();
           break;
     }
}

void MainWindow::setDataToGraph(QVector<QString> a){
    auto hexToDec = [](QString b){
        int k=0;
        int y=0;
        int size=b.length()-1;
        for(int i=0;i<b.length();i++){
            if (b[i]=='0'){y=0;}
            if (b[i]=='1'){y=1;}
            if (b[i]=='2'){y=2;}
            if (b[i]=='3'){y=3;}
            if (b[i]=='4'){y=4;}
            if (b[i]=='5'){y=5;}
            if (b[i]=='6'){y=6;}
            if (b[i]=='7'){y=7;}
            if (b[i]=='8'){y=8;}
            if (b[i]=='9'){y=9;}
            if (b[i]=='A'){y=10;}
            if (b[i]=='B'){y=11;}
            if (b[i]=='C'){y=12;}
            if (b[i]=='D'){y=13;}
            if (b[i]=='E'){y=14;}
            if (b[i]=='F'){y=15;}
            k = k + y*pow(16, size);
            size--;
        }
        return k;
    };
    QVector <QVector<int>> datases;
    for (int i=0;i<8;i++){
        QVector<int> vs1;
        datases.append(vs1);
    }
    for(int i=0;i<a.length();i++){
        QVector<QString> as=a[i].split(" ").toVector();
        QVector<QString> asq;
        QVector<double> graphic_new;
        for (int j=0;j<as.length();j++){
            if(as[j]!=""){
                graphic_new.append(hexToDec(as[j]));
            }
        }
        for (int j=0;j<graphic_new.length();j++){
            datases[j].append(graphic_new[j]);
        }

    }
    qDebug()<<datases.length();

    for (int i=0;i<datases.length();i++)
    {
        QLineSeries *series = new QLineSeries();
        QVector<int> s1=datases[i];
        for (int j=0;j<s1.length();j++){
            series->append(j, s1[j]);
        }
        chart->addSeries(series);
    }
}

void MainWindow::zoomAll(int type) {
    if (type == 1) chart->zoomIn();
    else chart->zoomOut();
}

void MainWindow::graphCreate1() {
    if (can1_mdiUI->log->toPlainText().length() == 0) {
        QMessageBox::information(this,"Внимание!","Логи не загружены!");
    }
    else setDialogGraph();
}

void MainWindow::graphCreate2() {
    if (can2_mdiUI->log->toPlainText().length() == 0) {
        QMessageBox::information(this,"Внимание!","Логи не загружены!");
    }
    else setDialogGraph();
}

QVector<QString>MainWindow::parseToGraph(QStringList data, QString ID) {
    QVector<QString> s;
    for (int i = 0; i < data.length(); i++) {
        QVector<QString> vr;
        QStringList s1=data[i].split("     ");
        QStringList s2=s1[0].split(" ");
        QString q;
        for (int j=0;j<s2.length();j++) {
            if (s2[j].startsWith("0x")==true){q=s2[j].trimmed();break;}
        }
        if (q == ID) {
            if (s.contains(s1[1]) != true) {
                s.append(s1[1]);
            }
        }
    }
    return s;
}
