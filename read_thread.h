#ifndef _READ_THREAD_H
#define _READ_THREAD_H


/////////////////////////////

#include <QTimer>
#include <QThread>
//#include "chai.h"
#include <QPlainTextEdit>
/////////////////////////////


class MainWindow;

struct Task {
    QByteArray block;
    qint32 delay;
    qint32 repeats;
};


class read_thread : public QThread
{
Q_OBJECT

private:

    MainWindow* _mw = nullptr;
    bool paused=false;
    ////////////////////////////////
    //
    //
    ////////////////////////////////
    //
    ////////////////////////////////

    QString parse_error(const int& core_error);

signals:

    void plog(QByteArray );

    void interfaceConfigurationSuccess(QByteArray block_data = nullptr);
    void interfaceConfigurationFailed(QString error = nullptr);
    void variableConfigurationSuccess();
    void variableConfigurationFailed(QString error = nullptr);

    void istarterror();
    void fwinfo(QByteArray);

    void showStatus(QString);

    void can_cfg_ready();

    void greenStatus();
    void redStatus();


public slots:

	void devconnect();
	void devdisconnect();

	void devconn();
	void devdisconn();


	void can_startbn();
    void can_stopbn();
    void get_fw_info();

	void read_data();
    void virtual_write_data(QByteArray data);
    //void run();


    void tx1_tm();
    void tx2_tm();
    void tx3_tm();
    void tx4_tm();

    void start_tx1(QList<Task> taskList, uint bigCycleRepeats);
    void start_tx2(QList<Task> taskList, uint bigCycleRepeats);
    void start_tx3(QByteArray data, int ntx3, int delay);
    void start_tx4(QByteArray data, int ntx4, int delay);

    void stop_tx1();
    void stop_tx2();
    void stop_tx3();
    void stop_tx4();

    void play();
    void pause();

    void error();

    void time_sync();

public:

    read_thread(MainWindow * mw);
    void run() override;
	QByteArray data;


    QTimer tx1;
    QTimer tx2;
    QTimer tx3;
    QTimer tx4;

    QByteArray tx1_data;
    QByteArray tx2_data;
    QByteArray tx3_data;
    QByteArray tx4_data;

    int tx1_nrem=-1;
    int tx2_nrem=-1;
    int tx3_nrem=-1;
    int tx4_nrem=-1;

    QList<Task> tx1_task_list;
    int tx1_current_task = 0;
    int tx1_current_task_repeat_count = 0;
    uint tx1_big_cycle_repeat_count = 0;

    QList<Task> tx2_task_list;
    int tx2_current_task = 0;
    int tx2_current_task_repeat_count = 0;
    uint tx2_big_cycle_repeat_count = 0;

    QPlainTextEdit* pt2 = nullptr;
    QPlainTextEdit* pt1 = nullptr;

    int time_syncing=0;

};



#endif // CAN_READ_THREAD_H
