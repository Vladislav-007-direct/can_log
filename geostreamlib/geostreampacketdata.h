#pragma once

#include "geostreampacket.h"

class geoStreamPacketData : public GeoStreamPacket
{
private:


	QByteArray block;

 	int fw_npieces = -1;
	int fw_current_piece = -1;

	int flags = 0;

	int device_type=-1;
public:

    ////////////////////
    geoStreamPacketData();
    ////////////////////

    int deviceType() { 
	
	if(data.size()>=9)	device_type = data[8];
	return device_type; }

    void initFromBufferData(QByteArray data_)
    {
        data.clear();
        data.append(data_);
    }
    void initFromBufferData(char* data_, int n)
    {
        data.clear();
        data.append(data_,n);
    }

    void initFwBlock(QByteArray& block_)
    {
        block = block_;
    }
    void initCfgBlock(QByteArray& block_, int fw_npieces_, int fw_current_piece_)
    {
        block = block_;

        fw_npieces = fw_npieces_;
        fw_current_piece = fw_current_piece_;

        if(fw_current_piece == 0)
            flags = 0x1;
        else
            flags = 0x0;
    }
    bool checkAuth0()
    {


        if((uchar)data[0] != 0x40)
            return false;
        if((uchar)data[1] != 0x00)
            return false;
        if((uchar)data[2] != 0xa0) // 
            return false;
        if((uchar)data[3] != 0xff)
            return false;


	return true;
    }

    bool checkAuth1()
    {

        if((uchar)data[0] != 0x40)
            return false;
        if((uchar)data[1] != 0x00)
            return false;
        if((uchar)data[2] != 0xa4) // 
            return false;
        if((uchar)data[3] != 0x00)
            return false;
        if(!(	(uchar)data[8] == DEVICE_TYPE_BMN 		|| // device_type
        	(uchar)data[8] == DEVICE_TYPE_TRACKER		||
		(uchar)data[8] == DEVICE_TYPE_SCANIA_TRACKER)) // device_type
            return false;
        if(!((uchar)data[9] & 0x01)) //
            return false;
        if((uchar)data[14] != 0x02) // version proto
            return false;
        if((uchar)data[15] != 0x03) // encr_type
            return false;

	device_type = data[8];

	return true;
    }

    bool checkFwTT() // transmission timeout error 
    {


        if((uchar)data[0] != 0xFF)
            return false;
        if((uchar)data[1] != 0x00)
            return false;
        if((uchar)data[2] != 0x00) 
            return false;
        if((uchar)data[3] != 22)
            return false;
        if(!(	(uchar)data[8] == DEVICE_TYPE_BMN 	|| // device_type
        	(uchar)data[8] == DEVICE_TYPE_TRACKER	||
		(uchar)data[8] == DEVICE_TYPE_SCANIA_TRACKER)) // device_type
            return false;
        if(!((uchar)data[9] & 0x01)) // 0xa1
            return false;
        if((uchar)data[14] != 0x02) // version proto
            return false;

	device_type = data[8];

	return true;

    }

    bool checkFwAk()
    {

        if((uchar)data[0] != 0xFF)
            return false;
        if((uchar)data[1] != 0x00)
            return false;
        if((uchar)data[2] != 0x00) 
            return false;
        if((uchar)data[3] != 0x00)
            return false;
        if(!(	(uchar)data[8] == DEVICE_TYPE_BMN 	|| // device_type
        	(uchar)data[8] == DEVICE_TYPE_TRACKER	||
		(uchar)data[8] == DEVICE_TYPE_SCANIA_TRACKER)) // device_type
            return false;
        if(!((uchar)data[9] & 0x01)) // 0xa1
            return false;
        if((uchar)data[14] != 0x02) // version proto
            return false;
        //if((uchar)data[15] != 0x01) // encr_type
        //    return false;

	device_type = data[8];

	return true;
    }


    void makeFw1()
    {
        clear();

        char d[256];
        memset(d,0,256);

        d[0]=(uchar)0xFF;
        d[1]=(uchar)0x00; // start sequence // service

        // argument sequence
        d[2]=(char)0x00; ///// argument sequence
        d[3]=(char)0xFF;

        d[4]=(char)fw_npieces-fw_current_piece-1; // num of seanses
        d[5]=0; // num of seanses
        d[6]=0; // 
        d[7]=0; // 
        //
        d[8]=(char)MY_DEV_TYPE;
        d[9]=(char)flags;

        // packet ln
        d[10] = 0;
        d[11] = 0;
        d[12] = 0;
        d[13] = 0;
        /////////////////////////////
        d[14] = (char)0x02; // version proto
        d[15] = (char)RSA_AES; // encr type

        data.reserve(2048);
        data.append(d,16);
        data.append(block);

        closePacket();
    }
};
