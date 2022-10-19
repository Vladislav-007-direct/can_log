#include "update_thread.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "QtDebug"
#include <Crc/ModbusCrc.h>

#include <QTimer>
#include <Crc/ModbusCrc.h>

#include "geostreampacketvirtual.h"
#include "geostreampacketauth.h"

#include <QMutexLocker>
#include <QSerialPortInfo>

#include <QPlainTextEdit>
#include "ui_dialog.h"
#include <QResource>
#define MINGSPACKETSIZE 18

update_thread::update_thread(MainWindow * mw)

    : _mw(mw)
//    ,have_version_info("\x00\x28\x0b\x01\x03\x15\x02\x01",8)
    //Logger_0007031220
{
    QSettings settings("settings.conf",QSettings::IniFormat,this);
    settings.beginGroup("firmware");
    QString version = settings.value("version","\x00\x28\x0b\x1f\x02\x15\x02\x01").toString();
    qDebug() << "VERSION" << version;
    QByteArray versionArray = QByteArray::fromStdString(version.toStdString());
    qDebug() << "VERSION ARRAY" << versionArray.toHex(' ');
    have_version_info.append(versionArray);
    rnd = QRandomGenerator::securelySeeded();

    mbedtls_rsa_init( &peerrsa, MBEDTLS_RSA_PKCS_V15, 0 );

    //mbedtls_entropy_context entropy;

    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_init( &ctr_drbg );


    const char *pers = "some string ohdhoinoivblvbalkgh;hg;lqtpuy23-8567";

    int ret=0;
    if( (ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                    reinterpret_cast<const unsigned char *>(pers),
                    strlen( pers ) ) ) != 0 )
    {
        qDebug() << "error mbedtls_ctr_drbg_seed";
        // error
    }

    ////connect(_mw->serialPort, SIGNAL(readyRead()), this, SLOT(read_data()));
    //connect(&read_timer, SIGNAL(timeout()), this, SLOT(read_data()));
    //read_timer.setInterval(1000);

}

void update_thread::read_data()
{


    qDebug() << "read_data update_thread()";

    a.clear();


    //if (_mw->serialPort && _mw->serialPort->error() != QSerialPort::NoError)
    //{
    //    return;
    //}
    /*
    if (_mw->serialPort && _mw->serialPort->error() != QSerialPort::NoError)
    {
        _mw->serialPort->clearError();
        _mw->statusBar()->showMessage("Ошибка на порту");
        _mw->serialPort->close();
        for(int i=0;i<10;i++)
        {
            qDebug()<< "trying to reopen port";
            _mw->statusBar()->showMessage("Попытка восстановить соединение с портом");
            QThread::msleep(2000);

            if(_mw->serialPort->open(QIODevice::ReadWrite))
            {
                _mw->statusBar()->showMessage("Порт открыт");
                break;
            }
        }
        return;
    }
    */

    if (_mw->serialPort && _mw->serialPort->isOpen())
    {
        if (_mw->serialPort->error() == QSerialPort::NoError)
            {
                data += _mw->serialPort->readAll();
                int parseLen = 0;

                quint32 len = 0;
                for (int i = 0; i + MINGSPACKETSIZE <= data.size(); )
                {
                    len = *(reinterpret_cast<quint16*>(&data.data()[i + 10]));
                    if (i + len > (uint)data.size() || len < MINGSPACKETSIZE)
                    {
                        i++;
                        parseLen++;
                        continue;
                    }

                    quint16 my_crc16 = CRC16((uint8_t*)(&data.data()[i]), len - 2);
                    quint16 _crc16 = *(reinterpret_cast<quint16*>(&data.data()[i + len - 2]));

                    if (my_crc16 != _crc16)
                    {
                        i++;
                        parseLen++;
                        continue;
                    }
                    parseLen += len;

                    QByteArray d;
                    d = data.mid(i + 16, len-MINGSPACKETSIZE);


                    qDebug() << "crc ok " << data.toHex(' ');

                    // read data and parse the work flow

                    {
                        GeoStreamPacketVirtual p0;
                        p0.initFromBufferData(data);


                        int errorCode = 0;
                        if(p0.checkReturnErrs(errorCode))
                        {
                            emit showStatus(QString("Ошибка. Код ошибки: ")+QString::number(errorCode));

                            data.clear();
                            return;
                        }
                    }
//                    emit showStatus("Пришел ответ какой-то ответ");

                    switch(current_operation)
                    {

                    case NOOP:
                    {
                        emit showStatus("Этап: операция не требуется");
                        break;
                    }
                    case INFO_EXCHANGE:
                    {
                        emit showStatus("Этап: обмен информацией");
                        GeoStreamPacketVirtual p;
                        p.initFromBufferData(data);

                        if(p.checkInfoAk())
                        {
                            emit showStatus("Получена информация об устройстве");

                            qDebug() << "p.checkInfoAk()";
                            int mode = p.readInfoAk();

                            qDebug() << mode;

                            if(mode == 0xb1)
                            {
                                emit showStatus("Режим b1");
                                qDebug() << "b1 mode";
                                 if(p.current_firmware_version != have_version_info)
                                 {
                                    emit showStatus("Прошивки не совпадают");
                                    emit showStatus("Начат процесс авторизации");
                                    begin_auth_exchange();
                                 }
                            }
                            if(mode == 0xb2)
                            {
                                emit showStatus("Режим b2");
                                qDebug() << "b2 mode";

                                emit showStatus("Отправка команды перехода в режим загрузчика");
                                begin_bootloader_req();
                            }
                        }

                        break;
                    }
                    case BOOTLOADER_MODE_REQ:
                    {

                        emit showStatus("Этап: переход в режим загручика");
                        GeoStreamPacketVirtual p;
                        p.initFromBufferData(data);
                        if(p.checkBootAk())
                        {

                            emit showStatus("Устройство успешно переведено в режим загрузчика");

                            qDebug() << "p.checkBootAk()";


							/*
                            _mw->serialPort->close();
                            for(int i=0;i<10;i++)
                            {
                                qDebug()<< "trying to reopen port";
                                emit showStatus("Попытка восстановить соединение с портом");
                                QThread::msleep(2000);

                                if(_mw->serialPort->open(QIODevice::ReadWrite))
                                {
                                    emit showStatus("Порт открыт");
                                    break;
                                }
                            }
							*/
							emit disconn();

							QThread::msleep(300);

							emit conn();


                            QThread::msleep(3000);


                            emit showStatus("Обмен информацией после перевода устройства в режим загрузчика");
                            begin_info_exchange();
                        }
                        break;
                    }
                    case WORK_MODE_REQ:
                    {
                        GeoStreamPacketVirtual p;
                        p.initFromBufferData(data);
                        if(p.checkWorkAk())
                        {
                            //_mw->serialPort->close();
							emit disconn();
							/*
                            for(int i=0;i<10;i++)
                            {
                                qDebug()<< "trying to reopen port";
                                emit showStatus("Попытка восстановить соединение с портом");
                                QThread::msleep(200);

                                if(_mw->serialPort->open(QIODevice::ReadWrite))
                                {
                                    emit showStatus("Порт открыт");
                                    emit showStatus("Обновление прошло успешно");
                                    break;
                                }
                            }
							*/

							QThread::msleep(300);

							emit conn();

                            QThread::msleep(3000);

                            emit showStatus(tr("Обновление прошло успешно"));

                            qDebug() << "p.checkWorkAk()";
                            fw_up_complete();


                            emit sprogress(fw_up_npieces+1);


                        }
                        break;
                    }
                    case AUTH_EXCHANGE:
                    {
                        emit showStatus("Этап обмен данными авторизации");

                        a.initFromBufferData(data);

                        switch(current_authorization)
                        {

                        case AUTH_A1:

                            emit showStatus("А1");

                            if(a.checkA1Ak())
                            {

                                qDebug() << "p.checkA1Ak()";

                                a1_timer->stop();
                                current_authorization = AUTH_A2;
                                a2_auth_step();
                            }

                            break;

                        case AUTH_A2:

                            emit showStatus("А2");

                            if(a.checkA2Ak() && a.checkA2AkValid())
                            {
                                qDebug() << "p.checkA2Ak()";

                                if(a.isA2Complete())
                                {
                                    qDebug() << "a2 complete";
                                    current_authorization = AUTH_A3;
                                    a3_auth_step();

                                    emit showStatus("А2 пройден");
                                }
                                else
                                {

                                    emit showStatus("А2 проверка");
                                    if(a.recvd_delta() > 0)
                                    {
                                        qDebug() << "a2 send ninterval=" <<
                                        a.recvd_delta()*GeoStreamPacketAuth::AUTH_TIME_STEP

                                                    +GeoStreamPacketAuth::AUTH_TIME_STEP
                                                    ;//-
                                        //  GeoStreamPacketAuth::VALID_AUTH_TIME_DELTA/2;


                                        connect(a2_timer, SIGNAL(timeout()), this, SLOT(a2_auth_step()) );
                                        a2_timer->setSingleShot(true);
                                        a2_timer->setInterval(
                                                    a.recvd_delta()*GeoStreamPacketAuth::AUTH_TIME_STEP
                                                    +GeoStreamPacketAuth::AUTH_TIME_STEP);//-
                                                      //GeoStreamPacketAuth::VALID_AUTH_TIME_DELTA/2);
                                        a2_timer->start();
                                    }
                                }
                            }
                            break;

                        case AUTH_A3:

                            emit showStatus("А3");
                            if(a.checkA3Ak() && a.checkA3AkValid())
                            {
                                emit showStatus("А3 пройден");

                                qDebug() << "a.checkA3Ak()";

                                current_authorization = AUTH_A4;
                                a4_auth_step();
                            }

                            break;

                        case AUTH_A4:

                            emit showStatus("А4");
                            if(a.checkA4Ak() && a.setA4Info())
                            {

                                emit showStatus("А4 пройден, переход к процессу обновления");
                                qDebug() << "a.checkA4Ak()";


                                authComplete(a);
                                begin_fw_update();
                            }
                            break;
                        }

                        break;
                    }
                    case FW_UPDATE:
                    {

                        emit showStatus("Этап: обновление прошивки");
                        GeoStreamPacketVirtual p;
                        p.initFromBufferData(data);

                        if(p.checkFwAk())
                        {
                            qDebug() << "p.checkFwAk()";


                            if(current_fw_up_piece > 0 && current_fw_up_piece == fw_up_npieces)
                            {

                                emit showStatus(tr("перезагрузка устройства"));
                                begin_work_req();

                                break;
                                //fw_up_complete();

                            }

                            fw_up_step();

                        }

                        break;
                    }

                    }
                    i += len;
                    data.remove(0, parseLen);
                }

        } else {
           showStatus("Ошибка порта");
           // begin_info_exchange();
        }
    }
}

void update_thread::begin_bootloader_req()
{
    current_operation = BOOTLOADER_MODE_REQ;

    GeoStreamPacketVirtual p;
    p.makeBootReq();
    emit virtual_write_data(p.getData());
}

void update_thread::begin_work_req()
{
    current_operation = WORK_MODE_REQ;

    GeoStreamPacketVirtual p;
    p.makeWorkReq();
    emit virtual_write_data(p.getData());
}


void update_thread::begin_info_exchange()
{
    emit showStatus(tr("предварительный обмен информацией"));
    current_operation = INFO_EXCHANGE;

    GeoStreamPacketVirtual p;
    p.makeInfoReq();
    emit virtual_write_data(p.getData());
}

void update_thread::a2_auth_step()
{

    qDebug() << "a2_auth_step() called";
    current_authorization = AUTH_A2;

    a.makeA2Req();
    emit virtual_write_data(a.getData());

    disconnect(a2_timer, SIGNAL(timeout()), this, SLOT(a2_auth_step()) );


}

void update_thread::a3_auth_step()
{

    current_authorization = AUTH_A3;

    a.makeA3Req();
    emit virtual_write_data(a.getData());


}

void update_thread::a4_auth_step()
{


    current_authorization = AUTH_A4;

    a.makeA4Req();
    emit virtual_write_data(a.getData());



}



void update_thread::a1_auth_step()
{


    a.makeA1Req();
    emit virtual_write_data(a.getData());

}


void update_thread::begin_auth_exchange()
{

    current_operation = AUTH_EXCHANGE;

    current_authorization = AUTH_A1;

    connect(a1_timer, SIGNAL(timeout()), this, SLOT(a1_auth_step()));

    a1_timer->setInterval(1000);
    a1_timer->start();



}
void update_thread::begin_fw_update()
{
    emit showStatus(tr("Производится обновление прошивки"));

    current_operation = FW_UPDATE;


    /*
    QFile file(QString("qrc:///Logger_02"));

    if(file.open(QIODevice::ReadOnly))
    {
        fw_data = file.readAll();        
        emit showStatus(QString("загружена прошивка %1 байт").arg(fw_data.size()));
        file.close();

    }else {
       // emit showStatus(file.errorString());

    }
    */

    qDebug() << "fw_data.size()=" << fw_data.size();
    qDebug() << "fw_data.size()=" << fw_data.size();
    qDebug() << "fw_data.size()=" << fw_data.size();
    qDebug() << "fw_data.size()=" << fw_data.size();
    qDebug() << "fw_data.size()=" << fw_data.size();
    qDebug() << "fw_data.size()=" << fw_data.size();
    qDebug() << "fw_data.size()=" << fw_data.size();

    fw_up_npieces = fw_data.size()/fw_block_size + (fw_data.size()%fw_block_size?1:0);
    current_fw_up_piece=0;

    emit sprogressmax(fw_up_npieces+1);
    emit sprogressmin(0);
    emit sprogress(1);

    fw_up_step();
}

QByteArray update_thread::encryptRSA(QByteArray data)
{
        QByteArray buffer;

        ////////////////////////////////////////////////////////////////////////////

        unsigned char buf[256];
        memset(buf,0,256);

        QByteArray d(data);
        //d.resize(128);


        qDebug() << d.toHex('+');


        int ret = 0;


        ret = mbedtls_rsa_pkcs1_encrypt( &peerrsa, mbedtls_ctr_drbg_random,
                                             &ctr_drbg, MBEDTLS_RSA_PUBLIC,
                                             16, (const unsigned char*)d.constData(), buf );
        if(ret != 0)
        {
          qDebug() << "error mbedtls_rsa_pkcs1_encrypt = " << ret;
          return buffer;
        }

        ////////////////////////////////////////////////////////////////////////////


        buffer = QByteArray(reinterpret_cast<char*>(buf), 128);
        return buffer;

}



void update_thread::fw_up_step()
{
    QByteArray data_;

    QByteArray d;
    d.reserve(fw_block_size);
    const char* s = &fw_data.constData()[current_fw_up_piece*fw_block_size];
    int len = (current_fw_up_piece+1)*fw_block_size-current_fw_up_piece*fw_block_size;

    if((current_fw_up_piece+1)*fw_block_size > fw_data.size())
        len = fw_data.size()-current_fw_up_piece*fw_block_size;

    d.append(s,len);
    while(d.size()%16)
        d.append(char(0));

    emit sprogress(current_fw_up_piece+1);

    qDebug() << d.size();
    qDebug() << d.toHex('*');

    // encrypt here

    QByteArray block(d);

    char ky[16];

    quint64 rnd1 = rnd.generate64();
    quint64 rnd2 = rnd.generate64();

    memcpy(ky,reinterpret_cast<void*>(&rnd1),8);
    memcpy(&ky[8],reinterpret_cast<void*>(&rnd2),8);

    // generate block.size() byte enccrypted buffer, multiple of 16

    QByteArray enc;
    enc.reserve(block.size());
    unsigned char* inputbuf = new unsigned char[16];
    unsigned char* outputbuf = new unsigned char[16];


    // encryption
    mbedtls_aes_context ctx;
    mbedtls_aes_init(&ctx);

    mbedtls_aes_setkey_enc(&ctx, (const unsigned char*)ky, RSA_PUB_KEY_SIZE);

    for(int __i=0; __i<block.size()/16; __i++)
    {
        memset(inputbuf,0x0,16);
        memset(outputbuf,0x0,16);
        for(int __j=__i*16,__k=0;__j<block.size() && __j<(__i+1)*16;__j++,__k++)
            inputbuf[__k] = (unsigned char)block.constData()[__j];

        mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, inputbuf, outputbuf);

        enc.append((char*)outputbuf, 16);
    }


    data_.reserve(2048);



    data_.append(enc);


    quint16 dlen = data_.size() + 2;
    data_.insert(0, reinterpret_cast<char*>(&dlen),2);



    {
      QByteArray d_(ky,16);
      QByteArray enckey = encryptRSA(d_);

      data_.insert(0, enckey ); // encrypted AES key


      qDebug() << "enckey=" << enckey.toHex('&');
    }





    /////////////////////////////////////////////////////////////////////////////////////







    GeoStreamPacketVirtual p;
    p.initFwBlock(data_, fw_up_npieces, current_fw_up_piece);
    p.makeFwUp();

    emit virtual_write_data(p.getData());

    current_fw_up_piece++;
}


/*
void update_thread::virtual_write_data(QByteArray data)
{
    qDebug() << data.toHex(' ');

    if(_mw->serialPort && _mw->serialPort->isOpen())
    {
        //QMutexLocker lock(&_mw->serialPortMutex);

        //if (_mw->serialPort && _mw->serialPort->error() != QSerialPort::NoError)
        //{
        //    _mw->serialPort->clearError();
        //}

        _mw->serialPort->write(data);
        _mw->serialPort->waitForBytesWritten(1000);

    }
    else
    {
        qDebug() << "port closed";
    }

}
*/


void update_thread::run()
{
    //read_timer.start();
    disconnect(_mw->serialPort, SIGNAL(readyRead()), this, SLOT(read_data()));
    connect(_mw->serialPort, SIGNAL(readyRead()), this, SLOT(read_data()));

    disconnect(this,SIGNAL(squit()),this,SLOT(quit()));
    connect(this,SIGNAL(squit()),this,SLOT(quit()));

    disconnect(this,SIGNAL(squit()),_mw->_read_thread,SLOT(play()));
    connect(this,SIGNAL(squit()),_mw->_read_thread,SLOT(play()));

    disconnect(this, SIGNAL(virtual_write_data(QByteArray)),
            _mw->_read_thread, SLOT(virtual_write_data(QByteArray)));
    connect(this, SIGNAL(virtual_write_data(QByteArray)),
            _mw->_read_thread, SLOT(virtual_write_data(QByteArray)));

	disconnect(this, SIGNAL(conn()),
		_mw->_read_thread, SLOT(devconn()));
	connect(this, SIGNAL(conn()),
		_mw->_read_thread, SLOT(devconn()));


	disconnect(this, SIGNAL(disconn()),
		_mw->_read_thread, SLOT(devdisconn()));
	connect(this, SIGNAL(disconn()),
		_mw->_read_thread, SLOT(devdisconn()));



    //void sprogress(int);
    //void sprogressmin(int);
    //void sprogressmax(int);


    connect(this,SIGNAL(sprogress(int)),_mw->fwdui->progressBar, SLOT(setValue(int)));
    connect(this,SIGNAL(sprogressmin(int)),_mw->fwdui->progressBar, SLOT(setMinimum(int)));
    connect(this,SIGNAL(sprogressmax(int)),_mw->fwdui->progressBar, SLOT(setMaximum(int)));

    connect(this, SIGNAL(showStatus(QString)),_mw, SLOT(showStatus(QString)));



    if(a1_timer)
        delete a1_timer;
    if(a2_timer)
        delete a2_timer;

    a1_timer=new QTimer;
    a2_timer=new QTimer;

    begin_info_exchange();


    qDebug() << "endless while";
	while (1)
		exec();
}


