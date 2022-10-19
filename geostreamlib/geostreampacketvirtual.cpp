
#include "geostreampacketvirtual.h"
#include "../geostreamlib/geostreampacket.h"

#include <QDateTime>

GeoStreamPacketVirtual::GeoStreamPacketVirtual()
{}

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
bool GeoStreamPacketVirtual::checkCfgPk()
{
  if(data.size() < MINPACKETSIZE)
        return false;

  if(     data[0] != (char)0xdb     ||
          data[1] != (char)0xea     ||
          data[2] != (char)0x02     ||
          data[3] != (char)0x00     )
      return false;

  return true;
}

bool GeoStreamPacketVirtual::checkCfgIPk()
{
  if(data.size() < MINPACKETSIZE)
        return false;

  if(     data[0] != (char)0xdb     ||
          data[1] != (char)0xea     ||
         !(data[2] == (char)0x01 || data[2] == (char)0x02 || data[2] == (char)0x03 || data[2] == (char)0x10 || data[2] == (char)0x12 || data[2] == (char)0x04) ||
          data[3] != (char)0x00     )
      return false;

  return true;
}

bool GeoStreamPacketVirtual::checkCfgIPkError()
{
  if(data.size() < MINPACKETSIZE)
        return false;

  if (data[0] != (char)0xdb ||
      data[1] != (char)0xea ||
    !(data[2] == (char)0x01 || data[2] == (char)0x02 || data[2] == (char)0x03 || data[2] == (char)0x10 || data[2] == (char)0x12))
      return false;

  if (data[3] == (char)0x00)
      return false;

  return true;
}

bool GeoStreamPacketVirtual::checkCfg1Err()
{

    if((uchar)data[0] != 0xDB)
        return false;
    if((uchar)data[1] != 0xEA)
        return false;
    if((uchar)data[2] != 0x00)
        return false;
    if((uchar)data[3] != 0x00)
        return true;

    device_type = data[8];

    return false;
}

bool GeoStreamPacketVirtual::checkCfg1Ak()
{

    if((uchar)data[0] != 0xDB)
        return false;
    if((uchar)data[1] != 0xEA)
        return false;
    if((uchar)data[2] != 0x01)
        return false;
    if((uchar)data[3] != 0x00)
        return false;

    if(!((uchar)data[9] & 0x01)) // 0xa1
        return false;
    if((uchar)data[14] != 0x02) // version proto
        return false;

    device_type = data[8];

    return true;
}

void GeoStreamPacketVirtual::makeCfgD0()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0xDB;
    d[1]=(uchar)0xEA; // start sequence

    // argument sequence
    d[2]=0xD0; ///// argument sequence
    d[3]=0xFF;

    d[4]=fw_npieces-fw_current_piece-1; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
}

void GeoStreamPacketVirtual::makeCfg1()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0xDB;
    d[1]=(uchar)0xEA; // start sequence

    // argument sequence
    d[2]=0x01; ///// argument sequence
    d[3]=0xFF;

    d[4]=fw_npieces-fw_current_piece-1; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
}

void GeoStreamPacketVirtual::makeCfg2()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0xDB;
    d[1]=(uchar)0xEA; // start sequence

    // argument sequence
    d[2]=0x02; ///// argument sequence
    d[3]=0xFF;

    d[4]=fw_npieces-fw_current_piece-1; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
}

void GeoStreamPacketVirtual::makeCfg3()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0xDB;
    d[1]=(uchar)0xEA; // start sequence

    // argument sequence
    d[2]=0x03; ///// argument sequence
    d[3]=0xFF;

    d[4]=fw_npieces-fw_current_piece-1; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
}

void GeoStreamPacketVirtual::makeCfg4()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0xDB;
    d[1]=(uchar)0xEA; // start sequence

    // argument sequence
    d[2]=0x04; ///// argument sequence
    d[3]=0xFF;

    d[4]=fw_npieces-fw_current_piece-1; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
}

void GeoStreamPacketVirtual::makeCfg10()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0xDB;
    d[1]=(uchar)0xEA; // start sequence

    // argument sequence
    d[2]=0x10; ///// argument sequence
    d[3]=0xFF;

    d[4]=fw_npieces-fw_current_piece-1; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
}

void GeoStreamPacketVirtual::makeCfg12()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0xDB;
    d[1]=(uchar)0xEA; // start sequence

    // argument sequence
    d[2]=0x12; ///// argument sequence
    d[3]=0xFF;

    d[4]=fw_npieces-fw_current_piece-1; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
}

void GeoStreamPacketVirtual::makeSet1()
{
   clear();

   char d[256];
   memset(d,0,256);

   d[0]=(uchar)0xDB;
   d[1]=(uchar)0xEA; // start sequence

   // argument sequence
   d[2]=0x02; ///// argument sequence
   d[3]=0xFF;

   d[4]=0; // num of seanses
   d[5]=0; // num of seanses
   d[6]=0; //
   d[7]=0; //
   //
   //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
   d[8]=MY_DEV_TYPE;
   d[9]=flags;

   // packet ln
   d[10] = 0;
   d[11] = 0;
   d[12] = 0;
   d[13] = 0;
   /////////////////////////////
   d[14] = 0x02; // version proto
   d[15] = 0x0; // encr type

   data.reserve(2048);
   data.append(d,16);
       //
       // no empty imei

   data.append(block);

   closePacket();
}

bool GeoStreamPacketVirtual::checkSet1Ak()
{
    if((uchar)data[0]   != 0xDB)
        return false;
    if((uchar)data[1]   != 0xEA)
        return false;
    if((uchar)data[2]   != 0x02)
        return false;
    if((uchar)data[3]   != 0x00)
        return false;
    if(!((uchar)data[9] & 0x01)) // 0xa1
        return false;
    if((uchar)data[14]  != 0x02) // version proto
        return false;

    device_type = data[8];
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
void GeoStreamPacketVirtual::makeTxReqV2(QByteArray block)
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
//    quint16 len = block.size()+2;
//      block.insert(0,reinterpret_cast<char*>(&len),2);
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


  QDateTime dt = QDateTime::currentDateTime();


  block.append((char)0x01); // periphery setting
  block.append((char)0x0);

  block.append((char)0x06);
  block.append((char)0);

  block.append((char)(dt.date().year()-2000));
  block.append((char)(dt.date().month()-1));
  block.append((char)(dt.date().day()));
  block.append((char)(dt.date().dayOfWeek()-1));

  block.append((char)(dt.time().hour()));
  block.append((char)(dt.time().minute()));
  block.append((char)(dt.time().second()));



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
bool GeoStreamPacketVirtual::checkSrvAk()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != SRV_SEQ)
        return false;
    if(gs.argseq != C1_AK)
        return false;

    return true;
}
bool GeoStreamPacketVirtual::checkSrvReq()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != SRV_SEQ)
        return false;
    if(gs.argseq != C1_REQ)
        return false;

    return true;
}


//////////////////////////////////////////////////////////////////////


void GeoStreamPacketVirtual::makeTrpScanReq(quint8 itype, quint8 inumber)
{
    startPk(INFO_SEQ, DEV_SCAN);

    // add data block

    quint16 ln=4;

    data.append(reinterpret_cast<char*>(&ln),2);
    data.append(char(itype));
    data.append(char(inumber));

    closePacket();
}


bool GeoStreamPacketVirtual::checkTrpStopReq()
{
    return data.size()>=4
            && data[0]==char(0x2b)
            && data[1]==char(0x2b)
            && data[2]==char(0x2b)
            && data[3]==char(0x2b);

}

bool GeoStreamPacketVirtual::checkDisconnectSignal()
{
    return data.size()>=4
            && data[0]==char(0x2d)
            && data[1]==char(0x2d)
            && data[2]==char(0x2d)
            && data[3]==char(0x2d);

}


void GeoStreamPacketVirtual::makeTrpStopReq()
{
    clear();
    data.append(char(0x2b));
    data.append(char(0x2b));
    data.append(char(0x2b));
    data.append(char(0x2b));

}


void GeoStreamPacketVirtual::makeTrpReq(quint8 itype, quint8 inumber, quint32 frameid)
{
    startPk(SRV_SEQ, TRP_REQ);

    // add data block

    quint16 ln=8;

    data.append(reinterpret_cast<char*>(&ln),2);
    data.append(char(itype));
    data.append(char(inumber));
    data.append(reinterpret_cast<char*>(&frameid),4);

    closePacket();
}

////////////////
bool GeoStreamPacketVirtual::checkTrpScanAk()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != INFO_SEQ)
        return false;
    if(gs.argseq != DEV_SCAN_AK)
        return false;

    return true;
}

bool GeoStreamPacketVirtual::checkTrpAk()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != SRV_SEQ)
        return false;
    if(gs.argseq != TRP_AK)
        return false;

    return true;
}

bool GeoStreamPacketVirtual::checkDataReq()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != DATA_SEQ)
        return false;
    if(gs.argseq != DATA_REQ)
        return false;

    return true;

}

bool GeoStreamPacketVirtual::checkTrpScanReq()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != INFO_SEQ)
        return false;
    if(gs.argseq != DEV_SCAN_REQ)
        return false;

    return true;
}

bool GeoStreamPacketVirtual::checkTrpScanErr()
{
  if(data.size() < MINPACKETSIZE)
    return false;
  gsheader gs;
  memcpy(reinterpret_cast<void*>(&gs),
       reinterpret_cast<void*>(data.data()),
       sizeof(gsheader));

  if(gs.startseq != INFO_SEQ)
    return false;
  if( (gs.argseq&0xFF) != (DEV_SCAN_AK&0xFF) )
    return false;
  if( (gs.argseq >> 8) != 0 && (gs.argseq >> 8) != 0xFF)
  {
    return true;
  }

    return false;
}


//////////////////////////////////////////////////////////////////////



bool GeoStreamPacketVirtual::checkInMsgsAk()
{
    if((uchar)data[0] != 0x45)
        return false;
    if((uchar)data[1] != 0x00)
        return false;
    if((uchar)data[2] != 0x30)
        return false;
    if((uchar)data[3] != 0x00)
        return false;
    if(!((uchar)data[9] & 0x01)) // 0xa1
        return false;
    if((uchar)data[14] != 0x02) // version proto
        return false;

    device_type = data[8];

    return true;
}
bool GeoStreamPacketVirtual::checkInMsgsErr()
{
    if((uchar)data[0] != 0x45)
        return false;
    if((uchar)data[1] != 0x00)
        return false;
    if((uchar)data[2] != 0x30)
        return false;
    if((uchar)data[3] == 0x00)
        return false;
    if(!((uchar)data[9] & 0x01)) // 0xa1
        return false;
    if((uchar)data[14] != 0x02) // version proto
        return false;

    device_type = data[8];

    return true;
}


void GeoStreamPacketVirtual::makeInMsgs()
{
    clear();

    char d[256];
    memset(d,0,256);

    d[0]=(uchar)0x45;
    d[1]=(uchar)0x00; // start sequence

    // argument sequence
    d[2]=0x30; ///// argument sequence
    d[3]=0xFF;

    d[4]=0; // num of seanses
    d[5]=0; // num of seanses
    d[6]=0; //
    d[7]=0; //
    //
    //d[8]=DEVICE_TYPE_SERVER_GEOCODE;
    d[8]=MY_DEV_TYPE;
    d[9]=flags;

    // packet ln
    d[10] = 0;
    d[11] = 0;
    d[12] = 0;
    d[13] = 0;
    /////////////////////////////
    d[14] = 0x02; // version proto
    d[15] = 0x0; // encr type

    data.reserve(2048);
    data.append(d,16);
    //
    // no empty imei

    data.append(block);

    closePacket();
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

void GeoStreamPacketVirtual::makeCalibrateReq()
{
    startPk(CFG_SEQ, CALIBRATE_REQ);
    closePacket();
}

bool GeoStreamPacketVirtual::checkCalibratAk()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != CFG_SEQ) {
        return false;
    }
    if(gs.argseq != CALIBRATE_AK) {
        return false;
    }

    return true;
}

void GeoStreamPacketVirtual::makeFwUp(int enctype)
{
    gsheader gs;
    startPk(FW_SEQ, FW_UP, &gs);
    //
    gs.nsessions = fw_npieces-fw_current_piece-1;
    gs.session_flag = flags;
    gs.encr_type = enctype;
    //
    data.append(reinterpret_cast<char*>(&gs),sizeof(gs));
    //
    data.append(block);
    //
    closePacket();
}

bool GeoStreamPacketVirtual::checkReturnErrs()
{
    if((uchar)data[3] > 0 && (uchar)data[3] < 0xFF)
    {
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



