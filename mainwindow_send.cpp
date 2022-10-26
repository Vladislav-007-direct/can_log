#include "mainwindow.h"
#include "geostreampacketvirtual.h"
#include <QDebug>
#include <QDateTime>
#include <QTimeZone>

void MainWindow::send_crane_info() {
    disconnect_read_thread_listeners();
    qDebug() << "Crane send identification";
    craneDlgUI->log->appendPlainText(QString("Отправка настроек крана"));

    cranestruct crn;
    QString elem0 = craneDlgUI->elem0->text();
    memset(reinterpret_cast<void *>(&crn.elem0), 0, sizeof(crn.elem0));
    memcpy(crn.elem0, elem0.utf16(), elem0.size() * 2);
    QString elem1 = craneDlgUI->elem1->text();
    memset(reinterpret_cast<void *>(&crn.elem1), 0, sizeof(crn.elem1));
    memcpy(crn.elem1, elem1.utf16(), elem1.size() * 2);
    QString elem2 = craneDlgUI->elem2->text();
    memset(reinterpret_cast<void *>(&crn.elem2), 0, sizeof(crn.elem2));
    memcpy(crn.elem2, elem2.utf16(), elem2.size() * 2);
    QString elem3 = craneDlgUI->elem3->text();
    memset(reinterpret_cast<void *>(&crn.elem3), 0, sizeof(crn.elem3));
    memcpy(crn.elem3, elem3.utf16(), elem3.size() * 2);
    crn.elem4 = craneDlgUI->elem4->value();
    crn.elem5 = craneDlgUI->elem5->value();
    crn.elem6 = craneDlgUI->elem6->value();
    QString elem7 = craneDlgUI->elem7->text();
    memset(reinterpret_cast<void *>(&crn.elem7), 0, sizeof(crn.elem7));
    memcpy(crn.elem7, elem7.utf16(), elem7.size() * 2);
    QString elem8 = craneDlgUI->elem7->text();
    memset(reinterpret_cast<void *>(&crn.elem8), 0, sizeof(crn.elem8));
    memcpy(crn.elem8, elem8.utf16(), elem8.size() * 2);
    QString elem9 = craneDlgUI->elem7->text();
    memset(reinterpret_cast<void *>(&crn.elem9), 0, sizeof(crn.elem9));
    memcpy(crn.elem9, elem9.utf16(), elem9.size() * 2);
    crn.elem10 = QDateTime(craneDlgUI->elem10->date()).toSecsSinceEpoch();
    crn.elem11 = craneDlgUI->elem11->value();
    crn.elem17 = QDateTime(craneDlgUI->elem17->date()).toSecsSinceEpoch();
    QString elem18 = craneDlgUI->elem18->text();
    memset(reinterpret_cast<void *>(&crn.elem18), 0, sizeof(crn.elem18));
    memcpy(crn.elem18, elem18.utf16(), elem18.size() * 2);

    connect(_read_thread, &read_thread::interfaceConfigurationSuccess, this, [this](QByteArray block_data) {
        disconnect_read_thread_listeners();
        this->craneDlgUI->log->appendPlainText("Настройка крана прошла успешно.");
    });
    connect(_read_thread, &read_thread::interfaceConfigurationFailed, this, [this](QString error) {
        disconnect_read_thread_listeners();
        this->craneDlgUI->log->appendPlainText(QString("Ошибка настройки крана: %1").arg(error));
        this->craneDlgUI->log->appendPlainText(QString("Настройка прервана."));
    });

    QByteArray data;
    data.append(reinterpret_cast<char*>(&crn), sizeof(crn));
    GeoStreamPacketVirtual* gsp = new GeoStreamPacketVirtual;
    gsp->makeCraneInfo(data);
//    craneDlgUI->log->appendPlainText(gsp->getData().toHex(' '));
    _read_thread->virtual_write_data(gsp->getData());
    delete gsp;

    emit showStatus(tr("Настройка крана."));
}

void MainWindow::send_periphery() {
    disconnect_read_thread_listeners();
    qDebug() << "Отправка периферии.";
    peripheryUI->log->appendPlainText(QString("Отправка периферии."));
    if (!ifaces.size()) return;

    QList<peripherystruct> pss;

    QByteArray data;
    for (auto vp: ifaces.values()) {
        peripherystruct ps;
        ps.num_interface = vp.numberiCb->currentIndex();
        QString name = vp.synEdit->text();
        memset(&ps.name, 0, sizeof(ps.name));
        memcpy(&ps.name, name.toStdString().c_str(), name.size());
        int log = 0;
        if (vp.logChb->isChecked()) log = 1;
        ps.settings = log << 2;
        ps.id = vp.canidEdit->text().toLongLong(nullptr, 16);
        ps.ratio = vp.ratioEdit->value();
        ps.start = vp.bitEdit->value();
        ps.size = vp.sizeEdit->value();
        ps.filter = vp.filterCb->currentIndex();
        QString tag = QString("#tag=%1#").arg(ps.filter);
        QString desc = vp.descEdit->text();
        memset(&ps.description, 0, sizeof(ps.description));
        memcpy(&ps.description, desc.utf16(), desc.size() * 2);
        QByteArray buf;
        quint16 len = ps.STRUCT_SIZE + tag.size() + 2;
        buf.reserve(len);
        buf.insert(0, reinterpret_cast<char*>(&len), 2);
        buf.append(reinterpret_cast<char*>(&ps), ps.STRUCT_SIZE);
        buf.append(tag.toStdString().data(), tag.size());
        data.append(buf);
        pss.append(ps);
    }

    connect(_read_thread, &read_thread::interfaceConfigurationSuccess, this, [this, pss](QByteArray block_data) {
        disconnect_read_thread_listeners();
        qDebug() << QString("Настройка завершена.");
        peripheryUI->log->appendPlainText(QString("Настройка завершена."));

        QSettings settings("ui.conf",QSettings::IniFormat,this);
        settings.remove("items");
        settings.beginWriteArray("items");
        for(int i = 0; i < pss.size(); i++) {
            auto ps = pss[i];
            settings.setArrayIndex(i);
            settings.setValue("element", ps.toHex());
        }
        settings.endArray();
    });
    connect(_read_thread, &read_thread::interfaceConfigurationFailed, this, [this](QString error) {
        disconnect_read_thread_listeners();
        qDebug() << QString("Ошибка настройки периферии: %1").arg(error);
        qDebug() << QString("Настройка прервана.");
        peripheryUI->log->appendPlainText(QString("Ошибка настройки периферии: %1").arg(error));
        peripheryUI->log->appendPlainText(QString("Настройка прервана."));
    });
    GeoStreamPacketVirtual* gsp = new GeoStreamPacketVirtual;
    peripheryUI->log->appendPlainText(data.toHex(' '));
    gsp->makeCfgPrph(data);
    _read_thread->virtual_write_data(gsp->getData());
    delete gsp;
}

void MainWindow::can1_send_interface_settings()
{
    disconnect_read_thread_listeners();

    qDebug() << "CAN1 send interface settings";

    can1SettingsUI->log->appendPlainText(QString("Отправка настроек интерфейса CAN."));

    int can_config_flag = 0;
    if (can1SettingsUI->canAddOptions->isChecked()) can_config_flag = 1;

    int can_filter = 0;
    if (can1SettingsUI->canFilter->isChecked()) can_filter = 1;

    int can_logger = 0;
    if (can1SettingsUI->canLogging->isChecked()) can_logger = 1;

    int interfaceNumber = 0;
    int mode = can1SettingsUI->canMode->currentIndex();
    int typeID = can1SettingsUI->canTypeID->currentIndex();
    int speed = can1SettingsUI->canSpeed->currentIndex();
    int protocol = can1SettingsUI->canProtocol->currentIndex();
    int logging = 4 | (can_logger << 1) | (can_config_flag << 3) | (can_filter << 4);

    QString typeIDStr = can1SettingsUI->canTypeID->currentText();
    QString speedStr = can1SettingsUI->canSpeed->currentText();

    connect(_read_thread, &read_thread::interfaceConfigurationSuccess, this, [this,typeIDStr,speedStr](QByteArray block_data)
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        QString title = QString("CAN 1; тип идентификатора: %1; скорость: %2").arg(typeIDStr).arg(speedStr);
        if (!this->w1)
        {
            this->can1_initial_win_title = title;
        }
        else
        {
            this->w1->setWindowTitle(title);
        }
        this->can1SettingsUI->log->appendPlainText(QString("Настройка интерфейсов CAN прошла успешно."));
        if (!this->can1SettingsUI->canAddOptions->isChecked())
        {
            this->can1SettingsUI->log->appendPlainText(QString("Настройка параметра CAN для записи не требуется."));
            this->can1SettingsUI->log->appendPlainText(QString("Настройка закончена."));
        }
        else
        {
            this->can1SettingsUI->log->appendPlainText(QString("Отправка настроек параметра CAN для записи."));
            this->can1_send_additional_settings();
        }
    });

    connect(_read_thread, &read_thread::interfaceConfigurationFailed, this, [this](QString error)
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        this->can1SettingsUI->log->appendPlainText(QString("Ошибка конфигурации интерфейсов: %1").arg(error));
        this->can1SettingsUI->log->appendPlainText(QString("Настройка прервана."));
    });

    GeoStreamPacketVirtual* gsp = new GeoStreamPacketVirtual;
    gsp->makeCfgReqOne(interfaceNumber,mode,typeID,speed,protocol,logging);
    _read_thread->virtual_write_data(gsp->getData());
    delete gsp;

    emit showStatus(tr("Конфигурация CAN 1"));
}

void MainWindow::can2_send_interface_settings()
{
    disconnect_read_thread_listeners();

    qDebug() << "CAN2 send interface settings";

    int can_config_flag = 0;
    if (can2SettingsUI->canAddOptions->isChecked()) can_config_flag = 1;

    int can_filter = 0;
    if (can2SettingsUI->canFilter->isChecked()) can_filter = 1;

    int can_logger = 0;
    if (can2SettingsUI->canLogging->isChecked()) can_logger = 1;

    int interfaceNumber = 1;
    int mode = can2SettingsUI->canMode->currentIndex();
    int typeID = can2SettingsUI->canTypeID->currentIndex();
    int speed = can2SettingsUI->canSpeed->currentIndex();
    int protocol = can2SettingsUI->canProtocol->currentIndex();
    int logging = 4 | (can_logger << 1) | (can_config_flag << 3) | (can_filter << 4);

    QString typeIDStr = can2SettingsUI->canTypeID->currentText();
    QString speedStr = can2SettingsUI->canSpeed->currentText();

    connect(_read_thread, &read_thread::interfaceConfigurationSuccess, this, [this,typeIDStr,speedStr](QByteArray block_data)
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        QString title = QString("CAN 2; тип идентификатора: %1; скорость: %2").arg(typeIDStr).arg(speedStr);
        if (!this->w2)
        {
            this->can2_initial_win_title = title;
        }
        else
        {
            this->w2->setWindowTitle(title);
        }

        this->can2SettingsUI->log->appendPlainText(QString("Настройка интерфейсов CAN прошла успешно."));
        if (!this->can2SettingsUI->canAddOptions->isChecked())
        {
            this->can2SettingsUI->log->appendPlainText(QString("Настройка параметра CAN для записи не требуется."));
            this->can2SettingsUI->log->appendPlainText(QString("Настройка закончена."));
        }
        else
        {
            this->can2SettingsUI->log->appendPlainText(QString("Отправка настроек параметра CAN для записи."));
            this->can2_send_additional_settings();
        }

    });

    connect(_read_thread, &read_thread::interfaceConfigurationFailed, this, [this](QString error)
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        this->can2SettingsUI->log->appendPlainText(QString("Ошибка конфигурации интерфейсов: %1").arg(error));
        this->can2SettingsUI->log->appendPlainText(QString("Настройка прервана."));
    });

    GeoStreamPacketVirtual* gsp = new GeoStreamPacketVirtual;
    gsp->makeCfgReqOne(interfaceNumber,mode,typeID,speed,protocol,logging);
    _read_thread->virtual_write_data(gsp->getData());
    delete gsp;

    emit showStatus(tr("Конфигурация CAN2"));
}

void MainWindow::can1_send_additional_settings()
{
    qDebug() << "CAN1 send additional settings";

    QString messageIDstr = can1SettingsUI->canIDHEX->text();
    messageIDstr.remove("0x");
    QByteArray messageID = QByteArray::fromHex(messageIDstr.toLocal8Bit());
    std::reverse(messageID.begin(), messageID.end()); // reverse byte order

    int condition = can1SettingsUI->canIDcondition->currentIndex() + 1;

    int compareWithInt = (can1SettingsUI->canIDvalue->text()).toInt();
    QByteArray compareWith;
    QDataStream stream(&compareWith, QIODevice::WriteOnly);
    stream << compareWithInt;
    std::reverse(compareWith.begin(), compareWith.end()); // reverse byte order

    QList<QModelIndex> selectedBytes = can1SettingsUI->startByteLength->selectionModel()->selectedIndexes();
    int length = selectedBytes.length()*8;

    int startBit = selectedBytes.at(0).row()*8;

    QByteArray block;
    block.append((char)0x01); // periphery setting
    block.append((char)0x0);

    block.append((char)0x20);
    block.append((char)0);    // interface number
    block.append(messageID);
    block.append((char)startBit);
    block.append((char)length);
    block.append((char)condition);
    block.append(compareWith);

    quint16 len0 = block.size()+2;
    block.insert(0,reinterpret_cast<char*>(&len0),2);

    connect(_read_thread, &read_thread::variableConfigurationSuccess, this, [this]()
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        can1SettingsUI->log->appendPlainText(QString("Настройка параметра CAN для записи прошла успешно."));
        can1SettingsUI->log->appendPlainText(QString("Настройка закончена."));
    });

    connect(_read_thread, &read_thread::variableConfigurationFailed, [this](QString error)
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        can1SettingsUI->log->appendPlainText(QString("Ошибка конфигурации параметра CAN: %1").arg(error));
        can1SettingsUI->log->appendPlainText(QString("Настройка прервана."));
    });

    GeoStreamPacketVirtual* g = new GeoStreamPacketVirtual;
    g->makeIDCfgReq(block);
    _read_thread->virtual_write_data(g->getData());

    delete g;
    emit showStatus(tr("Настройка параметров CAN 1"));
}

void MainWindow::can2_send_additional_settings()
{
    qDebug() << "CAN2 send additional settings";

    QString messageIDstr = can2SettingsUI->canIDHEX->text();
    messageIDstr.remove("0x");
    QByteArray messageID = QByteArray::fromHex(messageIDstr.toLocal8Bit());
    std::reverse(messageID.begin(), messageID.end()); // reverse byte order

    int condition = can2SettingsUI->canIDcondition->currentIndex() + 1;

    int compareWithInt = (can2SettingsUI->canIDvalue->text()).toInt();
    QByteArray compareWith;
    QDataStream stream(&compareWith, QIODevice::WriteOnly);
    stream << compareWithInt;
    std::reverse(compareWith.begin(), compareWith.end()); // reverse byte order

    QList<QModelIndex> selectedBytes = can2SettingsUI->startByteLength->selectionModel()->selectedIndexes();
    int length = selectedBytes.length()*8;

    int startBit = selectedBytes.at(0).row()*8;

    QByteArray block;
    block.append((char)0x01); // periphery setting
    block.append((char)0x0);

    block.append((char)0x20);
    block.append((char)1); // interface number
    block.append(messageID);
    block.append((char)startBit);
    block.append((char)length);
    block.append((char)condition);
    block.append(compareWith);

    quint16 len0 = block.size()+2;
    block.insert(0,reinterpret_cast<char*>(&len0),2);

    connect(_read_thread, &read_thread::variableConfigurationSuccess, this, [this]()
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        this->can2SettingsUI->log->appendPlainText(QString("Настройка параметра CAN для записи прошла успешно."));
        this->can2SettingsUI->log->appendPlainText(QString("Настройка закончена."));
    });

    connect(_read_thread, &read_thread::variableConfigurationFailed, this, [this](QString error)
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        this->can2SettingsUI->log->appendPlainText(QString("Ошибка конфигурации параметра CAN: %1").arg(error));
        this->can2SettingsUI->log->appendPlainText(QString("Настройка прервана."));
    });

    GeoStreamPacketVirtual* g = new GeoStreamPacketVirtual;
    g->makeIDCfgReq(block);
    _read_thread->virtual_write_data(g->getData());

    delete g;
    emit showStatus(tr("Настройка параметров CAN 2"));
}

void MainWindow::can1_transmit()
{
    qDebug() << "can 1 transmit";

    QList<Task> taskList;
    uint repeats = can1_send_optionsUI->repeats->text().toInt();

    for (int i=0; i<can1_packetsUI.length() ;i++ )
    {
        if(!can1_packetsUI[i]->sendIt->isChecked())
            continue;

        QByteArray data;
        data.append((char)0x20); // interface type (can type = 0x20
        data.append((char)0);    // interface number
        data.append((char)(sizeof(can_frm) + 5));
        data.append((char)0);

        QString idStr = can1_packetsUI[i]->frameID->text();
        idStr.remove("0x");

        can_frm fm;

        QByteArray id = QByteArray::fromHex(idStr.toLocal8Bit());
        std::reverse(id.begin(), id.end());

        fm.frm_id = *(reinterpret_cast<quint32*>(id.data()));
        qDebug() << fm.frm_id;

        fm.dlen = (char)(8-can1_packetsUI[i]->messageLength->currentIndex());

        if(can1_packetsUI[i]->frameType->currentIndex()==0)
        {
            fm.frm_id &= (quint32)(2047);
        }
        else
        {
            fm.frm_id &= (quint32)(~(7<<29));
            fm.frm_id |= (1<<31); // don't you dare remove this line! most significant bit tells device that frameID is EFF!
        }

        QString dataStr = can1_packetsUI[i]->message->text();
        dataStr.remove("0x");

        QByteArray d = QByteArray::fromHex(dataStr.toLocal8Bit());
        if(d.size() > 8)
        {
            d = d.mid(d.size()-8,8);
        }
        while(d.size() < 8)
            d.insert(0,(char)0);

        fm.data[0]=d[0];
        fm.data[1]=d[1];
        fm.data[2]=d[2];
        fm.data[3]=d[3];
        fm.data[4]=d[4];
        fm.data[5]=d[5];
        fm.data[6]=d[6];
        fm.data[7]=d[7];

        data.append(reinterpret_cast<char*>(&fm), sizeof(fm) );

        quint8 crc8_ = crc8(data.data(),data.size());
        data.append(crc8_);

        int delay= can1_packetsUI[i]->delay->text().toInt();
        int repeat = can1_packetsUI[i]->repeat->text().toInt();

        Task task{data,delay,repeat};
        taskList.append(task);
    }

    if (taskList.length()>0)
        emit stx1(taskList,repeats);
}

void MainWindow::can2_transmit()
{
    qDebug() << "can 2 transmit";

    QList<Task> taskList;
    uint repeats = can2_send_optionsUI->repeats->text().toInt();

    for (int i=0; i<can2_packetsUI.length() ;i++ )
    {
        if(!can2_packetsUI[i]->sendIt->isChecked())
            continue;

        QByteArray data;
        data.append((char)0x20); // interface type (can type = 0x20
        data.append((char)1);    // interface number
        data.append((char)(sizeof(can_frm) + 5) );
        data.append((char)0);

        QString idStr = can2_packetsUI[i]->frameID->text();
        idStr.remove("0x");

        can_frm fm;

        QByteArray id = QByteArray::fromHex(idStr.toLocal8Bit());
        std::reverse(id.begin(), id.end());

        fm.frm_id = * (reinterpret_cast<quint32*>(id.data()));

        fm.dlen = (char)(8-can2_packetsUI[i]->messageLength->currentIndex());

        if(can2_packetsUI[i]->frameType->currentIndex()==0)
        {
            fm.frm_id &= (quint32)(2047);
        }
        else
        {
            fm.frm_id &= (quint32)(~(7<<29));
            fm.frm_id |= (1<<31); // don't you dare remove this line! most significant bit tells device that frameID is EFF!
        }

        QString dataStr = can2_packetsUI[i]->message->text();
        dataStr.remove("0x");

        QByteArray d = QByteArray::fromHex(dataStr.toLocal8Bit());
        if(d.size() > 8)
        {
            d = d.mid(d.size()-8,8);
        }
        while(d.size() < 8)
            d.insert(0,(char)0);

        fm.data[0]=d[0];
        fm.data[1]=d[1];
        fm.data[2]=d[2];
        fm.data[3]=d[3];
        fm.data[4]=d[4];
        fm.data[5]=d[5];
        fm.data[6]=d[6];
        fm.data[7]=d[7];

        data.append(reinterpret_cast<char*>(&fm), sizeof(fm) );

        quint8 crc8_ = crc8(data.data(),data.size());
        data.append(crc8_);

        int delay= can2_packetsUI[i]->delay->text().toInt();
        int repeat = can2_packetsUI[i]->repeat->text().toInt();

        Task task{data,delay,repeat};
        taskList.append(task);
    }

    if (taskList.length()>0)
        emit stx2(taskList,repeats);
}

void MainWindow::transmitbn()
{
    qDebug() << "transmitbn";

    if(ui->tx_can0_check->isChecked())
    {
        qDebug() << "can0 tx";

        tx1_data.clear();

        tx1_data.append((char)0x20); // interface type (can type = 0x20
        tx1_data.append((char)0);    // interface number
        tx1_data.append((char)(sizeof(can_frm) + 5) );
        tx1_data.append((char)0);

        QString idStr = ui->tx_can0_frameid->text();
        idStr.remove("0x");

        can_frm fm;

        QByteArray id = QByteArray::fromHex(idStr.toLocal8Bit());
        std::reverse(id.begin(), id.end());

        fm.frm_id = * (reinterpret_cast<quint32*>(id.data()));

        fm.dlen = (char)(8-ui->tx_can0_len->currentIndex());

        if(ui->tx_can0_frametype->currentIndex()==0)
            fm.frm_id &= (quint32)(2047);
        else
            fm.frm_id &= (quint32)(~(7<<29));

        QString dataStr = ui->tx_can0_data->text();
        dataStr.remove("0x");

        QByteArray d = QByteArray::fromHex(dataStr.toLocal8Bit());
        if(d.size() > 8)
        {
            d = d.mid(d.size()-8,8);
        }
        while(d.size() < 8)
            d.insert(0,(char)0);

        fm.data[0]=d[0];
        fm.data[1]=d[1];
        fm.data[2]=d[2];
        fm.data[3]=d[3];
        fm.data[4]=d[4];
        fm.data[5]=d[5];
        fm.data[6]=d[6];
        fm.data[7]=d[7];

        tx1_data.append(reinterpret_cast<char*>(&fm), sizeof(fm) );
        qDebug() << tx1_data;

        quint8 crc8_ = crc8(tx1_data.data(),tx1_data.size());
        tx1_data.append(crc8_);

        tx1_timeout = ui->tx_can0_delay->text().toInt();

        //tx1.setSingleShot(true);

        tx1_nrem = ui->tx_can0_repeat->text().toInt();

        if(tx1_nrem == 0)
        {
            tx1_nrem = -1;
        }

        QList<Task> taskList;
        Task task{tx1_data,tx1_timeout,tx1_nrem};
        taskList.append(task);

        qDebug() << "Before send:" << tx1_data.toHex(' ');

//        emit stx1(taskList);
    }

    if(ui->tx_can1_check->isChecked())
    {

        qDebug() << "can1 tx";

        tx2_data.clear();
        ////////////////////////////////////////////////////////////

        tx2_data.append((char)0x20);
        tx2_data.append((char)0x1);
        tx2_data.append((char)(sizeof(can_frm) + 5) );
        tx2_data.append((char)0);

        QString idStr = ui->tx_can1_frameid->text();
        idStr.remove("0x");

        can_frm fm;

        QByteArray id = QByteArray::fromHex(idStr.toLocal8Bit());
        std::reverse(id.begin(), id.end());
        fm.frm_id = * (reinterpret_cast<quint32*>(id.data()) );


        fm.dlen = (char)(8-ui->tx_can1_len->currentIndex());

        if(ui->tx_can1_frametype->currentIndex()==0)
            fm.frm_id &= (quint32)(2047);
        else
            fm.frm_id &= (quint32)(~(7<<29));

        QString dataStr = ui->tx_can1_data->text();
        dataStr.remove("0x");

        QByteArray d = QByteArray::fromHex(dataStr.toLocal8Bit());
        if(d.size() > 8)
        {
            d = d.mid(d.size()-8,8);
        }
        while(d.size() < 8)
            d.insert(0,(char)0);

        fm.data[0]=d[0];
        fm.data[1]=d[1];
        fm.data[2]=d[2];
        fm.data[3]=d[3];
        fm.data[4]=d[4];
        fm.data[5]=d[5];
        fm.data[6]=d[6];
        fm.data[7]=d[7];

        tx2_data.append(reinterpret_cast<char*>(&fm), sizeof(fm) );

        ////////////////////////////////////////////////////////////

        quint8 crc8_ = crc8(tx2_data.data(),tx2_data.size());
        tx2_data.append(crc8_);


        tx2_timeout = ui->tx_can1_delay->text().toInt();
        tx2_nrem = ui->tx_can1_repeat->text().toInt();

        if(tx2_nrem == 0)
        {
            tx2_nrem = -1;
        }

//        emit stx2(tx2_data, tx2_nrem, tx2_timeout);
    }
}

void MainWindow::reboot()
{
    GeoStreamPacketVirtual* g = new GeoStreamPacketVirtual;
    g->makeWorkReq();
    _read_thread->virtual_write_data(g->getData());
}
