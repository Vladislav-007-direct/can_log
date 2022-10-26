#include "mainwindow.h"
#include <QDebug>
#include <QQueue>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>

void MainWindow::can1_saveLog()
{
    QDir d;
    QString log = can1_mdiUI->log->toPlainText();
    QFile file(d.absoluteFilePath(QString("can1_%1.txt").arg(QDateTime::currentSecsSinceEpoch())));
    if (file.open(QIODevice::ReadWrite))
    {
        file.write(log.toUtf8());
        file.close();
    }
}

void MainWindow::can2_saveLog()
{
    QDir d;
    QString log = can2_mdiUI->log->toPlainText();
    QFile file(d.absoluteFilePath(QString("can2_%1.txt").arg(QDateTime::currentSecsSinceEpoch())));
    if (file.open(QIODevice::ReadWrite))
    {
        file.write(log.toUtf8());
        file.close();
    }
}

void MainWindow::write_settings()
{
    if (can1Settings){
        QSettings settings("ui.conf",QSettings::IniFormat);

        settings.beginGroup("can1.settings.dialog");
        settings.setValue("startByte",      can1SettingsUI->startByteLength->selectionModel()->selectedRows().at(0).row());
        settings.setValue("length",         can1SettingsUI->startByteLength->selectedItems().length());
        settings.setValue("add.options",    can1SettingsUI->canAddOptions->isChecked());
        settings.setValue("logging",        can1SettingsUI->canLogging->isChecked());
        settings.setValue("filter",         can1SettingsUI->canFilter->isChecked());
        settings.setValue("id",             can1SettingsUI->canIDHEX->text());
        settings.setValue("value",          can1SettingsUI->canIDvalue->text());
        settings.setValue("condition",      can1SettingsUI->canIDcondition->currentIndex());
        settings.setValue("mode",           can1SettingsUI->canMode->currentIndex());
        settings.setValue("protocol",       can1SettingsUI->canProtocol->currentIndex());
        settings.setValue("speed",          can1SettingsUI->canSpeed->currentIndex());
        settings.setValue("typeID",         can1SettingsUI->canTypeID->currentIndex());
        settings.endGroup();
    }

    if (can2Settings){
        QSettings settings("ui.conf",QSettings::IniFormat);

        settings.beginGroup("can2.settings.dialog");
        settings.setValue("startByte",      can2SettingsUI->startByteLength->selectionModel()->selectedRows().at(0).row());
        settings.setValue("length",         can2SettingsUI->startByteLength->selectedItems().length());
        settings.setValue("add.options",    can2SettingsUI->canAddOptions->isChecked());
        settings.setValue("logging",        can2SettingsUI->canLogging->isChecked());
        settings.setValue("filter",         can2SettingsUI->canFilter->isChecked());
        settings.setValue("id",             can2SettingsUI->canIDHEX->text());
        settings.setValue("value",          can2SettingsUI->canIDvalue->text());
        settings.setValue("condition",      can2SettingsUI->canIDcondition->currentIndex());
        settings.setValue("mode",           can2SettingsUI->canMode->currentIndex());
        settings.setValue("protocol",       can2SettingsUI->canProtocol->currentIndex());
        settings.setValue("speed",          can2SettingsUI->canSpeed->currentIndex());
        settings.setValue("typeID",         can2SettingsUI->canTypeID->currentIndex());
        settings.endGroup();
    }
}

void MainWindow::save_can1_send_settings_window_configuration()
{
    QString filename = QFileDialog::getSaveFileName(this,"Сохранить...",QString(),QString("Файлы настройки (*.conf)"));

    if (filename.isEmpty())
        return;

    QSettings settings(filename,QSettings::IniFormat);
    settings.beginWriteArray("packets");
    for (int i=0;i<can1_packetsUI.length();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("sendIt",       can1_packetsUI[i]->sendIt->isChecked());
        settings.setValue("frameID",      can1_packetsUI[i]->frameID->text());
        settings.setValue("frameType",    can1_packetsUI[i]->frameType->currentIndex());
        settings.setValue("messageLength",can1_packetsUI[i]->messageLength->currentIndex());
        settings.setValue("message",      can1_packetsUI[i]->message->text());
        settings.setValue("delay",        can1_packetsUI[i]->delay->text());
        settings.setValue("repeat",       can1_packetsUI[i]->repeat->text());
    }
    settings.endArray();
    settings.beginGroup("general");
    settings.setValue("repeats",can1_send_optionsUI->repeats->text());
    settings.endGroup();
}

void MainWindow::save_can2_send_settings_window_configuration()
{
    QString filename = QFileDialog::getSaveFileName(this,"Сохранить...",QString(),QString("Файлы настройки (*.conf)"));

    if (filename.isEmpty())
        return;

    QSettings settings(filename,QSettings::IniFormat);
    settings.beginWriteArray("packets");
    for (int i=0;i<can2_packetsUI.length();i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("sendIt",       can2_packetsUI[i]->sendIt->isChecked());
        settings.setValue("frameID",      can2_packetsUI[i]->frameID->text());
        settings.setValue("frameType",    can2_packetsUI[i]->frameType->currentIndex());
        settings.setValue("messageLength",can2_packetsUI[i]->messageLength->currentIndex());
        settings.setValue("message",      can2_packetsUI[i]->message->text());
        settings.setValue("delay",        can2_packetsUI[i]->delay->text());
        settings.setValue("repeat",       can2_packetsUI[i]->repeat->text());
    }
    settings.endArray();
    settings.beginGroup("general");
    settings.setValue("repeats",can2_send_optionsUI->repeats->text());
    settings.endGroup();
}

QQueue<trblock> MainWindow::parseLogHelper(QByteArray &data) {
    qDebug() << "Parse Log Helper";
    quint16 len = 0;

    QQueue<trblock> result;
    qDebug() << "TrBlock size: " << (int)sizeof(trblock);
    for(int i=0; i+(int)sizeof(trblock) <= data.size(); ) {
        trblock block;
        memcpy(&block,&data.data()[i],sizeof(trblock));

        quint8 my_crc8 = crc8(&data.data()[i],block.header.messageLength-1);
        quint8 _crc8 = block.crc;

        if(my_crc8 != _crc8) {
            i++;
            continue;
        }

        result.enqueue(block);
        i+=sizeof(trblock);
    }

    return result;
}

void MainWindow::parseLog(QByteArray data, int silent)
{
    qDebug() << "parselog";
    qDebug() << data.toHex(' ');

    QQueue<trblock> blocks = parseLogHelper(data);
    while (blocks.count())
    {
        trblock block = blocks.dequeue();
        can_frm cf = block.canFrame;
        int niface = block.header.interfaceNumber;

        // Logging
        if(niface == 0)
        {
            if((export_data_can1.size()%10240)==0 || export_data_can1.size()==0)
                export_data_can1.reserve(export_data_can1.size()+10240);

            export_data_can1.append(cf);
        }
        if(niface == 1)
        {
            if((export_data_can2.size()%10240)==0 || export_data_can2.size()==0)
                export_data_can2.reserve(export_data_can2.size()+10240);

            export_data_can2.append(cf);
        }

        // Visualisation
        quint64 timestamp = block.header.timestamp;
        QDateTime time = QDateTime::fromMSecsSinceEpoch(timestamp);
        time.setTimeSpec(Qt::UTC);

        QByteArray frameID = QByteArray::fromRawData(reinterpret_cast<char*>(&block.canFrame.frm_id),sizeof(can_frm::frm_id));
        std::reverse(frameID.begin(), frameID.end());

        QString str;
        str = QString("RX ") + MainWindow::create_table_row_string(
                niface==0 ? rx1_count++ : rx2_count++,
                frameID.toHex().toUpper(),
                block.canFrame.dlen,
                QByteArray::fromRawData(reinterpret_cast<char*>(block.canFrame.data),sizeof(can_frm::data)),
                time.toSecsSinceEpoch(),
                (uchar)time.date().day(),
                (uchar)time.date().month(),
                (quint16)time.date().year(),
                (uchar)time.time().hour(),
                (uchar)time.time().minute(),
                (uchar)time.time().second(),
                time.time().msec());

        if(niface == 0 && !silent)
        {
            show_can1_mdi_window();
            QTextCharFormat tf = can1_mdiUI->log->currentCharFormat();
            tf.setForeground(QBrush(Qt::black));
            can1_mdiUI->log->setCurrentCharFormat(tf);
            can1_mdiUI->log->appendPlainText(str);
        }

        if(niface == 1 && !silent)
        {
            show_can2_mdi_window();
            QTextCharFormat tf = can2_mdiUI->log->currentCharFormat();
            tf.setForeground(QBrush(Qt::black));
            can2_mdiUI->log->setCurrentCharFormat(tf);
            can2_mdiUI->log->appendPlainText(str);
        }
    }
    QStringList red=checkCanDialogText(can1_mdiUI->log,can2_mdiUI->log);
    //qDebug()<<red;
//    QVector<QVector<QString>>s=parseToGraph(red);

}

QString MainWindow::create_table_row_string(int count, QByteArray frameID, uchar length, QByteArray data, qint64 timestamp, quint8 day, quint8 month, quint16 year, quint8 hours, quint8 minutes, quint8 seconds, quint16 milliseconds)
{
    QString str = QString("%1%2%3%20%4%21%5%21%6%21%7%21%8%21%9%21%10%21%11%20%12 %13.%14.%15 %16:%17:%18.%19 ")
        .arg(count,4)
        .arg(QString("0x")+QString(frameID).toUpper(),12)
        .arg(length,4, 16)
        .arg(QString::number((uchar)data[0],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(QString::number((uchar)data[1],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(QString::number((uchar)data[2],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(QString::number((uchar)data[3],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(QString::number((uchar)data[4],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(QString::number((uchar)data[5],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(QString::number((uchar)data[6],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(QString::number((uchar)data[7],16).rightJustified(2,QChar('0'),true).toUpper())
        .arg(timestamp,10) // 12
        .arg(QString::number(day),2,QChar('0'))
        .arg(QString::number(month),2,QChar('0'))
        .arg(QString::number(year),4)
        .arg(QString::number(hours),2,QChar('0'))
        .arg(QString::number(minutes),2,QChar('0'))
        .arg(QString::number(seconds),2,QChar('0'))
        .arg(QString::number(milliseconds),3,QChar('0'))
        .arg(QString(""),5,QChar(' ')) // %20 is 12 char spacer
        .arg(QString(""),2,QChar(' ')); // %21 is 2 char spacer
    return str;
}

QString MainWindow::create_table_header_string()
{
    return QString("%1%2%3%15%4%5%6%7%8%9%10%11%15%12%13%14")
            .arg(QString("CNT"),7)
            .arg(QString("FRM ID"),12)
            .arg(QString("LN"),4)
            .arg(QString("D0"),2)
            .arg(QString("D1"),4)
            .arg(QString("D2"),4)
            .arg(QString("D3"),4)
            .arg(QString("D4"),4)
            .arg(QString("D5"),4)
            .arg(QString("D6"),4)
            .arg(QString("D7"),4)
            .arg(QString("TIMESTAMP"),10)
            .arg(QString("DD.MM.YYYY"),11)
            .arg(QString("TIME"),13)
            .arg(QString(""),5,QChar(' ')); // %15 is a spacer
}

void MainWindow::openFiles1()
{
    qDebug() << "openfiles1";
    files1 = QFileDialog::getOpenFileNames();

    if(files1.size())
    {
        convert->files->clear();
        for(auto file: files1)
        {
         convert->files->addItem(file);
        }
        convert->status->setText(tr("файлы выбраны"));
        convert->progress->setValue(0);
    }
    else
    {
        convert->status->setText(tr("файлы не выбраны"));
        convert->progress->setValue(0);
    }

}

void MainWindow::openFiles2()
{
    qDebug() << "openfiles2";
    files2 = QFileDialog::getOpenFileNames();

    if(files2.size())
    {
        convert->files->clear();
        for(auto file: files2)
        {
         convert->files->addItem(file);
        }
        convert->status->setText(tr("файлы выбраны"));
        convert->progress->setValue(0);

    }
    else
    {
        convert->status->setText(tr("файлы не выбраны"));
        convert->progress->setValue(0);

    }
}

void MainWindow::convertToHex()
{
    QDialog* d = new QDialog;
    qDebug() << "action <convert to HEX> started";

    QStringList files;
    convert->setupUi(d);
    connect(convert->openfiles,&QPushButton::clicked,this,[&files,this](){
        files = QFileDialog::getOpenFileNames();
        if(files.size())
        {
            this->convert->files->clear();
            for(auto file: files)
            {
                qDebug() << "file added";
                this->convert->files->addItem(file);
            }
            this->convert->status->setText(tr("файлы выбраны"));
            this->convert->progress->setValue(0);
        }
        else
        {
            convert->status->setText(tr("файлы не выбраны"));
            convert->progress->setValue(0);
        }
    });
    connect(convert->convert,&QPushButton::clicked, this, [&files,this](){
        QQueue<trblock> can1;
        QQueue<trblock> can2;

        if(files.size())
        {
            this->setCursor(Qt::WaitCursor);
            convert->status->setText(tr("чтение файлов..."));

            QList<QQueue<trblock>> logsCan1;
            QList<QQueue<trblock>> logsCan2;

            for(auto file_: files)
            {
                QFile file(file_);
                if(file.open(QIODevice::ReadOnly))
                {
                    QByteArray data = file.readAll();
                    QQueue<trblock> queue = parseLogHelper(data);

                    QQueue<trblock> queueCan1;
                    QQueue<trblock> queueCan2;

                    while(queue.size())
                    {
                        if (queue.first().header.interfaceNumber == 0)
                            queueCan1.enqueue(queue.dequeue());
                        else
                            queueCan2.enqueue(queue.dequeue());
                    }
                    if (queueCan1.size())
                    {
                        std::stable_sort(queueCan1.begin(),queueCan1.end(), [](const trblock &block1, const trblock &block2)
                        {
                            return block1.header.timestamp < block2.header.timestamp;
                        });
                        logsCan1.append(queueCan1);
                    }
                    if (queueCan2.size())
                    {
                        std::stable_sort(queueCan2.begin(),queueCan2.end(),[](const trblock &block1, const trblock &block2)
                        {
                            return block1.header.timestamp < block2.header.timestamp;
                        });
                        logsCan2.append(queueCan2);
                    }
                }
            }
            convert->status->setText(tr("обработка..."));
            convert->progress->setRange(0,logsCan1.size()+logsCan2.size());
            int i=0;
            convert->progress->setValue(0);

            while (logsCan1.size())
            {
                // find minimum value over file queue's
                auto minimumValue = ((quint64)1)<<62;
                auto minimumIndex = 0;

                for (int j=0;j<logsCan1.size();j++)
                {
                    if (logsCan1[j].constFirst().header.timestamp < minimumValue)
                    {
                        minimumValue = logsCan1[j].constFirst().header.timestamp;
                        minimumIndex = j;
                    }
                }
                can1.enqueue(logsCan1[minimumIndex].dequeue());

                // remove empty queue's
                for (int j=0;j<logsCan1.size();j++)
                {
                    if (!logsCan1[j].size())
                    {
                        logsCan1.removeAt(j);
                        i++;
                        convert->progress->setValue(i);
                    }
                }
            }

            while (logsCan2.size())
            {
                // find minimum value over file queue's
                auto minimumValue = ((quint64)1)<<62;
                auto minimumIndex = 0;

                for (int j=0;j<logsCan2.size();j++)
                {
                    if (logsCan2[j].constFirst().header.timestamp < minimumValue)
                    {
                        minimumValue = logsCan2[j].constFirst().header.timestamp;
                        minimumIndex = j;
                    }
                }
                can2.enqueue(logsCan2[minimumIndex].dequeue());

                // remove empty queue's
                for (int j=0;j<logsCan2.size();j++)
                {
                    if (!logsCan2[j].size())
                    {
                        logsCan2.removeAt(j);
                        i++;
                        convert->progress->setValue(i);
                    }
                }
            }
            convert->status->setText(tr("запись на диск..."));
        }

        quint64 currentTS = QDateTime::currentMSecsSinceEpoch();

        QFile fcan1(QString("can1_%1.txt").arg(currentTS));
        if(fcan1.open(QIODevice::WriteOnly))
        {
            int _rx1_count = 1;
            while(can1.size())
            {
                auto block = can1.dequeue();

                quint64 timestamp = block.header.timestamp;
                QDateTime time = QDateTime::fromMSecsSinceEpoch(timestamp);
                time.setTimeSpec(Qt::UTC);

                QByteArray frameID = QByteArray::fromRawData(reinterpret_cast<char*>(&block.canFrame.frm_id),sizeof(can_frm::frm_id));
                std::reverse(frameID.begin(), frameID.end());

                QString str = MainWindow::create_table_row_string(_rx1_count++,
                                                                frameID.toHex().toUpper(),
                                                                block.canFrame.dlen,
                                                                QByteArray::fromRawData(reinterpret_cast<char*>(block.canFrame.data),sizeof(can_frm::data)),
                                                                time.toSecsSinceEpoch(),
                                                                (uchar)time.date().day(),
                                                                (uchar)time.date().month(),
                                                                (quint16)time.date().year(),
                                                                (uchar)time.time().hour(),
                                                                (uchar)time.time().minute(),
                                                                (uchar)time.time().second(),
                                                                time.time().msec()) + QString('\n');
                QByteArray ba = str.toLocal8Bit();

                fcan1.write(ba);
            }
            fcan1.close();

            QFile fcan2(QString("can2_%1.txt").arg(currentTS));
            if(fcan2.open(QIODevice::WriteOnly))
            {
                int _rx2_count = 1;
                while(can2.size())
                {
                    auto block = can2.dequeue();

                    quint64 timestamp = block.header.timestamp;
                    QDateTime time = QDateTime::fromMSecsSinceEpoch(timestamp);
                    time.setTimeSpec(Qt::UTC);

                    QByteArray frameID = QByteArray::fromRawData(reinterpret_cast<char*>(&block.canFrame.frm_id),sizeof(can_frm::frm_id));
                    std::reverse(frameID.begin(), frameID.end());

                    QString str = MainWindow::create_table_row_string(_rx2_count++,
                                                                    frameID.toHex().toUpper(),
                                                                    block.canFrame.dlen,
                                                                    QByteArray::fromRawData(reinterpret_cast<char*>(block.canFrame.data),sizeof(can_frm::data)),
                                                                    time.toSecsSinceEpoch(),
                                                                    (uchar)time.date().day(),
                                                                    (uchar)time.date().month(),
                                                                    (quint16)time.date().year(),
                                                                    (uchar)time.time().hour(),
                                                                    (uchar)time.time().minute(),
                                                                    (uchar)time.time().second(),
                                                                    time.time().msec()) + QString('\n');
                    QByteArray ba = str.toLocal8Bit();

                    fcan2.write(ba);
                }
                fcan2.close();
            }

            convert->status->setText(tr("успешно"));
        }
        this->setCursor(Qt::ArrowCursor);
    });

    if(d->exec() == QDialog::Rejected)
    {
        d->hide();
        delete d;
    }
}

void MainWindow::convertToASCII()
{
    QDialog* d = new QDialog;
    qDebug() << "action <convert to ASCI> started";

    QStringList files;
    convert->setupUi(d);
    connect(convert->openfiles,&QPushButton::clicked,this,[&files,this]()
    {
        files = QFileDialog::getOpenFileNames(this, tr("Open firmware file"), "./", tr("Binary Files (*.log)"));
        if(files.size())
        {
            this->convert->files->clear();
            for(auto file: files)
            {
                qDebug() << "file added";
                this->convert->files->addItem(file);
            }
            this->convert->status->setText(tr("файлы выбраны"));
            this->convert->progress->setValue(0);
        }
        else
        {
            convert->status->setText(tr("файлы не выбраны"));
            convert->progress->setValue(0);
        }
    });

    connect(convert->convert,&QPushButton::clicked, this, [&files,this]()
    {
        if (files.size())
        {
            this->setCursor(Qt::WaitCursor);
            convert->status->setText(tr("чтение файлов..."));

            for (auto fileName : files)
            {
                QFile file(fileName);

                if (file.open(QIODevice::ReadOnly))
                {
                    QByteArray logRaw = file.readAll();

                    QQueue<trblock> blocks = parseLogHelper(logRaw);

                    std::stable_sort(blocks.begin(),blocks.end(), [](const trblock &block1, const trblock &block2)
                    {
                        return block1.header.timestamp < block2.header.timestamp;
                    });


                    QFile newFile(fileName + ".asc");

                    convert->status->setText(tr("запись на диск..."));

                    if(newFile.open(QIODevice::WriteOnly))
                    {
                        // write header
                        uint64_t timeStartMs = 0;
                        // get current time
                        timeStartMs = blocks.first().header.timestamp;
                        // get current date
                        QDateTime time = QDateTime::fromMSecsSinceEpoch(timeStartMs);
                        QString timeStr = QLocale("en_EN").toString(time, "ddd MMM dd hh:mm:ss.zzz ap yyyy");

                        QByteArray headerArray;
                        headerArray.clear();
                        headerArray.append("date ");
                        headerArray.append(timeStr.toLocal8Bit());
                        headerArray.append("\r\n");
                        headerArray.append("base hex  timestamps absolute\r\n");
                        headerArray.append("no internal events logged\r\n");
                        headerArray.append("// version 7.0.0\r\n");
                        headerArray.append("Begin TriggerBlock ");
                        headerArray.append(timeStr.toLocal8Bit());
                        headerArray.append("\r\n");

                        newFile.write(headerArray);

                        // time
                        long sec, msec;
                        sec = 0;
                        msec = 0;

                        // start of measurment
                        QByteArray ba = QString("%1.%2 Start of measurement\r\n")
                                .arg(long(sec), 4, 10)
                                .arg(long((msec) * 1000), 6, 10, QLatin1Char('0')).toLocal8Bit();

                        newFile.write(ba);

                        convert->progress->setRange(0,blocks.size());
                        size_t blocksSizeStart = blocks.size();

                        while (blocks.size())
                        {
                            trblock block = blocks.dequeue();

                            char szId[32];
                            char szIdStr[32];
                            char szLen[32];

                            memset(szId, 0, sizeof(szId));
                            memset(szIdStr, 0, sizeof(szIdStr));

                            sprintf(szIdStr, "%X%c", block.canFrame.frm_id, 'x');
                            sprintf(szId, "%-15s Rx   ", szIdStr);
                            sprintf(szLen, "d %d", block.canFrame.dlen); /* data frame */

                            QString canData = QString("%1 %2 %3 %4 %5 %6 %7 %8")
                                    .arg(uchar(block.canFrame.data[0]), 2, 16, QLatin1Char('0'))
                                    .arg(uchar(block.canFrame.data[1]), 2, 16, QLatin1Char('0'))
                                    .arg(uchar(block.canFrame.data[2]), 2, 16, QLatin1Char('0'))
                                    .arg(uchar(block.canFrame.data[3]), 2, 16, QLatin1Char('0'))
                                    .arg(uchar(block.canFrame.data[4]), 2, 16, QLatin1Char('0'))
                                    .arg(uchar(block.canFrame.data[5]), 2, 16, QLatin1Char('0'))
                                    .arg(uchar(block.canFrame.data[6]), 2, 16, QLatin1Char('0'))
                                    .arg(uchar(block.canFrame.data[7]), 2, 16, QLatin1Char('0')).toUpper();

                            QByteArray ba = QString("%1.%2 %3  %4 %5 %6 %7\r\n")
                                    .arg(long((block.header.timestamp - timeStartMs)/1000), 4, 10)
                                    .arg(long((block.header.timestamp - timeStartMs)%1000)*1000UL, 6, 10, QLatin1Char('0'))
                                    .arg(int(block.header.interfaceNumber + 1))
                                    .arg(szId, strlen(szId))
                                    .arg(szLen, strlen(szLen))
                                    .arg(canData)
                                    .arg(block.header.timestamp).toLocal8Bit();

                            newFile.write(ba);

                            convert->progress->setValue(blocksSizeStart - blocks.size());

                        }
                        newFile.write("End TriggerBlock\r\n");
                        /*char c = 0x0d;
                        file.write(&c,1);
                        c = 0x0a;
                        file.write(&c,1);*/
                        newFile.close();
                    }
                    file.close();
                    convert->status->setText(tr("успешно"));
                }
                convert->progress->setValue(0);
                this->setCursor(Qt::ArrowCursor);
            }
        }
    });

    if(d->exec() == QDialog::Rejected)
    {
        d->hide();
        delete d;
    }
}
