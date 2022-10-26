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
#include "read_thread.h"
#include "ui_mainwindow.h"
#include "ui_dialog.h"
#include "ui_convert.h"
#include "ui_cansetting.h"
#include "ui_cansendwidget.h"
#include "ui_packetRow.h"
#include "ui_mdisubwindow.h"
#include "ui_needUpdate.h"
#include "qcustomplot.h"
#include "graph.h"
#include <QtCharts/QtCharts>
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
    class Graph;
}

namespace FwDialogUi
{
}

#pragma pack(push,1)
struct can_frm
{
    quint32 frm_id;
    quint8 dlen;
    qint8 data[8];
};

struct trblockheader
{
    quint8 interfaceType;
    quint8 interfaceNumber;
    quint64 timestamp;
    quint16 messageLength;
};

struct trblock
{
    trblockheader header;
    can_frm canFrame;
    quint8 crc;
};
#pragma pack(pop)

quint8 crc8(char *pcBlock, quint8 len);

class HexIntegerValidator : public QValidator
{
	Q_OBJECT
public:
    explicit HexIntegerValidator(QObject *parent, int maxlen);

	QValidator::State validate(QString &input, int &) const;

	void setMaximum(uint maximum);

private:
	uint m_maximum = 0;
};

class HexStringValidator : public QValidator
{
	Q_OBJECT

public:
    explicit HexStringValidator(QObject *parent, int maxlen);

	QValidator::State validate(QString &input, int &pos) const;

	void setMaxLength(int maxLength);

private:
	int m_maxLength = 0;
};


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
    static void zoomPlus();
    static void zoomMinus();
    //void virtual_write_data(QByteArray& data);

    static QStringList checkCanDialogText(QPlainTextEdit *par1,QPlainTextEdit *par2);
    static QVector<QString> parseToGraph(QStringList data, QString ID);
    static int hexToDec(QString b);
    void setDataToGraph(QVector<QString> a);

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
    void setDialogInf();
    void setDialogCrane();
    void on_actionInf_triggered();

    void on_actionCrane_triggered();

public slots:
    void zoomAll(int zoom);
    virtual void closeEvent(QCloseEvent *event) override;
    //virtual void wheelEvent(QWheelEvent *event);
    //virtual void keyPressEvent(QKeyEvent *event);
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
    QDialog *Infdlg=nullptr;//Info
    QDialog *Cranedlg=nullptr;//Crane
    //QDialog *Graph=nullptr;//Graph
    QChart *chart;
    QChartView *chartView;

    /////////////////////////////

    QMainWindow* can1_send_options = nullptr;
    QMainWindow* can2_send_options = nullptr;


    Ui::CANSendWindow* can1_send_optionsUI = nullptr;
    Ui::CANSendWindow* can2_send_optionsUI = nullptr;
    Graph* graphic=nullptr;

    QList<Ui::PacketRow*> can1_packetsUI;
    QList<Ui::PacketRow*> can2_packetsUI;

    //////////////////////////////

    QString sPortName;

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

