#ifndef GEOSTREAMPACKET_H
#define GEOSTREAMPACKET_H

#include <QObject>

#include "QtDebug"
#include <Crc/ModbusCrc.h>

#define MINPACKETSIZE 18
#define CRCSIZE 2



enum {
    DEVICE_TYPE_PC = 0,
    DEVICE_TYPE_BMG10,
    DEVICE_TYPE_BMG20,
    DEVICE_TYPE_DISPLAYBOARD25,
    DEVICE_TYPE_WATERING,
    DEVICE_TYPE_CAN_LOGGER,
    DEVICE_TYPE_BMN,
    DEVICE_TYPE_TRACKER,
    DEVICE_TYPE_SCANIA_TRACKER,
    DEVICE_TYPE_BMA,
    DEVICE_TYPE_SERVER_GEOCODE=238
};

enum {
    NOENC=0,
    AES=1,
    MD5,
    RSA_PUBKEY,
    RSA_AES,
    TOKEN
};

const quint8 RSA_PUB_KEY_SIZE=128;
const quint8 MD5_DGST_SIZE=16;

const quint8 START_SESSION_FLAG = 0x1;
const quint8 CONTINUE_SESSION_FLAG = 0x0;

const quint8 MY_DEV_TYPE    = DEVICE_TYPE_SERVER_GEOCODE;
//const quint8 PEER_DEV_TYPE  = DEVICE_TYPE_CAN_LOGGER;
/////////////////////////////////
const quint8  VER_PROTO      = 0x2;
//////////////////////////// /////
const quint16 FW_SEQ        = 0x00FF;
const quint16 CFG_SEQ       = 0xEADB;
const quint16 INFO_SEQ      = 0x0045;
const quint16 SRV_SEQ       = 0x0040; // + AUTH
const quint16 AUTH_SEQ      = 0x0040;
const quint16 DATA_SEQ      = 0x0030; // + AUTH

//
extern QList<quint16> geostream_allowed_sequences;
//

// INFO
/////////////////////////////////
const quint16 INFO_REQ      = 0xFF00;
const quint16 DEV_SCAN      = 0xFF01;
const quint16 INFO_AK       = 0x0000;
const quint16 DEV_SCAN_AK   = 0x0001;
const quint16 DEV_SCAN_REQ  = 0xFF01;
/////////////////////////////////

// DATA 
/////////////////////////////////
const quint16 DATA_REQ      = 0xFF00;
const quint16 DATA_AK       = 0x0000;
const quint16 DATA_ERR_AK   = 0x0100;
/////////////////////////////////

// AUTH
//////////////////////////////////
const quint16 A0_REQ      = 0xFFA0;
const quint16 A0_AK       = 0x00A0;

const quint16 A1_REQ      = 0xFFA1;
const quint16 A1_AK       = 0x00A1;

const quint16 A2_REQ      = 0xFFA2;
const quint16 A2_AK       = 0x00A2;

const quint16 A3_REQ      = 0xFFA3;
const quint16 A3_AK       = 0x00A3;

const quint16 A4_REQ      = 0xFFA4;
const quint16 A4_AK       = 0x00A4;

///////////////////////////////////

// B1
/////////////////////////////////
const quint16 BOOT_REQ      = 0xFFB1;
const quint16 BOOT_AK       = 0x00B1;
/////////////////////////////////
// B2
/////////////////////////////////
const quint16 WORK_REQ      = 0xFFB2;
const quint16 WORK_AK       = 0x00B2;
/////////////////////////////////


const quint16 B3_REQ      = 0xFFB3;
const quint16 B3_AK       = 0x00B3;

const quint16 B2_REQ      = 0xFFB2;
const quint16 B2_AK       = 0x00B2;

const quint16 B1_REQ      = 0xFFB1;
const quint16 B1_AK       = 0x00B1;

// FW
/////////////////////////////////
const quint16 FW_UP       = 0xFF00;
const quint16 FW_AK       = 0x0000;
/////////////////////////////////


const quint16 C1_REQ      = 0xFFC1;
const quint16 C2_REQ      = 0xFFC2;
const quint16 C1_AK      = 0x00C1;
const quint16 C2_AK      = 0x00C2;


const quint16 CALIBRATE_REQ      = 0xFFEE;
const quint16 CALIBRATE_AK       = 0x00EE;


const quint16 TRP_REQ   = 0xFFD1;
const quint16 TRP_AK   = 0x00D1;


const quint16 TRP_EXIT_REQ = 0x2B2B;
const quint16 TRP_EXIT   = 0x2B2B;



#pragma pack(push, 1)
struct gsheader
{
    gsheader() { memset(this,0,sizeof(gsheader));}
    quint16 startseq;
    quint16 argseq;

    qint16 nsessions;
    qint16 parm;

    quint8 device_type;
    quint8 session_flag;

    quint32 full_length;

    quint8 version_proto;
    quint8 encr_type;
};

struct gsgsmheader
{
    gsgsmheader() { memset(this,0,sizeof(gsgsmheader));}
    quint16 startseq;
    quint16 argseq;

    qint16 nsessions;
    qint16 parm;

    quint8 device_type;
    quint8 session_flag;

    quint32 full_length;

    quint8 version_proto;
    quint8 encr_type;

    char imei[15];
};

#pragma pack(pop)

struct encr_key
{
    QByteArray data;
};

/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////

class GeoStreamPacket
{

public:

    GeoStreamPacket();
    virtual ~GeoStreamPacket();


    QByteArray getDataBlock() { return data.mid(sizeof(gsheader), data.size()-sizeof(gsheader)-2); }
    QByteArray& getData() { return data; }

    virtual bool checkLengthAndCRC();
    virtual void closePacket();
    virtual void startPk(quint16 startseq, quint16 argseq, gsheader* gsh=nullptr); // generate empty header
    virtual void clear();

protected:

    QByteArray data;

    bool packetClosed = false;
};

#endif // GEOSTREAMPACKET_H
