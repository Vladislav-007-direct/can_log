#include "geostreampacket.h"

QList<quint16> geostream_allowed_sequences({FW_SEQ, CFG_SEQ, INFO_SEQ, SRV_SEQ, AUTH_SEQ, DATA_SEQ});

GeoStreamPacket::GeoStreamPacket()
{

}

GeoStreamPacket::~GeoStreamPacket()
{

}


void GeoStreamPacket::startPk(quint16 startseq, quint16 argseq, gsheader* gsh)
{
    //
    clear();
    //
    gsheader gs;
    gs.startseq = startseq;
    gs.argseq = argseq;
    //
    gs.nsessions = 0;
    gs.session_flag=START_SESSION_FLAG;
    //
    gs.device_type=MY_DEV_TYPE;
    gs.version_proto=VER_PROTO;
    //
    gs.encr_type = NOENC;
    //
    if(!gsh)
        data.append(reinterpret_cast<char*>(&gs),sizeof(gs));
    //
    if(gsh)
    {
        memcpy(
                   reinterpret_cast<void*>(gsh),
                   reinterpret_cast<void*>(&gs),
                   sizeof(gsheader)
               );
    }
    //
}


bool GeoStreamPacket::checkLengthAndCRC()
{
	//qDebug() << "checkLengthAndCrc()";
    if(data.size() >= MINPACKETSIZE)
    {
      qint32 length = * (reinterpret_cast<qint32*>(&data.data()[10]) );

      if(length > data.size())
          return false;
      if(length < MINPACKETSIZE)
          return false;


      if(data.size() >= length)
      {
        quint16 my_crc = CRC16(reinterpret_cast<uint8_t*>(data.data()),length-2);

       if(my_crc == *(reinterpret_cast<quint16*>(&(data.data()[length-2])) ) )
       {
          return true;
       }
       qDebug() << "crc mismatch mycrc=" << my_crc
                << "recvcrc=" << *((quint16*)&(data.data()[length-2]));
      }
    }
    return false;
}


void GeoStreamPacket::closePacket()
{
    if(!packetClosed && data.size()>=MINPACKETSIZE-CRCSIZE)
    {

        quint32 length = data.size() + 2;

        memcpy(&data.data()[10],reinterpret_cast<char*>(&length),4);

        quint16 my_crc = CRC16(reinterpret_cast<uint8_t*>(data.data()),length-2);

        data.append(char(0));
        data.append(char(0));

        memcpy(&data.data()[length-2],reinterpret_cast<char*>(&my_crc),2);

        packetClosed = true;
    }
}

void GeoStreamPacket::clear()
{
    data.clear();
    packetClosed = false;
}
