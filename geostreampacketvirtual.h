#ifndef GEOSTREAMPACKETVIRTUAL_H
#define GEOSTREAMPACKETVIRTUAL_H

#include "modules/geostreamlib/geostreampacket.h"

/////////////////////////////////

class GeoStreamPacketVirtual : public GeoStreamPacket
{
public:
    GeoStreamPacketVirtual();


    void initFromBufferData(QByteArray data_)
    {
        data.clear();
        data.append(data_);
    }
    void initFwBlock(QByteArray& block_, int fw_npieces_, int fw_current_piece_)
    {
            block = block_;

            fw_npieces = fw_npieces_;
            fw_current_piece = fw_current_piece_;

            if(fw_current_piece == 0)
                flags = 0x1;
            else
                flags = 0x0;
    }




    // general rw
    void makeIDCfgReq(QByteArray _data);
    void makeCfgReqOne(int niface0, int mode0, int type_id0, int speed0, int protocol0, int loging0);
    void makeCfgReq(int niface0, int mode0, int type_id0, int speed0, int protocol0, int loging0,
                    int niface1, int mode1, int type_id1, int speed1, int protocol1, int loging1);
    void makeStartReq();
    void makeStopReq();

    bool checkReturnErrs(int& errorCode);
    bool checkCfgPk(int& haveError);
    bool checkCfgIDPk(int& haveError);
    bool checkStartPk(int& error);

    void makeTxReq(QByteArray);
    void makeTimeSyncReq();

    ////////////////////////////////////////
    // firmware update rw


    void makeInfoReq();
    bool checkInfoAk();

    int readInfoAk();

    void makeBootReq();
    bool checkBootAk();

    void makeWorkReq();
    bool checkWorkAk();


    /////////////////

    void makeFwUp();
    bool checkFwAk();

    ////////////////////////////////////////



    QByteArray current_firmware_version;

    QByteArray block;
    qint16 fw_npieces = -1;
    qint16 fw_current_piece = -1;
    quint8 flags = 0;


};

#endif // GEOSTREAMPACKETVIRTUAL_H
