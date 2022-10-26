#pragma once



/////////////////////////////

#include <QTimer>
#include <QThread>
//#include "chai.h"

#include "geostreampacketauth.h"

#include "mainwindow.h"

/////////////////////////////


class MainWindow;


class update_thread : public QThread
{
Q_OBJECT

private:

    bool auth_complete = false;

    MainWindow* _mw = nullptr;
    ////////////////////////////////
    // current flow

    enum { NOOP=-1,

           INFO_EXCHANGE,
           BOOTLOADER_MODE_REQ,
           WORK_MODE_REQ,
           AUTH_EXCHANGE,
           FW_UPDATE
        };




    int current_operation=NOOP;


    enum { NOAUTH=-1,

           AUTH_A1,
           AUTH_A2,
           AUTH_A3,
           AUTH_A4
        };

    int current_authorization=NOAUTH;

    //
    ////////////////////////////////
    //

    QByteArray have_version_info; // ("\x0c\x06\x04\x04\x02\x00",6);
//    quint32 have_version_info;
    //
    ////////////////////////////////
    //
    QTimer* a1_timer=nullptr;
    QTimer* a2_timer=nullptr;
    //
    //
    ////////////////////////////////
    //

    const int fw_block_size=1792;

    int current_fw_up_piece=-1;
    int fw_up_npieces=-1;



    mbedtls_rsa_context peerrsa;

    GeoStreamPacketAuth a;

signals:

    void squit();

    void sprogress(int);
    void sprogressmin(int);
    void sprogressmax(int);

    void showStatus(QString);
    void virtual_write_data(QByteArray);

	void disconn();
	void conn();

public slots:


    /////////// io ops
	void read_data();
    //void virtual_write_data(QByteArray data);
    ///////////


    /////////// work flow
    void begin_info_exchange();
    void begin_bootloader_req();
    void begin_work_req();
    void begin_auth_exchange();
    void begin_fw_update();


    void a1_auth_step();
    void a2_auth_step();
    void a3_auth_step();
    void a4_auth_step();
    ///////////


    void fw_up_step();
    void fw_up_complete()
    {



        emit squit();



        qDebug() << "fw_up_complete();";

        terminate();

        qDebug() << "fw_up_complete()-------;";


        disconnect(_mw->serialPort, SIGNAL(readyRead()), this, SLOT(read_data()));
        disconnect(this,SIGNAL(squit()),this,SLOT(quit()));
        disconnect(this,SIGNAL(squit()),_mw->_read_thread,SLOT(play()));

        emit _mw->get_fw_info();
    }


    bool checkNeedUpdate(QByteArray fw_info) {  return(fw_info.size() == have_version_info.size() &&
                                                      fw_info != have_version_info); };

    QByteArray getHaveUpdateInfo() { return have_version_info; }

    /////////// control thread execution
   // void play();
   // void pause();
    ///////////

    void run() override;

public:

    update_thread(MainWindow * mw);

    void authComplete(GeoStreamPacketAuth& g)
    {
        auth_complete=true;

        peerrsa = g.getPeerRsa();

    }

    QByteArray encryptRSA(QByteArray data);

    QByteArray data;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
    QRandomGenerator rnd;
    QByteArray fw_data;
};



