#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>

void MainWindow::disconnect_read_thread_listeners()
{
    disconnect(this->_read_thread, &read_thread::variableConfigurationSuccess,nullptr,nullptr); // disconnect all listeners
    disconnect(this->_read_thread, &read_thread::variableConfigurationFailed,nullptr,nullptr); // disconnect all listeners
    disconnect(this->_read_thread, &read_thread::interfaceConfigurationSuccess,nullptr,nullptr); // disconnect all listeners
    disconnect(this->_read_thread, &read_thread::interfaceConfigurationFailed,nullptr,nullptr); // disconnect all listeners
}

void MainWindow::on_devconnect_triggered()
{

    qDebug() << "emit devcon()";
    emit devcon();
}

void MainWindow::on_devdisconnect_triggered()
{
    qDebug() << "emit devdiscon()";
    emit devdiscon();
}

void MainWindow::on_fileopen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"открыть файл...");

    if(filename.isEmpty())
        return;

    QFile file(filename);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray data = file.readAll();

        parseLog(data);
    }
}

void MainWindow::on_filesave_triggered()
{

}

void MainWindow::on_quit_triggered()
{
    QApplication::quit();
}

void MainWindow::_can1_starterror()
{
    statusBar()->showMessage("### Ошибка ###");
}

void MainWindow::_can1_stopbn()
{
    qDebug() << "emit stopbn";
    emit can_stop();
}

void MainWindow::on_time_sync_triggered()
{
    emit timesync();
}

void MainWindow::on_firmware_update_triggered()
{
    QDialog* d = new QDialog;
    qDebug() << "emit get_fw_info();";

    fwdui->setupUi(d);
    firmwareUpdateWindowOpen = true;
    fwdui->closebn->setVisible(false);
    emit get_fw_info();

    connect(fwdui->updatebn,SIGNAL(clicked()), this, SLOT(update_bn()));
    if(d->exec() == QDialog::Rejected)
    {
        firmwareUpdateWindowOpen = false;
        d->hide();
        delete d;
    }
}

void MainWindow::on_action_dec_txt_triggered()
{
    QString filename = QFileDialog::getSaveFileName();
    if(filename.isEmpty()==false)
    {

        QFile f(filename);
        if(f.open(QIODevice::WriteOnly))
        {

            for(auto cf:export_data_can1)
            {

                cf.frm_id &= ~(0x80000000);

                QByteArray ba = QString("%1 (%2) %3 %4 %5 %6 %7 %8 %9 %10\n")
                        .arg(QString::number(cf.frm_id,10))
                        .arg(QString::number(cf.frm_id,16),8,QChar('0'))
                        .arg(uchar(cf.data[0]))
                        .arg(uchar(cf.data[1]))
                        .arg(uchar(cf.data[2]))
                        .arg(uchar(cf.data[3]))
                        .arg(uchar(cf.data[4]))
                        .arg(uchar(cf.data[5]))
                        .arg(uchar(cf.data[6]))
                        .arg(uchar(cf.data[7])).toLocal8Bit();

                f.write(ba);
            }

            f.close();
        }
    }
}

void MainWindow::on_action_dec_txt_can2_triggered()
{
    QString filename = QFileDialog::getSaveFileName();
    if(filename.isEmpty()==false)
    {
        QFile f(filename);
        if(f.open(QIODevice::WriteOnly))
        {
            for(auto cf:export_data_can2)
            {
                cf.frm_id &= ~(0x80000000);

                QByteArray ba = QString("%1 (%2) %3 %4 %5 %6 %7 %8 %9 %10\n")
                        .arg(QString::number(cf.frm_id,10))
                        .arg(QString::number(cf.frm_id,16),8,QChar('0'))
                        .arg(uchar(cf.data[0]))
                        .arg(uchar(cf.data[1]))
                        .arg(uchar(cf.data[2]))
                        .arg(uchar(cf.data[3]))
                        .arg(uchar(cf.data[4]))
                        .arg(uchar(cf.data[5]))
                        .arg(uchar(cf.data[6]))
                        .arg(uchar(cf.data[7])).toLocal8Bit();

                f.write(ba);
            }
            f.close();
        }
    }

}

void MainWindow::convert1()
{
    if(files1.size())
    {
        this->setCursor(Qt::WaitCursor);

        convert->status->setText(tr("обработка..."));
        convert->progress->setRange(0,files1.size()+1);
        int i=0;
        convert->progress->setValue(0);
        for(auto file_: files1)
        {
            i++;
            convert->progress->setValue(i);


          QFile file(file_);
          if(file.open(QIODevice::ReadOnly))
          {
            QByteArray data = file.readAll();

            export_data_can1.clear();
            parseLog(data, 0);

            QFile f(file_+".dec");
            if(f.open(QIODevice::WriteOnly))
            {
                for(auto cf:export_data_can1)
                {
                    cf.frm_id &= ~(0x80000000);

                    QByteArray ba = QString("%1 (%2) %3 %4 %5 %6 %7 %8 %9 %10\n")
                            .arg(QString::number(cf.frm_id,10))
                            .arg(QString::number(cf.frm_id,16),8,QChar('0'))
                            .arg(uchar(cf.data[0]))
                            .arg(uchar(cf.data[1]))
                            .arg(uchar(cf.data[2]))
                            .arg(uchar(cf.data[3]))
                            .arg(uchar(cf.data[4]))
                            .arg(uchar(cf.data[5]))
                            .arg(uchar(cf.data[6]))
                            .arg(uchar(cf.data[7])).toLocal8Bit();

                    f.write(ba);
                }

                f.close();
                export_data_can1.clear();
            }
          }
        }
        convert->progress->setValue(i+1);
        convert->status->setText(tr("успешно"));
    }
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::convert2()
{
    if(files2.size())
    {

        this->setCursor(Qt::WaitCursor);

        convert->status->setText(tr("обработка..."));
        convert->progress->setRange(0,files2.size()+1);
        int i=0;
        convert->progress->setValue(0);

        for(auto file_: files2)
        {
            i++;
            convert->progress->setValue(i);

            QFile file(file_);
            if(file.open(QIODevice::ReadOnly))
            {
                QByteArray data = file.readAll();

                export_data_can2.clear();
                parseLog(data, 1);

                QFile f(file_+".dec");
                if(f.open(QIODevice::WriteOnly))
                {
                    for(auto cf:export_data_can2)
                    {
                        cf.frm_id &= ~(0x80000000);

                        QByteArray ba = QString("%1 (%2) %3 %4 %5 %6 %7 %8 %9 %10\n")
                                                .arg(QString::number(cf.frm_id,10))
                                                .arg(QString::number(cf.frm_id,16),8,QChar('0'))
                                                .arg(uchar(cf.data[0]))
                                                .arg(uchar(cf.data[1]))
                                                .arg(uchar(cf.data[2]))
                                                .arg(uchar(cf.data[3]))
                                                .arg(uchar(cf.data[4]))
                                                .arg(uchar(cf.data[5]))
                                                .arg(uchar(cf.data[6]))
                                                .arg(uchar(cf.data[7])).toLocal8Bit();

                        f.write(ba);
                    }
                    f.close();
                    export_data_can2.clear();
                }
            }
        }
        convert->progress->setValue(i+1);
        convert->status->setText(tr("успешно"));
    }
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::on_action_dec_txt_can1_triggered()
{

    QDialog* d = new QDialog;
    qDebug() << "emit on_action_dec_txt_can1_triggered();";

    convert->setupUi(d);
    connect(convert->openfiles,SIGNAL(clicked()), this, SLOT(openFiles1()));
    connect(convert->convert,SIGNAL(clicked()), this, SLOT(convert1()));

    if(d->exec() == QDialog::Rejected)
    {
        d->hide();
        delete d;
    }

}

void MainWindow::on_action_dec_txt_can2_2_triggered()
{

    QDialog* d = new QDialog;
    qDebug() << "emit on_action_dec_txt_can2_triggered();";

    convert->setupUi(d);
    connect(convert->openfiles,SIGNAL(clicked()), this, SLOT(openFiles2()));
    connect(convert->convert,SIGNAL(clicked()), this, SLOT(convert2()));

    if(d->exec() == QDialog::Rejected)
    {
        d->hide();
        delete d;
    }
}

void MainWindow::on_actionASC_triggered() {
    convertToASCII();
}

void MainWindow::on_action_Hex_triggered() {
    convertToHex();
}
