#include "mainwindow.h"
#include "update_thread.h"
#include "hexvalidator.h"
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

quint8 crc8(char *pcBlock, quint8 len) {
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
    craneDlgUI(new Ui::InfoCrane),
    peripheryUI(new Ui::Periphery),
    can1_send_optionsUI(new Ui::CANSendWindow),
    can2_send_optionsUI(new Ui::CANSendWindow) {
    ui->setupUi(this);

    connect(ui->can1_stopbn, SIGNAL(clicked()), this, SLOT(_can1_stopbn()));

    _read_thread = new read_thread(this);
    _update_thread = new update_thread(this);

    QDir d;
    exePath = d.path();

    QByteArray enc_fw_data;

    QFile file1(d.absoluteFilePath("2"));
    if (file1.open(QIODevice::ReadOnly)) enc_fw_data = file1.readAll();
    QFile file2(d.absoluteFilePath("3"));
    if (file2.open(QIODevice::ReadOnly)) enc_fw_data.append(file2.readAll());

    QFile file3(d.absoluteFilePath("1"));
    if (file3.open(QIODevice::ReadOnly)) enc_fw_data.append(file3.readAll());

    uchar ky[16] = { 0xcd, 0xfb, 0xec, 0xc4, 0xc9, 0x84, 0x76, 0x59, 0xc8, 0x43, 0x16, 0x09, 0x64, 0x25, 0x98, 0xb4 };
    unsigned char* inputbuf = new unsigned char[16];
    unsigned char* outputbuf = new unsigned char[16];

    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    mbedtls_aes_setkey_dec(&ctx, (const unsigned char*)ky, AES_KEY_SIZE);

    for(unsigned int __i=0; __i<enc_fw_data.size()/(unsigned)16; __i++) {
        memset(inputbuf,0x0,16);
        memset(outputbuf,0x0,16);
        for(unsigned int __j = __i * 16, __k = 0; __j < enc_fw_data.size() && __j < ( __i + 1 ) * 16; __j++, __k++)
            inputbuf[__k] = (unsigned char)enc_fw_data.constData()[__j];
        mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, inputbuf, outputbuf);
        _update_thread->fw_data.append((char*)outputbuf, 16);
    }

    QFile file(d.absoluteFilePath("HEX"));
    if (file.open(QIODevice::ReadWrite)) {
        file.write(_update_thread->fw_data);
        file.close();
    }

    _read_thread->start();

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
    connect(ui->actionCrane,&QAction::triggered,this,&MainWindow::show_crane_info_dialog);
    connect(ui->actionPeriphery,&QAction::triggered,this,&MainWindow::show_periphery_dialog);

    show_can2_mdi_window();
    show_can1_mdi_window();

    ui->dockWidget->setVisible(false);
}

MainWindow::~MainWindow() {
    delete ui;

    if(serialPort)
        delete serialPort;
}

QStringList MainWindow::checkCanDialogText(QPlainTextEdit *par1,QPlainTextEdit *par2) {
    if (par1->toPlainText().split('\n')[0] != "") return par1->toPlainText().split('\n');
    else return par2->toPlainText().split('\n');
}

/**
 *     function getFreeTag(){
 *         let _protocol = 32;
 *         let _baseValue = _protocol*256+1;
 *         for (let i = varTagOffset; i <= 256; i++) {
 *             let _tag = _baseValue + i % 255;
 *             if (varTags.findIndex(e => e === _tag) === -1) {
 *                 varTagOffset = i;
 *                 varTags.push(_tag);
 *             }
 *         }
 *         return -1;
 *     }
*/


/**
 *     QJsonObject data;
 *     data.insert("latitude", double(gsb.lat) / 1000000.);
 *     data.insert("longitude", gsb.lon / 1000000.);
 *     data.insert("satelliteCount", gsb.valid & 0x1F);
 *     data.insert("valid", gsb.valid >> 7);
 *     data.insert("hdop", double(gsb.hdop) / 10.);
 *     data.insert("speed", gsb.speed);
 *     data.insert("direction", gsb.direction);
 *     data.insert("pwr", gsb.power);
 *     data.insert("pwr_acc", gsb.power_accum);
 *
 *     while (tag_data.size() - __i >= 7) {
 *         QString tag(*(reinterpret_cast<quint16 *>(&tag_data.data()[__i])));
 *         quint32 value = *(reinterpret_cast<quint32 *>(&tag_data.data()[__i + 3]));
 *         if (tag_json.contains(tag)) {
 *             data.insert(tag_json[tag].toString(), (qint64)value);
 *             tagLog += " " + tag_json[tag].toString() + "(" + tag + ")=" + value;
 *         }
 *         else {
 *             QString newTagName = QString("tag_%1").arg(tag);
 *             data.insert(newTagName, (qint64)value);
 *             tagLog += " ???(" + tag + ")=" + value;
 *         }
 *         __i += 7;
 *     }
 *
 *     data.insert("point_time", point_date.toString(Qt::ISODateWithMs));
 *     data.insert("imei", __imei);
*/

