#pragma once

#include <QMainWindow>
#include <QMdiSubWindow>
#include <QWindow>

#include <QSerialPort>
#include <QMutex>
#include <QGroupBox>
#include <QRegularExpression>
#include <QValidator>
#include <QLabel>
#include <QSettings>
#include <QToolButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "read_thread.h"
#include "structs.h"
#include "graph.h"

#include "ui_mainwindow.h"
#include "ui_dialog.h"
#include "ui_convert.h"
#include "ui_cansetting.h"
#include "ui_cansendwidget.h"
#include "ui_packetRow.h"
#include "ui_mdisubwindow.h"
#include "ui_needUpdate.h"
#include "ui_infoCrane.h"
#include "ui_periphery.h"
#include "ui_peripheryRow.h"

#define AES_KEY_SIZE 128
#define PACKETAMOUNT 40
#define MAXNUMAINS 50

class update_thread;
namespace Ui
{
    class MainWindow;
    class FwDialog;
    class ConvertForm;
    class CANSetting;
    class CANSendWindow;
    class PacketRow;
    class MdiSubWindow;
    class NeedUpdate;
    class InfoCrane;
    class Periphery;
    class PeripheryRow;
}

extern QList<can_frm> export_data_can1;
extern QList<can_frm> export_data_can2;

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

quint8 crc8(char *pcBlock, quint8 len);

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static QString create_table_row_string(
            int count,
            QByteArray frameID,
            uchar length,
            QByteArray data,
            qint64 timestamp,
            quint8 day,
            quint8 month,
            quint16 year,
            quint8 hours,
            quint8 minutes,
            quint8 seconds,
            quint16 milliseconds);
    static QString create_table_header_string();

    void disconnect_read_thread_listeners();
    void write_settings();
    //void virtual_write_data(QByteArray& data);

    void setDataToGraph(QVector<QString> a);
    static QStringList checkCanDialogText(QPlainTextEdit *par1,QPlainTextEdit *par2);
    static QVector<QString> parseToGraph(QStringList data, QString ID);
    QQueue<trblock> parseLogHelper(QByteArray &data);


private slots:

    /////////////////////////////

    void on_devconnect_triggered();
    void on_devdisconnect_triggered();
    void on_fileopen_triggered();
    void on_filesave_triggered();
    void on_quit_triggered();

    /////////////////////////////

    void on_time_sync_triggered();
    void on_firmware_update_triggered();
    void on_action_dec_txt_triggered();
    void on_action_dec_txt_can2_triggered();
    void on_action_dec_txt_can1_triggered();
    void on_action_dec_txt_can2_2_triggered();

    /////////////////////////////
    void convertToHex();
    void convertToASCII();

    void on_actionASC_triggered();

    void on_action_Hex_triggered();


    void graphCreate1();
    void graphCreate2();

public slots:
    void zoomAll(int zoom);
    virtual void closeEvent(QCloseEvent *event) override;

    void can1_saveLog();
    void can2_saveLog();
    void setDialogGraph();
    void createGraphObject();
    /////////////////////////////

    void reboot();

    void _can1_starterror();

    void can1_send_interface_settings();
    void can1_send_additional_settings();
    void can2_send_interface_settings();
    void can2_send_additional_settings();

    void show_can1_send_settings();
    void show_can2_send_settings();

    void load_can1_send_settings_window_configuration();
    void load_can2_send_settings_window_configuration();

    void save_can1_send_settings_window_configuration();
    void save_can2_send_settings_window_configuration();

    void show_can1_settings_dialog();
    void show_can2_settings_dialog();

    void show_can1_mdi_window();
    void show_can2_mdi_window();

    void show_periphery_dialog();
    void addAin();
    void removeAin(int index);
    void send_periphery();
    void load_periphery();

    void show_crane_info_dialog();
    void send_crane_info();
    void load_crane_info();

    void _can1_stopbn();

    void update_bn();

    void transmitbn();
    void can1_transmit();
    void can2_transmit();
    void canceltxbn();
    void can1_cancel();
    void can2_cancel();

    void can1_clear();
    void can2_clear();

    void parseLog(QByteArray data, int silent=0);
    void fwinfo(QByteArray info_data);

    void showStatus(QString);

    void greenStatus();
    void redStatus();

    void openFiles1();
    void openFiles2();

    void convert1();
    void convert2();

    /////////////////////////////

signals:
    void _can_endCnfg();

    void start_ifaces();

	void devcon();
	void devdiscon();

	void can_start();
	void can_stop();
	void can_cfg();
    void get_fw_info();

//    void stx1(QByteArray data, int ntx1, int delay);
    void stx1(QList<Task> taskList,uint bigCycleRepeats);
    void stx2(QList<Task> taskList,uint bigCycleRepeats);
    void stx3(QByteArray data, int ntx3, int delay);
    void stx4(QByteArray data, int ntx4, int delay);

    void stoptx1();
    void stoptx2();
    void stoptx3();
    void stoptx4();

    void timesync();


private:

    Ui::MainWindow *ui=nullptr;
    Ui::FwDialog *fwdui=nullptr;
    Ui::ConvertForm *convert=nullptr;


    /////////////////////////////

    QString can1_initial_win_title = "CAN 1";
    QString can2_initial_win_title = "CAN 2";

    Ui::MdiSubWindow *can1_mdiUI = nullptr;
    Ui::MdiSubWindow *can2_mdiUI = nullptr;

    QMainWindow* w1 = nullptr;
    QMainWindow* w2 = nullptr;

    QMainWindow* test = nullptr;

    /////////////////////////////

    Ui::NeedUpdate *needUpdateUI = nullptr;
    QDialog *needUpdate = nullptr;

    /////////////////////////////

    Ui::CANSetting *can1SettingsUI = nullptr;
    Ui::CANSetting *can2SettingsUI = nullptr;

    QDialog* can1Settings = nullptr;
    QDialog* can2Settings = nullptr;

    QDialog *createGraphdlg = nullptr;//Graph
    QChart *chart;
    QChartView *chartView;

    QDialog *craneDlg = nullptr;//Crane
    QDialog *peripheryDlg = nullptr;//Crane

    Ui::InfoCrane *craneDlgUI = nullptr;
    Ui::Periphery *peripheryUI = nullptr;
    QMap <int, Ui::PeripheryRow> ifaces;
    int ifaces_lt = 0;
    int ifaces_cnt = 0;
    /////////////////////////////

    QMainWindow* can1_send_options = nullptr;
    QMainWindow* can2_send_options = nullptr;


    Ui::CANSendWindow* can1_send_optionsUI = nullptr;
    Ui::CANSendWindow* can2_send_optionsUI = nullptr;

    QList<Ui::PacketRow*> can1_packetsUI;
    QList<Ui::PacketRow*> can2_packetsUI;

    //////////////////////////////

    QString sPortName;
    Graph* graphic=nullptr;

    QSerialPort* serialPort = nullptr;
    QMutex serialPortMutex;

    friend class read_thread;
    friend class update_thread;

    read_thread* _read_thread = nullptr;
    update_thread* _update_thread = nullptr;

    QLabel* statusLb = nullptr;

    int updating=0;

    /////////////////////////////

    int tx1_count=0;
    int tx2_count=0;
    int tx3_count=0;
    int tx4_count=0;

    int rx1_count=0;
    int rx2_count=0;
    int rx3_count=0;
    int rx4_count=0;

    int tx1_nrem=0;
    int tx2_nrem=0;
    int tx3_nrem=0;
    int tx4_nrem=0;

    int tx1_timeout=0;
    int tx2_timeout=0;
    int tx3_timeout=0;
    int tx4_timeout=0;


    QByteArray tx1_data;
    QByteArray tx2_data;
    QByteArray tx3_data;
    QByteArray tx4_data;

    /////////////////////////////

    QString exePath;

    /////////////////////////////

    QStringList files1;
    QStringList files2;

    bool firstTimeConnection = true;
    bool firmwareUpdateWindowOpen = false;
};

