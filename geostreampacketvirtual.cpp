
#include "geostreampacketvirtual.h"
#include "../geostreamlib/geostreampacket.h"

#include <QDateTime>
#include <QTimeZone>

GeoStreamPacketVirtual::GeoStreamPacketVirtual()
{

}

void GeoStreamPacketVirtual::makeCfgReqOne(int niface0, int mode0, int type_id0, int speed0, int protocol0, int logging0)
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=0xDB;
    d[1]=0xEA; // start sequence // service

    d[2]=0x02; ///// argument sequence
    d[3]=0xFF;

    d[4]=0; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; // num of seanses
    d[7]=0; // num of seanses

    d[8]=DEVICE_TYPE_PC; // dev type
    d[9]=0x01; // start frame

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;

    d[14] = 0x02; // version proto
    d[15] = 0; // encr type

    data.append(d,16);

    /////////////////////////////////
    QByteArray block0;
    block0.append((char)0x01); // periphery setting
    block0.append((char)0x0);

    block0.append((char)0x20);
    block0.append((char)niface0);
    block0.append((char)logging0);

    block0.append((char)mode0);
    block0.append((char)type_id0);
    block0.append((char)speed0);
    block0.append((char)protocol0);

      // id/mask of input filter
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);


    quint16 len0 = block0.size()+2;
      block0.insert(0,reinterpret_cast<char*>(&len0),2);

    data.append(block0);

    closePacket();
}

void GeoStreamPacketVirtual::makeCfgReq(
          int niface0, int mode0, int type_id0, int speed0, int protocol0, int logging0,
          int niface1, int mode1, int type_id1, int speed1, int protocol1, int logging1)
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=0xDB;
    d[1]=0xEA; // start sequence // service

    d[2]=0x02; ///// argument sequence
    d[3]=0xFF;

    d[4]=0; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; // num of seanses
    d[7]=0; // num of seanses

    d[8]=DEVICE_TYPE_PC; // dev type
    d[9]=0x01; // start frame

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;

    d[14] = 0x02; // version proto
    d[15] = 0; // encr type

    data.append(d,16);

    /////////////////////////////////
    QByteArray block0;
    block0.append((char)0x01); // periphery setting
    block0.append((char)0x0);

    block0.append((char)0x20);
    block0.append((char)niface0);
    block0.append((char)logging0);

    block0.append((char)mode0);
    block0.append((char)type_id0);
    block0.append((char)speed0);
    block0.append((char)protocol0);

      // id/mask of input filter
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);
    block0.append((char)0);


    quint16 len0 = block0.size()+2;
      block0.insert(0,reinterpret_cast<char*>(&len0),2);

    data.append(block0);
    /////////////////////////////////
    QByteArray block1;
    block1.append((char)0x01); // periphery setting
    block1.append((char)0x0);

    block1.append((char)0x20);
    block1.append((char)niface1);
    block1.append((char)logging1);

    block1.append((char)mode1);
    block1.append((char)type_id1);
    block1.append((char)speed1);
    block1.append((char)protocol1);


    // id/mask of input filter
    block1.append((char)0);
    block1.append((char)0);
    block1.append((char)0);
    block1.append((char)0);
    block1.append((char)0);
    block1.append((char)0);
    block1.append((char)0);
    block1.append((char)0);



    quint16 len1 = block1.size()+2;
      block1.insert(0,reinterpret_cast<char*>(&len1),2);

    data.append(block1);
    /////////////////////////////////




    closePacket();
}

void GeoStreamPacketVirtual::makeIDCfgReq(QByteArray _data)
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=0xDB;
    d[1]=0xEA; // start sequence // service

    d[2]=0x11; ///// argument sequence
    d[3]=0xFF;

    d[4]=0; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; // num of seanses
    d[7]=0; // num of seanses

    d[8]=DEVICE_TYPE_PC; // dev type
    d[9]=0x01; // start frame

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;

    d[14] = 0x02; // version proto
    d[15] = 0; // encr type

    data.append(d,16);
    /////////////////////////////////
//  QByteArray block0;
//  block0.append((char)0x01); // periphery setting
//  block0.append((char)0x0);

//  block0.append((char)0x20);
//  block0.append((char)niface);
//  block0.append(messageId);
//  block0.append((char)startBit);
//  block0.append((char)length);
//  block0.append((char)condition);
//  block0.append(compareWith);


//  quint16 len0 = block0.size()+2;
//  block0.insert(0,reinterpret_cast<char*>(&len0),2);

    data.append(_data);
//  qDebug() << data.toHex(' ');

    closePacket();
}

void GeoStreamPacketVirtual::makeStartReq()
{
      clear();

      char d[256];
      memset(d,0,256);

      d[0]=0x40;
      d[1]=0x00; // start sequence // service

      d[2]=0x10; ///// argument sequence
      d[3]=0xFF;

      d[4]=0; // num of seanses
      d[5]=0; // num of seanses
      d[6]=0; // num of seanses
      d[7]=0; // num of seanses

      d[8]=DEVICE_TYPE_PC; // dev type
      d[9]=0x01; // start frame

      // packet ln
      d[10] = 0;
      d[11] = 0;
      d[12] = 0;
      d[13] = 0;

      d[14] = 0x02; // version proto
      d[15] = 0; // encr type

      data.append(d,16);
      closePacket();
}
void GeoStreamPacketVirtual::makeStopReq()
{
    clear();
    char d[256];
    memset(d,0,256);
    d[0]=0x40;
    d[1]=0x00; // start sequence // service

    d[2]=0x11; ///// argument sequence
    d[3]=0xFF;

    d[4]=0; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; // num of seanses
    d[7]=0; // num of seanses

    d[8]=DEVICE_TYPE_PC; // dev type
    d[9]=0x01; // start frame

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;

    d[14] = 0x02; // version proto
    d[15] = 0; // encr type

    // skip 16 bytes
    data.append(d,16);
    closePacket();
}
bool GeoStreamPacketVirtual::checkCfgPk(int& haveError)
{
    if(data.size() < MINPACKETSIZE)
      return false;

    if(data[0] != (char)0xdb ||
       data[1] != (char)0xea ||
       data[2] != (char)0x02)
       return false;

    haveError = (char)data[3];
    return true;
}

bool GeoStreamPacketVirtual::checkCfgIDPk(int& haveError){
    if(data.size() < MINPACKETSIZE)
      return false;

    if(data[0] != (char)0xdb ||
       data[1] != (char)0xea ||
       data[2] != (char)0x11)
      return false;

    haveError = (char)data[3];
    return true;
}

bool GeoStreamPacketVirtual::checkStartPk(int& error)
{
      if(data.size() < MINPACKETSIZE)
          return false;

      error = (char)data[3];

      if(     data[0] != (char)0x40 ||
              data[1] != (char)0x00 ||
              data[2] != (char)0x10 )
          return false;

      return true;
}
void GeoStreamPacketVirtual::makeTxReq(QByteArray block)
{
      clear();
      char d[256];
      memset(d,0,256);

      d[0]=0x40;
      d[1]=0x00; // start sequence // service

      d[2]=0x0F; ///// argument sequence
      d[3]=0xFF;

      d[4]=0; // num of seanses
      d[5]=0; // num of seanses
      d[6]=0; // num of seanses
      d[7]=0; // num of seanses

      d[8]=DEVICE_TYPE_PC; // dev type
      d[9]=0x01; // start frame
      // packet ln
      d[10] = 0;
      d[11] = 0;
      d[12] = 0;
      d[13] = 0;
      /////////////////////////////
      d[14] = 0x02; // version proto
      d[15] = 0; // encr type
      // skip 16 bytes
      data.append(d,16);
      quint16 len = block.size()+2;
      block.insert(0,reinterpret_cast<char*>(&len),2);
      data.append(block);
      closePacket();
}
void GeoStreamPacketVirtual::makeTimeSyncReq()
{

    clear();


    char d[256];
    memset(d,0,256);

    d[0]=0xDB;
    d[1]=0xEA; // start sequence // service

    d[2]=0x02; ///// argument sequence
    d[3]=0xFF;

    d[4]=0; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; // num of seanses
    d[7]=0; // num of seanses

    d[8]=DEVICE_TYPE_PC; // dev type
    d[9]=0x01; // start frame


    // packet ln

    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;

    /////////////////////////////

    d[14] = 0x02; // version proto
    d[15] = 0; // encr type

    // skip 16 bytes

    data.append(d,16);


    // data block ////////////////////

    QByteArray block;

    block.append((char)0x01); // periphery setting
    block.append((char)0x0);

    block.append((char)0x06);
    block.append((char)0);

//    block.append((char)(dt.date().year()-2000));
//    block.append((char)(dt.date().month()-1));
//    block.append((char)(dt.date().day()));
//    block.append((char)(dt.date().dayOfWeek()-1));

//    block.append((char)(dt.time().hour()));
//    block.append((char)(dt.time().minute()));
//    block.append((char)(dt.time().second()));

    QDateTime dt = QDateTime::currentDateTime();
    dt.setTimeSpec(Qt::LocalTime);
    qDebug() << dt.timeZone();
    quint64 timestamp = dt.toMSecsSinceEpoch();

    qDebug() << timestamp;
    QByteArray timestampChunk;
    timestampChunk.insert(0,reinterpret_cast<char *>(&timestamp),8);
    qDebug() << timestampChunk.toHex();

    block.append(timestampChunk);

    quint16 len = block.size()+2;
    block.insert(0,reinterpret_cast<char*>(&len),2);

    data.append(block);

    /////////////////////////////////

    closePacket();
}


//////////////////////////////////////////////////////////////////////
void GeoStreamPacketVirtual::makeInfoReq()
{
      startPk(INFO_SEQ, INFO_REQ);
      closePacket();
}
////////////////
bool GeoStreamPacketVirtual::checkInfoAk()
{
      if(data.size() < MINPACKETSIZE)
          return false;
      gsheader gs;
      memcpy(reinterpret_cast<void*>(&gs),
             reinterpret_cast<void*>(data.data()),
             sizeof(gsheader));

      if(gs.startseq != INFO_SEQ)
          return false;
      if(gs.argseq != INFO_AK)
          return false;

      return true;
}




int GeoStreamPacketVirtual::readInfoAk()
{
      if(data.size() < MINPACKETSIZE)
          return 0;


      QByteArray d = getDataBlock();

      current_firmware_version = d.mid(2+8,8);

      int mode = (uchar)(d.mid(2+8*3+44,1).data()[0]);

      return mode;
}


void GeoStreamPacketVirtual::makeBootReq()
{
      startPk(SRV_SEQ, BOOT_REQ);
      closePacket();
}
////////////////
bool GeoStreamPacketVirtual::checkBootAk()
{
      if(data.size() < MINPACKETSIZE)
          return false;
      gsheader gs;
      memcpy(reinterpret_cast<void*>(&gs),
             reinterpret_cast<void*>(data.data()),
             sizeof(gsheader));

      if(gs.startseq != SRV_SEQ)
          return false;
      if(gs.argseq != BOOT_AK)
          return false;

      return true;
}

void GeoStreamPacketVirtual::makeWorkReq()
{
      startPk(SRV_SEQ, WORK_REQ);
      closePacket();
}

void GeoStreamPacketVirtual::makeIDInfo() {
    startPk(ID_SEQ, ID_REQ);
    closePacket();
}

bool GeoStreamPacketVirtual::checkIDInfo(int& error) {
    if(data.size() < MINPACKETSIZE)
      return false;

    error = (char)data[3];

    if(data[0] != (char)0x45 ||
       data[1] != (char)0x00 ||
       data[2] != (char)0x60)
       return false;

    return true;
}

void GeoStreamPacketVirtual::makeCraneInfo(const QByteArray& _data) {
    startPk(CRN_SEQ, CRN_REQ);
    QByteArray b;
    b.reserve(_data.size() + 2);
    quint16 len = _data.size() + 2;
    b.insert(0,reinterpret_cast<char*>(&len),2);
    b.append(_data, _data.size());
    data.append(b, b.size());
    closePacket();
}

bool GeoStreamPacketVirtual::checkCraneInfo(int& error) {
    if(data.size() < MINPACKETSIZE)
      return false;

    error = (char)data[3];

    if(data[0] != (char)0x40 ||
       data[1] != (char)0x00 ||
       data[2] != (char)0x60)
       return false;

    return true;
}

void GeoStreamPacketVirtual::makeCfgPrph(const QByteArray &_data) {
    startPk(CFG_SEQ, DEV_SCAN_REQ);
    data.append(_data, _data.size());
    closePacket();
}

bool GeoStreamPacketVirtual::checkCfgPrph(int &error) {
    if(data.size() < MINPACKETSIZE)
      return false;

    error = (char)data[3];

    if(data[0] != (char)0xDB ||
       data[1] != (char)0xEA ||
       data[2] != (char)0x01)
       return false;

    return true;

}

////////////////
bool GeoStreamPacketVirtual::checkWorkAk()
{
      if(data.size() < MINPACKETSIZE)
          return false;
      gsheader gs;
      memcpy(reinterpret_cast<void*>(&gs),
             reinterpret_cast<void*>(data.data()),
             sizeof(gsheader));

      if(gs.startseq != SRV_SEQ)
          return false;
      if(gs.argseq != WORK_AK)
          return false;

      return true;
}



void GeoStreamPacketVirtual::makeFwUp()
{

      gsheader gs;
      startPk(FW_SEQ, FW_UP, &gs);
      //
      gs.nsessions = fw_npieces-fw_current_piece-1;
      gs.session_flag = flags;
      gs.encr_type = RSA_AES;
      //
      data.append(reinterpret_cast<char*>(&gs),sizeof(gs));
      //
      data.append(block);
      //
      closePacket();

}

bool GeoStreamPacketVirtual::checkReturnErrs(int& errorCode)
{
      if((uchar)data[3] > 0 && (uchar)data[3] < 0xFF)
      {
          errorCode = (uchar)data[3];
          return true;
      }
      return false;
}



bool GeoStreamPacketVirtual::checkFwAk()
{
      if(data.size() < MINPACKETSIZE)
          return false;
      gsheader gs;
      memcpy(reinterpret_cast<void*>(&gs),
             reinterpret_cast<void*>(data.data()),
             sizeof(gsheader));

      if(gs.startseq != FW_SEQ)
          return false;
      if(gs.argseq != FW_AK)
          return false;

      return true;
}



