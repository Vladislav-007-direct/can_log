#include "mainwindow.h"
#include "geostreampacketvirtual.h"
#include "update_thread.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QDateTime>
#include <QTimeZone>

void MainWindow::load_crane_info() {
    disconnect_read_thread_listeners();
    qDebug() << "Crane get identification";
    craneDlgUI->log->appendPlainText(QString("Чтение настроек крана"));

    connect(_read_thread, &read_thread::interfaceConfigurationSuccess, this, [this](QByteArray _data) {
        disconnect_read_thread_listeners();
        this->craneDlgUI->log->appendPlainText("Чтение настроек крана прошла успешно.");

        craneinfostruct crn;
        memset(reinterpret_cast<void *>(&crn), 0, sizeof(craneinfostruct));
        memcpy(reinterpret_cast<void *>(&crn),
               reinterpret_cast<void *>(_data.data()),
               std::min(int(sizeof(craneinfostruct)), _data.size()));
        craneDlgUI->log->appendPlainText(_data.toHex(' '));

        craneDlgUI->elem0->setText(crn.elem0ToUTF16());
        craneDlgUI->elem1->setText(crn.elem1ToUTF16());
        craneDlgUI->elem2->setText(crn.elem2ToUTF16());
        craneDlgUI->elem3->setText(crn.elem3ToUTF16());
        craneDlgUI->elem4->setValue(crn.elem4);
        craneDlgUI->elem5->setValue(crn.elem5);
        craneDlgUI->elem6->setValue(crn.elem6);
        craneDlgUI->elem7->setText(crn.elem7ToUTF16());
        craneDlgUI->elem8->setText(crn.elem8ToUTF16());
        craneDlgUI->elem9->setText(crn.elem9ToUTF16());
        craneDlgUI->elem10->setDate(QDateTime::fromSecsSinceEpoch(crn.elem10).date());
        qDebug() << crn.elem10;
        craneDlgUI->elem11->setValue(crn.elem11);
        craneDlgUI->elem12->setText(crn.elem12ToUTF16());
        craneDlgUI->elem13->setText(crn.elem13ToUTF16());
        craneDlgUI->elem14->setText(crn.elem14ToUTF16());
        craneDlgUI->elem15->setText(crn.elem15ToUTF16());
        craneDlgUI->elem16->setValue(crn.elem16);
        craneDlgUI->elem17->setDate(QDateTime::fromSecsSinceEpoch(crn.elem17).date());
        qDebug() << crn.elem17;
        craneDlgUI->elem18->setText(crn.elem18ToUTF16());
        craneDlgUI->elem19->setText(crn.toStringTime(crn.elem19));
        craneDlgUI->elem20->setText(crn.toStringTime(crn.elem20));
        craneDlgUI->elem21->setValue(crn.elem21);
        craneDlgUI->elem22->setValue(crn.elem22);
        craneDlgUI->elem23->setValue(crn.elem23);
        craneDlgUI->elem24->setValue(crn.elem24);
        craneDlgUI->elem25->setValue(crn.elem25);
        craneDlgUI->elem26->setValue(crn.elem26);
        craneDlgUI->elem27->setValue(crn.elem27);
        craneDlgUI->elem28->setValue(crn.elem28);
        craneDlgUI->elem29->setValue(crn.elem29);
        craneDlgUI->elem30->setValue(crn.elem30);
        craneDlgUI->elem31->setValue(crn.elem31);
        craneDlgUI->elem32->setValue(crn.elem32);
    });
    connect(_read_thread, &read_thread::interfaceConfigurationFailed, this, [this](QString error) {
        disconnect_read_thread_listeners();
        this->craneDlgUI->log->appendPlainText(QString("Ошибка чтения крана: %1").arg(error));
        this->craneDlgUI->log->appendPlainText(QString("Чтение прервано."));
    });

    GeoStreamPacketVirtual* gsp = new GeoStreamPacketVirtual;
    gsp->makeIDInfo();
//    craneDlgUI->log->appendPlainText(gsp->getData().toHex(' '));
    _read_thread->virtual_write_data(gsp->getData());
    delete gsp;

    emit showStatus(tr("Чтение настроек крана."));
}

void MainWindow::load_periphery() {
    QSettings settings("ui.conf",QSettings::IniFormat,this);
    auto size = settings.beginReadArray("items");

    for (auto key: ifaces.keys()) {
        removeAin(key);
    }

    for(int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QByteArray buf = QByteArray::fromHex(settings.value("element").toString().toLocal8Bit());
        peripherystruct ps;
        memset(&ps, 0, ps.STRUCT_SIZE + 1);
        memset(&ps.description, 0, sizeof(ps.description));
        memcpy(reinterpret_cast<void *>(&ps),
               reinterpret_cast<void *>(buf.data()), buf.size());
        addAin();
        auto code = ifaces_cnt - 1;
        if (ifaces.contains(code)) {
            ifaces[code].numberiCb->setCurrentIndex(ps.num_interface);
            ifaces[code].synEdit->setText(ps.name);
            ifaces[code].logChb->setChecked(ps.settings & 4);
            ifaces[code].canidEdit->setText(QString().setNum(ps.id, 16));
            ifaces[code].ratioEdit->setValue(ps.ratio);
            ifaces[code].bitEdit->setValue(ps.start);
            ifaces[code].sizeEdit->setValue(ps.size);
            ifaces[code].filterCb->setCurrentIndex(ps.filter);
            ifaces[code].descEdit->setText(ps.elemToUTF16(ps.description, sizeof(ps.description)));
        }
    }
    settings.endArray();
    peripheryUI->log->appendPlainText(QString("Загружена периферия."));
}

void MainWindow::load_can1_send_settings_window_configuration()
{
    QString filename = QFileDialog::getOpenFileName(this,"Открыть файл...",QString(),QString("Файлы настройки (*.conf)"));

    if(filename.isEmpty())
        return;

    QSettings settings(filename,QSettings::IniFormat);
    int size = settings.beginReadArray("packets");
    if (size > PACKETAMOUNT)
        size = PACKETAMOUNT;

    for (int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        can1_packetsUI[i]->sendIt->         setChecked     (settings.value("sendIt",        false).toBool());
        can1_packetsUI[i]->frameID->        setText        (settings.value("frameID",       "00 00 00 00").toString());
        can1_packetsUI[i]->frameType->      setCurrentIndex(settings.value("frameType",     0).toUInt());
        can1_packetsUI[i]->messageLength->  setCurrentIndex(settings.value("messageLength", 0).toUInt());
        can1_packetsUI[i]->message->        setText        (settings.value("message",       "00 00 00 00 00 00 00 00").toString());
        can1_packetsUI[i]->delay->          setText        (settings.value("delay",         "0").toString());
        can1_packetsUI[i]->repeat->         setText        (settings.value("repeat",        "1").toString());
    }
    settings.endArray();
    settings.beginGroup("general");
    can1_send_optionsUI->repeats->setText(settings.value("repeats","1").toString());
    settings.endGroup();
}

void MainWindow::load_can2_send_settings_window_configuration()
{
    QString filename = QFileDialog::getOpenFileName(this,"Открыть файл...",QString(),QString("Файлы настройки (*.conf)"));

    if(filename.isEmpty())
        return;

    QSettings settings(filename,QSettings::IniFormat);
    int size = settings.beginReadArray("packets");
    if (size > PACKETAMOUNT)
        size = PACKETAMOUNT;

    for (int i=0; i<size; i++)
    {
        settings.setArrayIndex(i);
        can2_packetsUI[i]->sendIt->         setChecked     (settings.value("sendIt",        false).toBool());
        can2_packetsUI[i]->frameID->        setText        (settings.value("frameID",       "00 00 00 00").toString());
        can2_packetsUI[i]->frameType->      setCurrentIndex(settings.value("frameType",     0).toUInt());
        can2_packetsUI[i]->messageLength->  setCurrentIndex(settings.value("messageLength", 0).toUInt());
        can2_packetsUI[i]->message->        setText        (settings.value("message",       "00 00 00 00 00 00 00 00").toString());
        can2_packetsUI[i]->delay->          setText        (settings.value("delay",         "0").toString());
        can2_packetsUI[i]->repeat->         setText        (settings.value("repeat",        "1").toString());
    }
    settings.endArray();
    settings.beginGroup("general");
    can2_send_optionsUI->repeats->setText(settings.value("repeats","1").toString());
    settings.endGroup();
}

void MainWindow::fwinfo(QByteArray info_data)
{

    qDebug() << "mainwinow::fwinfo(QByteArray);";

    if (firstTimeConnection)
    {
        firstTimeConnection = false;

        if (_update_thread && _update_thread->checkNeedUpdate(info_data))
        {
            QDialog* d = new QDialog;
            qDebug() << "emit get_fw_info();";

            needUpdateUI->setupUi(d);

            bool proceedToUpdate = false;
            if(d->exec() == QDialog::Rejected)
            {
                d->hide();
                delete d;
            }
            else
            {
                proceedToUpdate = true;
            }

            if (proceedToUpdate)
                on_firmware_update_triggered();
        }
    }


    if (!firmwareUpdateWindowOpen)
        return;

    if(_update_thread && _update_thread->checkNeedUpdate(info_data))
    {
        qDebug() << "1";

        fwdui->updatebn->setEnabled(true);

        fwdui->dev_fw_info->setText(QString::fromLocal8Bit(info_data.toHex()));
        fwdui->have_fw_info->setText(QString::fromLocal8Bit(_update_thread->getHaveUpdateInfo().toHex()));
    }
    else
    {
        qDebug() << "2";

        fwdui->updatebn->setEnabled(false);
        fwdui->updatebn->setVisible(false);
        fwdui->closebn->setVisible(true);

        fwdui->dev_fw_info->setText(QString::fromLocal8Bit(info_data.toHex()));

        if(_update_thread)
            fwdui->have_fw_info->setText(QString::fromLocal8Bit(_update_thread->getHaveUpdateInfo().toHex()));
    }
}

void MainWindow::update_bn()
{
    if(updating)
        return;
    updating=1;
    qDebug() << "update_bn();";
    _read_thread->pause();
    qDebug() << "pause";
    qDebug() << "start";
    QThread *thread = new QThread();
    _update_thread->moveToThread(thread);
    connect(thread, SIGNAL(started()), _update_thread, SLOT(run()));
    thread->start();
    updating = 0;
}
