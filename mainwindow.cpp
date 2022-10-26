#include "mainwindow.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QDataStream>
#include <QPlainTextEdit>
#include <QQueue>
#include <QMessageBox>
#include "geostreampacketvirtual.h"
#include "update_thread.h"
#include <QMutexLocker>
#include <QtCharts/QtCharts>
#include <graph.h>
#define AES_KEY_SIZE 128
#define PACKETAMOUNT 40

extern QList<can_frm> export_data_can1;
extern QList<can_frm> export_data_can2;
Graph *graphics;
/*
  Name  : CRC-8
  Poly  : 0x07    x^8 + x^5 + x^4 + 1
  Init  : 0x00
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
  MaxLen: 15 байт (127 бит) - обнаружение
    одинарных, двойных, тройных и всех нечетных ошибок
*/
const quint8 Crc8Table[256] =
{
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
    0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
    0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
    0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
    0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
    0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
    0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
    0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
    0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
    0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
    0xfa, 0xfd, 0xf4, 0xf3
};

quint8 crc8(char *pcBlock, quint8 len)
{
    quint8 crc = 0x00;
    quint8* pcb = reinterpret_cast<quint8*>(pcBlock);

    while (len--)
        crc = Crc8Table[crc ^ *pcb++];

    return crc;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fwdui(new Ui::FwDialog),
    convert(new Ui::ConvertForm),
    can1_mdiUI(new Ui::MdiSubWindow),
    can2_mdiUI(new Ui::MdiSubWindow),
    needUpdateUI(new Ui::NeedUpdate),
    can1SettingsUI(new Ui::CANSetting),
    can2SettingsUI(new Ui::CANSetting),
    can1_send_optionsUI(new Ui::CANSendWindow),
    can2_send_optionsUI(new Ui::CANSendWindow)
{
    ui->setupUi(this);

    connect(ui->can1_stopbn, SIGNAL(clicked()), this, SLOT(_can1_stopbn()));

    graphic=new Graph();

    _read_thread = new read_thread(this);
    _update_thread = new update_thread(this);

    /*
    QFile file(QString("qrc:///Logger_02"));
    if(file.open(QIODevice::ReadOnly))
    {
        _update_thread->fw_data = file.readAll();
        emit showStatus(QString("загружена прошивка %1 байт").arg(_update_thread->fw_data.size()));
        file.close();
    }
    */
    QDir d;
    exePath = d.path();

    QByteArray enc_fw_data;

//    QFile file1(d.absoluteFilePath("-1.bin"));
//    if(file1.open(QIODevice::ReadOnly))
//    {
//        enc_fw_data = file1.readAll();
//    }

    QFile file1(d.absoluteFilePath("2"));
    if(file1.open(QIODevice::ReadOnly))
    {
        enc_fw_data = file1.readAll();
    }
    QFile file2(d.absoluteFilePath("3"));
    if(file2.open(QIODevice::ReadOnly))
    {
        enc_fw_data.append(file2.readAll());
    }

    QFile file3(d.absoluteFilePath("1"));
    if(file3.open(QIODevice::ReadOnly))
    {
        enc_fw_data.append(file3.readAll());
    }

//    for(int i=0; i<16;i++)
//        enc_fw_data.append(char(0));

    uchar ky[16] =
    {
        0xcd,
        0xfb,
        0xec,
        0xc4,
        0xc9,
        0x84,
        0x76,
        0x59,
        0xc8,
        0x43,
        0x16,
        0x09,
        0x64,
        0x25,
        0x98,
        0xb4
    };
    unsigned char* inputbuf = new unsigned char[16];
    unsigned char* outputbuf = new unsigned char[16];

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    mbedtls_aes_setkey_dec(&ctx, (const unsigned char*)ky, AES_KEY_SIZE);

//    for(int __i=0; __i<enc_fw_data.size()/16; __i++)
//    {
//        memset(inputbuf,0x0,16);
//        memset(outputbuf,0x0,16);
//        for(int __j=__i*16,__k=0;__j<enc_fw_data.size() && __j<(__i+1)*16;__j++,__k++)
//            inputbuf[__k] = (unsigned char)enc_fw_data.constData()[__j];

//        mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, inputbuf, outputbuf);

//        _update_thread->fw_data.append((char*)outputbuf, 16);
//    }

    for(unsigned int __i=0; __i<enc_fw_data.size()/(unsigned)16; __i++)
    {
        memset(inputbuf,0x0,16);
        memset(outputbuf,0x0,16);
        for(unsigned int __j=__i*16,__k=0;__j<enc_fw_data.size() && __j<(__i+1)*16;__j++,__k++)
            inputbuf[__k] = (unsigned char)enc_fw_data.constData()[__j];

        mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, inputbuf, outputbuf);

        _update_thread->fw_data.append((char*)outputbuf, 16);
    }

    // Decrypted file for testing purposes
    QFile file(d.absoluteFilePath("HEX"));
    if (file.open(QIODevice::ReadWrite))
    {
        file.write(_update_thread->fw_data);
        file.close();
    }


    /*
    QFile file("4");
    if(file.open(QIODevice::ReadWrite))
    {
        file.write(_update_thread->fw_data);
        file.close();
    }
    */


    //QThread *thread = new QThread();

    _read_thread->start();
    //_read_thread->moveToThread(thread);
    //connect(thread, SIGNAL(started()), _read_thread, SLOT(run()));
    //thread->start();

    //_update_thread->start();
    connect(this, SIGNAL(devcon()), _read_thread, SLOT(devconnect()));
    connect(this, SIGNAL(devdiscon()), _read_thread, SLOT(devdisconnect()));

    connect(this, SIGNAL(can_start()), _read_thread, SLOT(can_startbn()));
    connect(ui->can1_startbn, SIGNAL(clicked()),  _read_thread, SLOT(can_startbn()));
    connect(this, SIGNAL(can_stop()), _read_thread, SLOT(can_stopbn()));

    connect(this, SIGNAL(get_fw_info()), _read_thread, SLOT(get_fw_info()));

    connect(this, &MainWindow::stx1,_read_thread, &read_thread::start_tx1);
    connect(this, &MainWindow::stx2,_read_thread, &read_thread::start_tx2);

    connect(this, SIGNAL(stoptx1()),_read_thread, SLOT(stop_tx1()));
    connect(this, SIGNAL(stoptx2()),_read_thread, SLOT(stop_tx2()));

    connect(this, SIGNAL(timesync()),_read_thread, SLOT(time_sync()));

    ui->tx_can0_frameid->setValidator(new HexStringValidator(nullptr,8));
    ui->tx_can0_data->setValidator(new HexStringValidator(nullptr, 16));
    ui->tx_can1_frameid->setValidator(new HexStringValidator(nullptr, 8));
    ui->tx_can1_data->setValidator(new HexStringValidator(nullptr,16));
    ui->statusBar->addPermanentWidget(new QLabel("Версия 1.4.3 | 08.09.2021"));
    ui->statusBar->addPermanentWidget(statusLb = new QLabel(" "));
    statusLb->setFixedSize(QSize(20,20));

    redStatus();

    QImage img(":/Group 9_.png");
    ui->mdi->setBackground(img);
    ui->mdi->setTabsMovable(true);
    ui->mdi->setViewMode(QMdiArea::SubWindowView);

    connect(ui->action_CAN_1,&QAction::triggered,this,&MainWindow::show_can1_settings_dialog);
    connect(ui->action_CAN_2,&QAction::triggered,this,&MainWindow::show_can2_settings_dialog);

    connect(ui->can1,&QPushButton::clicked,this,&MainWindow::show_can1_mdi_window);
    connect(ui->can2,&QPushButton::clicked,this,&MainWindow::show_can2_mdi_window);

    connect(ui->reboot,&QAction::triggered,this,&MainWindow::reboot);

//    connect(can2_mdiUI->saveLog,&QPushButton::clicked,this,[this](){
//        QDir d;
//        QString log = this->can2_mdiUI->log->toPlainText();
//        QFile file(d.absoluteFilePath(QString("can2_%1.txt").arg(QDateTime::currentSecsSinceEpoch())));
//        if (file.open(QIODevice::ReadWrite)){
//            file.write(log.toUtf8());
//            file.close();
//        }
//    });

    show_can2_mdi_window();
    show_can1_mdi_window();

    ui->dockWidget->setVisible(false);

/** Keep learning (C) **/
//    quint16 testNum = 16;
//    QByteArray testByteArray;
//    can_frm testFrame;
//    testFrame.frm_id = 0xAA;
//    testFrame.dlen = 8;
//    testFrame.data[0] = 0xBB;
//    testFrame.data[1] = 0x0;
//    testFrame.data[2] = 0x0;
//    testFrame.data[3] = 0x0;
//    testFrame.data[4] = 0x0;
//    testFrame.data[5] = 0x0;
//    testFrame.data[6] = 0x0;
//    testFrame.data[7] = 0x0;
//    qDebug() << sizeof(can_frm);
//    qDebug() << sizeof(testFrame);
//    testByteArray.append(reinterpret_cast<char*>(&testFrame), sizeof(testFrame));
//    qDebug() << testByteArray.toHex();

//    testByteArray.resize(2);
//    testByteArray[0] = (uchar)0x20;
//    testByteArray[1] = (uchar)0x40;
//    qDebug() << testByteArray.toHex();

//    QByteArray newByteArray = testByteArray.mid(0,2);
//    std::reverse(newByteArray.begin(), newByteArray.end());
//    qDebug() << newByteArray.toHex();
//    qDebug() << testByteArray.toHex();
//    newByteArray.insert(0,reinterpret_cast<char*>(&testNum),2);
//    qDebug() << newByteArray.toHex();
//    qDebug() << testByteArray.toHex();
//    QDataStream testStream(&testByteArray,QIODevice::ReadWrite);
//    testStream << testNum;
//    testStream << testNum;
//    qDebug() << testByteArray.toHex();
//    testStream >> testNum;
//    qDebug() << testNum;
/** Learning end **/
}

MainWindow::~MainWindow()
{
    delete ui;

    if(serialPort)
        delete serialPort;
}

void MainWindow::disconnect_read_thread_listeners()
{
    disconnect(this->_read_thread, &read_thread::variableConfigurationSuccess,nullptr,nullptr); // disconnect all listeners
    disconnect(this->_read_thread, &read_thread::variableConfigurationFailed,nullptr,nullptr); // disconnect all listeners
    disconnect(this->_read_thread, &read_thread::interfaceConfigurationSuccess,nullptr,nullptr); // disconnect all listeners
    disconnect(this->_read_thread, &read_thread::interfaceConfigurationFailed,nullptr,nullptr); // disconnect all listeners
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "Приложение закрыто";
    write_settings();
    event->accept();
}

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

void MainWindow::reboot()
{
    GeoStreamPacketVirtual* g = new GeoStreamPacketVirtual;
    g->makeWorkReq();
    _read_thread->virtual_write_data(g->getData());
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

void MainWindow::show_can1_settings_dialog()
{
    if (!can1Settings)
    {
        can1Settings = new QDialog(this);
        can1SettingsUI->setupUi(can1Settings);
        can1Settings->setWindowTitle(QString("Конфигурация CAN 1"));

        QSettings settings("ui.conf",QSettings::IniFormat,this);
        settings.beginGroup("can1.settings.dialog");
        uint startByte = settings.value("startByte",0).toUInt();
        uint length = settings.value("length",1).toUInt();
        can1SettingsUI->startByteLength->selectionModel()->select(
                    QItemSelection(can1SettingsUI->startByteLength->model()->index(startByte,0),
                                   can1SettingsUI->startByteLength->model()->index(startByte+length-1,0)),QItemSelectionModel::SelectCurrent);
        can1SettingsUI->canAddOptions->setChecked(settings.value("add.options",false).toBool()); // V checkboxes
        can1SettingsUI->canLogging->setChecked(settings.value("logging",false).toBool());
        can1SettingsUI->canIDHEX->setText(settings.value("id",QString("00 00 00 00")).toString()); // V text
        can1SettingsUI->canIDvalue->setText(settings.value("value",QString("0")).toString());
        can1SettingsUI->canIDcondition->setCurrentIndex(settings.value("condition",0).toUInt()); // V combo
        can1SettingsUI->canMode->setCurrentIndex(settings.value("mode",1).toUInt());
        can1SettingsUI->canProtocol->setCurrentIndex(settings.value("protocol",1).toUInt());
        can1SettingsUI->canSpeed->setCurrentIndex(settings.value("speed",0).toUInt());
        can1SettingsUI->canTypeID->setCurrentIndex(settings.value("typeID",0).toUInt());
        settings.endGroup();

        connect(can1SettingsUI->sendConfig,&QPushButton::clicked,this,&MainWindow::can1_send_interface_settings);
        connect(can1Settings,&QDialog::rejected,this,[this]()
        {
           qDebug() << "CAN 1 closed";
           disconnect_read_thread_listeners();
        });
    }
    can1Settings->open();
}

void MainWindow::show_can2_settings_dialog()
{
    if (!can2Settings)
    {
        can2Settings = new QDialog(this);
        can2SettingsUI->setupUi(can2Settings);
        can2Settings->setWindowTitle(QString("CAN2 Configuration"));

        QSettings settings("ui.conf",QSettings::IniFormat,this);
        settings.beginGroup("can2.settings.dialog");
        uint startByte = settings.value("startByte",0).toUInt();
        uint length = settings.value("length",1).toUInt();
        can2SettingsUI->startByteLength->selectionModel()->select(
                    QItemSelection(can2SettingsUI->startByteLength->model()->index(startByte,0),
                                   can2SettingsUI->startByteLength->model()->index(startByte+length-1,0)),QItemSelectionModel::SelectCurrent);
        can2SettingsUI->canAddOptions->setChecked(settings.value("add.options",false).toBool()); // V checkboxes
        can2SettingsUI->canLogging->setChecked(settings.value("logging",false).toBool());
        can2SettingsUI->canIDHEX->setText(settings.value("id",QString("00 00 00 00")).toString()); // V text
        can2SettingsUI->canIDvalue->setText(settings.value("value",QString("0")).toString());
        can2SettingsUI->canIDcondition->setCurrentIndex(settings.value("condition",0).toUInt()); // V combo
        can2SettingsUI->canMode->setCurrentIndex(settings.value("mode",1).toUInt());
        can2SettingsUI->canProtocol->setCurrentIndex(settings.value("protocol",1).toUInt());
        can2SettingsUI->canSpeed->setCurrentIndex(settings.value("speed",0).toUInt());
        can2SettingsUI->canTypeID->setCurrentIndex(settings.value("typeID",0).toUInt());
        settings.endGroup();

        connect(can2SettingsUI->sendConfig,&QPushButton::clicked,this,&MainWindow::can2_send_interface_settings);
        connect(can2Settings,&QDialog::rejected,this,[this]()
        {
           qDebug() << "CAN 2 closed";
           disconnect_read_thread_listeners();
        });
    }
    can2Settings->open();
}

void MainWindow::show_can1_mdi_window()
{
    if (!w1)
    {
        w1 = new QMainWindow(this);
        QWidget* _w1 = new QWidget(w1);
        can1_mdiUI->setupUi(_w1);
        can1_mdiUI->header->setText(MainWindow::create_table_header_string());
        w1->setCentralWidget(_w1);
        w1->setWindowTitle(can1_initial_win_title);
        w1->resize(800,600);
//        ui->mdi->addSubWindow(w1);

        connect(can1_mdiUI->clear,&QPushButton::clicked,this,&MainWindow::can1_clear);
        connect(can1_mdiUI->send,&QPushButton::clicked,this,&MainWindow::show_can1_send_settings);
        connect(can1_mdiUI->saveLog,&QPushButton::clicked,this,&MainWindow::can1_saveLog);
        connect(can1_mdiUI->pushButton,&QPushButton::clicked,this,&MainWindow::graphCreate1);
    }

    w1->show();
    w1->raise();
//    ui->mdi->tileSubWindows();
}

void MainWindow::show_can2_mdi_window()
{
    if (!w2)
    {
        w2 = new QMainWindow(this);
        QWidget* _w2 = new QWidget(w2);
        can2_mdiUI->setupUi(_w2);
        can2_mdiUI->header->setText(MainWindow::create_table_header_string());
        w2->setCentralWidget(_w2);
        w2->setWindowTitle(can2_initial_win_title);
        w2->resize(800,600);
//        ui->mdi->addSubWindow(w2);

        connect(can2_mdiUI->clear,&QPushButton::clicked,this,&MainWindow::can2_clear);
        connect(can2_mdiUI->send,&QPushButton::clicked,this,&MainWindow::show_can2_send_settings);
        connect(can2_mdiUI->saveLog,&QPushButton::clicked,this,&MainWindow::can2_saveLog);
        connect(can2_mdiUI->pushButton,&QPushButton::clicked,this,&MainWindow::graphCreate2);
    }
    w2->show();
    w2->raise();
}

void MainWindow::can1_send_interface_settings()
{
    disconnect_read_thread_listeners();

    qDebug() << "CAN1 send interface settings";

    can1SettingsUI->log->appendPlainText(QString("Отправка настроек интерфейса CAN."));

    int can_config_flag = 0;
    if (can1SettingsUI->canAddOptions->isChecked()) can_config_flag = 1;

    int interfaceNumber = 0;
    int mode = can1SettingsUI->canMode->currentIndex();
    int typeID = can1SettingsUI->canTypeID->currentIndex();
    int speed = can1SettingsUI->canSpeed->currentIndex();
    int protocol = can1SettingsUI->canProtocol->currentIndex();
    int logging = 4 | (can1SettingsUI->canLogging->isChecked() << 1) | (can_config_flag << 3);

    QString typeIDStr = can1SettingsUI->canTypeID->currentText();
    QString speedStr = can1SettingsUI->canSpeed->currentText();

    connect(_read_thread, &read_thread::interfaceConfigurationSuccess, this, [this,typeIDStr,speedStr]()
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

    connect(_read_thread, &read_thread::interfaceConfigurationFailed, this, [this]()
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        this->can1SettingsUI->log->appendPlainText(QString("Ошибка конфигурации интерфейсов."));
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

    int interfaceNumber = 1;
    int mode = can2SettingsUI->canMode->currentIndex();
    int typeID = can2SettingsUI->canTypeID->currentIndex();
    int speed = can2SettingsUI->canSpeed->currentIndex();
    int protocol = can2SettingsUI->canProtocol->currentIndex();
    int logging = 4 | (can2SettingsUI->canLogging->isChecked() << 1) | (can_config_flag << 3);

    QString typeIDStr = can2SettingsUI->canTypeID->currentText();
    QString speedStr = can2SettingsUI->canSpeed->currentText();

    connect(_read_thread, &read_thread::interfaceConfigurationSuccess, this, [this,typeIDStr,speedStr]()
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

    connect(_read_thread, &read_thread::interfaceConfigurationFailed, this, [this]()
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        this->can2SettingsUI->log->appendPlainText(QString("Ошибка конфигурации интерфейсов."));
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

    connect(_read_thread, &read_thread::variableConfigurationFailed, [this]()
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        can1SettingsUI->log->appendPlainText(QString("Ошибка конфигурации параметра CAN."));
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

    connect(_read_thread, &read_thread::variableConfigurationFailed, this, [this]()
    {
        disconnect_read_thread_listeners();
        // Configuration send success
        this->can2SettingsUI->log->appendPlainText(QString("Ошибка конфигурации параметра CAN."));
        this->can2SettingsUI->log->appendPlainText(QString("Настройка прервана."));
    });

    GeoStreamPacketVirtual* g = new GeoStreamPacketVirtual;
    g->makeIDCfgReq(block);
    _read_thread->virtual_write_data(g->getData());

    delete g;
    emit showStatus(tr("Настройка параметров CAN 2"));
}

void MainWindow::show_can1_send_settings(){
    if (!can1_send_options)
    {
        can1_send_options = new QMainWindow(this);
        can1_send_options->setWindowTitle("Отправка CAN 1");
        can1_send_options->resize(800,600);
        QWidget *can1_send_options_widget = new QWidget();
        can1_send_optionsUI->setupUi(can1_send_options_widget);
        can1_send_options->setCentralWidget(can1_send_options_widget);

        for (int i=0; i< PACKETAMOUNT; i++)
        {
            Ui::PacketRow *row = new Ui::PacketRow();
            can1_packetsUI.append(row);
            QWidget *w = new QWidget();
            row->setupUi(w);
            row->number->setText(QString::number(i));
            can1_send_optionsUI->sendWidgetList->layout()->addWidget(w);
        }

        connect(can1_send_optionsUI->send,&QPushButton::clicked,this,&MainWindow::can1_transmit);
        connect(can1_send_optionsUI->stop,&QPushButton::clicked,this,&MainWindow::can1_cancel);
        connect(can1_send_optionsUI->load,&QPushButton::clicked,this,&MainWindow::load_can1_send_settings_window_configuration);
        connect(can1_send_optionsUI->save,&QPushButton::clicked,this,&MainWindow::save_can1_send_settings_window_configuration);
    }
    can1_send_options->show();
}

void MainWindow::show_can2_send_settings()
{
    if (!can2_send_options)
    {
        can2_send_options = new QMainWindow(this);
        can2_send_options->setWindowTitle("Отправка CAN 2");
        can2_send_options->resize(800,600);
        QWidget *can2_send_options_widget = new QWidget();
        can2_send_optionsUI->setupUi(can2_send_options_widget);
        can2_send_options->setCentralWidget(can2_send_options_widget);

        for (int i=0; i< PACKETAMOUNT; i++)
        {
            Ui::PacketRow *row = new Ui::PacketRow();
            can2_packetsUI.append(row);
            QWidget *w = new QWidget();
            row->setupUi(w);
            row->number->setText(QString::number(i));
            can2_send_optionsUI->sendWidgetList->layout()->addWidget(w);
        }

        connect(can2_send_optionsUI->send,&QPushButton::clicked,this,&MainWindow::can2_transmit);
        connect(can2_send_optionsUI->stop,&QPushButton::clicked,this,&MainWindow::can2_cancel);
        connect(can2_send_optionsUI->load,&QPushButton::clicked,this,&MainWindow::load_can2_send_settings_window_configuration);
        connect(can2_send_optionsUI->save,&QPushButton::clicked,this,&MainWindow::save_can2_send_settings_window_configuration);
    }
    can2_send_options->show();
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

void MainWindow::can1_clear()
{
    rx1_count=0;
    tx1_count=0;

    export_data_can1.clear();

    if(w1) can1_mdiUI->log->clear();
}

void MainWindow::can2_clear()
{
    rx2_count=0;
    tx2_count=0;

    export_data_can2.clear();

    if(w2) can2_mdiUI->log->clear();
}

void MainWindow::can1_cancel()
{
    qDebug() << "canceltxbn";
    emit stoptx1();
    tx1_nrem = 0;
}

void MainWindow::can2_cancel()
{
    qDebug() << "canceltxbn";
    emit stoptx2();
    tx2_nrem = 0;
}

void MainWindow::showStatus(QString str)
{
    ui->statusBar->showMessage(str);
}

void MainWindow::redStatus()
{
    if(statusLb)
    {
        statusLb->setStyleSheet("QWidget{background: url(:/red.png); }");
    }
}

void MainWindow::greenStatus()
{
    if(statusLb)
    {
        statusLb->setStyleSheet("QWidget{background: url(:/green.png); }");
    }
    if (firstTimeConnection)
        emit get_fw_info();
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

QQueue<trblock> MainWindow::parseLogHelper(QByteArray &data)
{
    //can2_mdiUI->log->
    qDebug() << "Parse Log Helper";
    quint16 len = 0;

    QQueue<trblock> result;
    qDebug() << "TrBlock size: " << (int)sizeof(trblock);
    for(int i=0; i+(int)sizeof(trblock) <= data.size(); )
    {
//        trblock block = *(reinterpret_cast<trblock*>(&data.data()[i]));
        trblock block;
        memcpy(&block,&data.data()[i],sizeof(trblock));

        quint8 my_crc8 = crc8(&data.data()[i],block.header.messageLength-1);
        quint8 _crc8 = block.crc;

        if(my_crc8 != _crc8)
        {
            /*qDebug() << "my_crc8=" << QString::number(my_crc8) <<
                        " _crc8=" << QString::number(_crc8);*/
            i++;
            //return;
            continue;
        }
        //qDebug() << "crc8 match:" << i;

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

    //QVector<QVector<QString>>s=parseToGraph(red);
    //can_frm red1;



}
QStringList MainWindow::checkCanDialogText(QPlainTextEdit *par1,QPlainTextEdit *par2)
{
    if (par1->toPlainText().split('\n')[0]!="")
    {
        return par1->toPlainText().split('\n');
    }
    else
    {
        return par2->toPlainText().split('\n');
    }
}
QVector<QString>MainWindow::parseToGraph(QStringList data,QString ID)
{
    QVector<QString> s;
    for(int i=0;i<data.length();i++)
    {
        QVector<QString> vr;
        QStringList s1=data[i].split("     ");
        QStringList s2=s1[0].split(" ");
        QString q;
        for (int j=0;j<s2.length();j++)
        {
            if (s2[j].startsWith("0x")==true){q=s2[j].trimmed();break;}
        }
        if(q==ID){

            if(s.contains(s1[1])!=true){
                s.append(s1[1]);
            }
        }
    }
    return s;
}
int MainWindow::hexToDec(QString b){
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
}

void MainWindow::setDataToGraph(QVector<QString> a){
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

void MainWindow::setDialogGraph()
{
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
void MainWindow::createGraphObject(){

    graphic=new Graph(this);
    graphic->setMinimumWidth(1250);
    graphic->setMinimumHeight(800);
    graphic->setWindowTitle("График");
    graphic->setWindowFlags(graphic->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDialogButtonBox *btn_box = new QDialogButtonBox(Infdlg);
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
void Graph::wheelEvent(QWheelEvent *event){
        if(event->delta()>0){
            emit signalZoom(1);
        }
        else{
            emit signalZoom(2);
        }
}
void MainWindow::zoomAll(int type){
    if(type==1){
        chart->zoomIn();
    }
    else{
        chart->zoomOut();
    }
}
void MainWindow::setDialogInf()
{
    Infdlg=new QDialog(this);
    Infdlg->setMinimumWidth(250);
    Infdlg->setWindowTitle("Идентификационная информация");
    Infdlg->setWindowFlags(Infdlg->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDialogButtonBox *btn_box = new QDialogButtonBox(Infdlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, Infdlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, Infdlg, &QDialog::reject);
    QGridLayout *layout = new QGridLayout();
    QGridLayout *layout1 = new QGridLayout();
    QGridLayout *layout2 = new QGridLayout();
    QLabel *lb_cr1=new QLabel("Изготовитель крана:");
    QLineEdit *ed_cr1=new QLineEdit();
    QLabel *lb_cr2=new QLabel("ID крана:");
    QLineEdit *ed_cr2=new QLineEdit();
    QLabel *lb_cr3=new QLabel("Грузоподъемность крана:");
    QLineEdit *ed_cr3=new QLineEdit();
    QLabel *lb_cr4=new QLabel("Год изготовления крана:");
    QLineEdit *ed_cr4=new QLineEdit();
    QLabel *lb_cr5=new QLabel("Дата ввода в эксплуатацию:");
    QLineEdit *ed_cr5=new QLineEdit();
    QLabel *lb_cr6=new QLabel("Нормативный срок службы в 'лет':");
    QLineEdit *ed_cr6=new QLineEdit();
    layout1->addWidget(lb_cr1,0,0);
    layout1->addWidget(ed_cr1,0,1);
    layout1->addWidget(lb_cr2,1,0);
    layout1->addWidget(ed_cr2,1,1);
    layout1->addWidget(lb_cr3,2,0);
    layout1->addWidget(ed_cr3,2,1);
    layout1->addWidget(lb_cr4,3,0);
    layout1->addWidget(ed_cr4,3,1);
    layout1->addWidget(lb_cr5,4,0);
    layout1->addWidget(ed_cr5,4,1);
    layout1->addWidget(lb_cr6,5,0);
    layout1->addWidget(ed_cr6,5,1);
    QLabel *lb_cr1_u=new QLabel("Модификация устройства:");
    QLineEdit *ed_cr1_u=new QLineEdit();
    QLabel *lb_cr2_u=new QLabel("Версия ПО:");
    QLineEdit *ed_cr2_u=new QLineEdit();
    QLabel *lb_cr3_u=new QLabel("Производитель:");
    QLineEdit *ed_cr3_u=new QLineEdit();
    QLabel *lb_cr4_u=new QLabel("Серийный номер:");
    QLineEdit *ed_cr4_u=new QLineEdit();
    QLabel *lb_cr5_u=new QLabel("Год изготовления устройства:");
    QLineEdit *ed_cr5_u=new QLineEdit();
    QLabel *lb_cr6_u=new QLabel("Дата установки на экран:");
    QLineEdit *ed_cr6_u=new QLineEdit();
    QLabel *lb_cr7_u=new QLabel("Компания установщик:");
    QLineEdit *ed_cr7_u=new QLineEdit();
    layout2->addWidget(lb_cr1_u,0,0);
    layout2->addWidget(ed_cr1_u,0,1);
    layout2->addWidget(lb_cr2_u,1,0);
    layout2->addWidget(ed_cr2_u,1,1);
    layout2->addWidget(lb_cr3_u,2,0);
    layout2->addWidget(ed_cr3_u,2,1);
    layout2->addWidget(lb_cr4_u,3,0);
    layout2->addWidget(ed_cr4_u,3,1);
    layout2->addWidget(lb_cr5_u,4,0);
    layout2->addWidget(ed_cr5_u,4,1);
    layout2->addWidget(lb_cr6_u,5,0);
    layout2->addWidget(ed_cr6_u,5,1);
    layout2->addWidget(lb_cr7_u,6,0);
    layout2->addWidget(ed_cr7_u,6,1);
    QLabel *lb1_main=new QLabel("ИНФО ПО КРАНУ");
    QLabel *lb2_main=new QLabel("ИНФО ПО УСТРОЙСТВУ");
    layout->addWidget(lb1_main,0,0);
    layout->addWidget(lb2_main,0,1);
    layout->addLayout(layout1,1,0);
    layout->addLayout(layout2,1,1);
    layout->addWidget(btn_box,4,1);
    Infdlg->setLayout(layout);
    switch( Infdlg->exec()) {
       case QDialog::Accepted:
            qDebug() << "Accepted";//can1_mdiUI->log
             Infdlg->deleteLater();
           break;
       case QDialog::Rejected:
           qDebug() << "Rejected";
            Infdlg->deleteLater();
           break;
     }
}

void MainWindow::setDialogCrane()
{
    Cranedlg=new QDialog(this);
    Cranedlg->setMinimumWidth(250);
    Cranedlg->setWindowTitle("Информация о кране");
    Cranedlg->setWindowFlags(Cranedlg->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDialogButtonBox *btn_box = new QDialogButtonBox(Infdlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, Cranedlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, Cranedlg, &QDialog::reject);
    QGridLayout *layout = new QGridLayout();
    QGridLayout *layout1 = new QGridLayout();
    QGridLayout *layout2 = new QGridLayout();
    QLabel *lb_m1=new QLabel("Информация о РП и кране:");
    QLabel *lb_m2=new QLabel("Информация об эксплуатации крана:");
    QLabel *lb_cr1=new QLabel("Изготовитель крана:");
    QLineEdit *ed_cr1=new QLineEdit();
    QLabel *lb_cr2=new QLabel("ID крана:");
    QLineEdit *ed_cr2=new QLineEdit();
    QLabel *lb_cr3=new QLabel("Грузоподъемность крана:");
    QLineEdit *ed_cr3=new QLineEdit();
    QLabel *lb_cr4=new QLabel("Год изготовления крана:");
    QLineEdit *ed_cr4=new QLineEdit();
    QLabel *lb_cr5=new QLabel("Дата ввода в эксплуатацию:");
    QLineEdit *ed_cr5=new QLineEdit();
    QLabel *lb_cr6=new QLabel("Нормативный срок службы в 'лет':");
    QLineEdit *ed_cr6=new QLineEdit();
    QLabel *lb_cr7=new QLabel("Модификация устройства:");
    QLineEdit *ed_cr7=new QLineEdit();
    QLabel *lb_cr8=new QLabel("Версия ПО:");
    QLineEdit *ed_cr8=new QLineEdit();
    QLabel *lb_cr9=new QLabel("Производитель:");
    QLineEdit *ed_cr9=new QLineEdit();
    QLabel *lb_cr10=new QLabel("Серийный номер:");
    QLineEdit *ed_cr10=new QLineEdit();
    QLabel *lb_cr11=new QLabel("Год изготовления устройства:");
    QLineEdit *ed_cr11=new QLineEdit();
    QLabel *lb_cr12=new QLabel("Дата установки на экран:");
    QLineEdit *ed_cr12=new QLineEdit();
    QLabel *lb_cr13=new QLabel("Компания установщик:");
    QLineEdit *ed_cr13=new QLineEdit();
    QLabel *lb_cr14=new QLabel("Общее время работы устройства:");
    QLineEdit *ed_cr14=new QLineEdit();
    QLabel *lb_cr15=new QLabel("Год изготовления устройства:");
    QLineEdit *ed_cr15=new QLineEdit();
    QLabel *lb_cr16=new QLabel("Дата установки на экран:");
    QLineEdit *ed_cr16=new QLineEdit();
    QLabel *lb_cr17=new QLabel("Компания установщик:");
    QLineEdit *ed_cr17=new QLineEdit();
    layout1->addWidget(lb_m1,0,0);
    layout1->addWidget(lb_cr1,1,0);
    layout1->addWidget(ed_cr1,1,1);
    layout1->addWidget(lb_cr2,2,0);
    layout1->addWidget(ed_cr2,2,1);
    layout1->addWidget(lb_cr3,3,0);
    layout1->addWidget(ed_cr3,3,1);
    layout1->addWidget(lb_cr4,4,0);
    layout1->addWidget(ed_cr4,4,1);
    layout1->addWidget(lb_cr5,5,0);
    layout1->addWidget(ed_cr5,5,1);
    layout1->addWidget(lb_cr6,6,0);
    layout1->addWidget(ed_cr6,6,1);
    layout1->addWidget(lb_cr7,1,2);
    layout1->addWidget(ed_cr7,1,3);
    layout1->addWidget(lb_cr8,2,2);
    layout1->addWidget(ed_cr8,2,3);
    layout1->addWidget(lb_cr9,3,2);
    layout1->addWidget(ed_cr9,3,3);
    layout1->addWidget(lb_cr10,4,2);
    layout1->addWidget(ed_cr10,4,3);
    layout1->addWidget(lb_cr11,5,2);
    layout1->addWidget(ed_cr11,5,3);
    layout1->addWidget(lb_cr12,6,2);
    layout1->addWidget(ed_cr12,6,3);
    layout1->addWidget(lb_cr13,7,2);
    layout1->addWidget(ed_cr13,7,3);
    layout2->addWidget(lb_m2,0,0);
    layout2->addWidget(lb_cr14,1,0);
    layout2->addWidget(ed_cr14,1,1);
    layout2->addWidget(lb_cr15,2,0);
    layout2->addWidget(ed_cr15,2,1);
    layout2->addWidget(lb_cr16,3,0);
    layout2->addWidget(ed_cr16,3,1);
    layout2->addWidget(lb_cr17,4,0);
    layout2->addWidget(ed_cr17,4,1);
    layout->addLayout(layout1,0,0,0,2);
    layout->addLayout(layout2,0,3);
    layout->addWidget(btn_box,4,3);
    Cranedlg->setLayout(layout);
    switch( Cranedlg->exec()) {
       case QDialog::Accepted:
            qDebug() << "Accepted";//can1_mdiUI->log
             Cranedlg->deleteLater();
           break;
       case QDialog::Rejected:
           qDebug() << "Rejected";
            Cranedlg->deleteLater();
           break;
     }
}

void MainWindow::graphCreate1()
{

    if(can1_mdiUI->log->toPlainText().length()==0)
    {
        QMessageBox::information(this,"Внимание!","Логи не загружены!");
    }
    else
    {
        setDialogGraph();
    }
}
void MainWindow::graphCreate2()
{
    if(can2_mdiUI->log->toPlainText().length()==0)
    {
        QMessageBox::information(this,"Внимание!","Логи не загружены!");
    }
    else
    {
        setDialogGraph();
    }
}

void MainWindow::on_actionInf_triggered()
{
    setDialogInf();
}

void MainWindow::on_actionCrane_triggered()
{
    setDialogCrane();
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

void MainWindow::canceltxbn()
{

    qDebug() << "canceltxbn";

    emit stoptx1();
    emit stoptx2();
    emit stoptx3();
    emit stoptx4();
    tx1_nrem = 0;
    tx2_nrem = 0;
    tx3_nrem = 0;
    tx4_nrem = 0;
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
    ///_update_thread->start();
    qDebug() << "start";



    QThread *thread = new QThread();
    _update_thread->moveToThread(thread);
    connect(thread, SIGNAL(started()), _update_thread, SLOT(run()));
    thread->start();


    //thread->wait();
    //qDebug() << "wait";
    ////_read_thread->play();
    //qDebug() << "play";
    updating = 0;

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

HexIntegerValidator::HexIntegerValidator(QObject *parent, int maxlen) :
    QValidator(parent),
    m_maximum(maxlen)
{
}

QValidator::State HexIntegerValidator::validate(QString &input, int &) const
{
    bool ok;
    uint value = input.toUInt(&ok, 16);

    if (!value)
        return Intermediate;

    if (!ok || value > m_maximum)
        return Invalid;

    return Acceptable;
}

void HexIntegerValidator::setMaximum(uint maximum)
{
    m_maximum = maximum;
}

HexStringValidator::HexStringValidator(QObject *parent, int maxlen) :
    QValidator(parent),
    m_maxLength(maxlen)
{
}

QValidator::State HexStringValidator::validate(QString &input, int &pos) const
{
    const int maxSize = 2 * m_maxLength;
    const QChar space = QLatin1Char(' ');
    QString data = input;
    data.remove(space);

    if (data.isEmpty())
        return Intermediate;

    // limit maximum size and forbid trailing spaces
    if ((data.size() > maxSize) || (data.size() == maxSize && input.endsWith(space)))
        return Invalid;

    // check if all input is valid
    const QRegularExpression re(QStringLiteral("^[[:xdigit:]]*$"));
    if (!re.match(data).hasMatch())
        return Invalid;

    // insert a space after every two hex nibbles
    const QRegularExpression insertSpace(QStringLiteral("(?:[[:xdigit:]]{2} )*[[:xdigit:]]{3}"));
    if (insertSpace.match(input).hasMatch())
    {
        input.insert(input.size() - 1, space);
        pos = input.size();
    }

    return Acceptable;
}

void HexStringValidator::setMaxLength(int maxLength)
{
    m_maxLength = maxLength;
}

void MainWindow::on_actionASC_triggered()
{
    convertToASCII();
}

void MainWindow::on_action_Hex_triggered()
{
    convertToHex();
}
