#include "mainwindow.h"
#include "geostreampacketvirtual.h"
#include <QDebug>

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
        can1SettingsUI->canFilter->setChecked(settings.value("filter",false).toBool());
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
        can2SettingsUI->canFilter->setChecked(settings.value("filter",false).toBool());
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

void MainWindow::show_crane_info_dialog()
{
    if (!craneDlg) {
        craneDlg = new QDialog(this);
        craneDlgUI->setupUi(craneDlg);

        connect(craneDlgUI->sendCrane, &QPushButton::clicked, this, &MainWindow::send_crane_info);
        connect(craneDlgUI->updateCrane, &QPushButton::clicked, this, &MainWindow::load_crane_info);
        connect(craneDlg, &QDialog::rejected, this, [this]() {
           qDebug() << "Crane closed";
           disconnect_read_thread_listeners();
        });
    }
    craneDlg->open();
    load_crane_info();
}

void MainWindow::show_periphery_dialog() {
    if (!peripheryDlg) {
        peripheryDlg = new QDialog(this);
        peripheryUI->setupUi(peripheryDlg);

        connect(peripheryUI->addElement, &QPushButton::clicked, this, &MainWindow::addAin);
        connect(peripheryUI->sendPeriphery, &QPushButton::clicked, this, &MainWindow::send_periphery);
        connect(peripheryUI->updatePeriphery, &QPushButton::clicked, this, &MainWindow::load_periphery);
        connect(peripheryDlg, &QDialog::rejected, this, [this]() {
           qDebug() << "Periphery closed";
           disconnect_read_thread_listeners();
        });
    }
    peripheryDlg->open();
    load_periphery();
}

void MainWindow::addAin() {
    if(ifaces.size() < MAXNUMAINS) {
        Ui::PeripheryRow pr;
        peripheryUI->ainsVLt->insertLayout(ifaces_lt, pr.setupUi(ifaces_cnt), 0);
        ifaces.insert(ifaces_cnt, pr);
        int ainlt = ifaces_cnt;
        auto ain = ifaces[ainlt];
        connect(ain.deleteBn, static_cast<void (QToolButton::*)(bool)>(&QToolButton::clicked),
                this, [this, ainlt](bool i) { removeAin(ainlt);});
        ifaces_lt++;
        ifaces_cnt++;
    }
}

void MainWindow::removeAin(int code) {
    if(ifaces.size() > 0) {
        auto vp = ifaces[code];
        vp.clear();
        auto index = ifaces.keys().indexOf(code);
        peripheryUI->ainsVLt->removeItem(peripheryUI->ainsVLt->itemAt(index));
        ifaces.remove(code);
        ifaces_lt--;
    }
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

void MainWindow::redStatus() {
    if (statusLb) {
        statusLb->setStyleSheet("QWidget{background: url(:/red.png); }");
    }
}

void MainWindow::greenStatus() {
    if (statusLb) {
        statusLb->setStyleSheet("QWidget{background: url(:/green.png); }");
    }

    if (firstTimeConnection)
        emit get_fw_info();
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qDebug() << "Приложение закрыто";
    write_settings();
    event->accept();
}
