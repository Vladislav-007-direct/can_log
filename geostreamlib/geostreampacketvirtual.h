#ifndef GEOSTREAMPACKETVIRTUAL_H
#define GEOSTREAMPACKETVIRTUAL_H

#include "../geostreamlib/geostreampacket.h"

/////////////////////////////////

class GeoStreamPacketVirtual : public GeoStreamPacket
{
public:
    GeoStreamPacketVirtual();

    ////////////////////////////////////////

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
    int deviceType()
    {
        if(data.size()>=9)      device_type = data[8];
        return device_type;
    }


    bool checkReturnErrs();


    ////////////////////////////////////////
    // general rw

    void makeCfgReq(int niface0, int mode0, int type_id0, int speed0, int protocol0, int loging0,
                    int niface1, int mode1, int type_id1, int speed1, int protocol1, int loging1);
    void makeCfgReqOne(int niface0, int mode0, int type_id0, int speed0, int protocol0, int logging0);
    void makeStartReq();
    void makeStopReq();

    bool checkCfgPk();
    bool checkStartPk(int& error);

    void makeTxReq(QByteArray);
    void makeTxReqV2(QByteArray);
    void makeTimeSyncReq();

    ////////////////////////////////////////
    // firmware update rw

    void makeInfoReq();
    bool checkInfoAk();

    void makeTrpScanReq(quint8 itype, quint8 inumber);
    bool checkTrpScanAk();
    bool checkTrpScanReq();
    bool checkTrpScanErr();

    void makeTrpReq(quint8 itype, quint8 inumber, quint32 frameid);
    void makeTrpStopReq();
    bool checkTrpStopReq();
    bool checkTrpAk();
    bool checkDisconnectSignal();


    bool checkSrvAk();
    bool checkSrvReq();
    bool checkDataReq();

    int readInfoAk();

    void makeBootReq();
    bool checkBootAk();

    void makeWorkReq();
    bool checkWorkAk();

    void makeCalibrateReq();
    bool checkCalibratAk();

    void makeCfg1();
    void makeCfg2();
    void makeCfg3();
    void makeCfg4();
    void makeCfg10();
    void makeCfg12();
    void makeCfgD0();
    bool checkCfgIPk();
    bool checkCfgIPkError();

    bool checkCfg1Ak();
    bool checkCfg1Err();


    void makeSet1();
    bool checkSet1Ak();

    bool checkInMsgsAk();
    bool checkInMsgsErr();
    void makeInMsgs();
    /////////////////

    void makeFwUp(int enc_type=RSA_AES);
    bool checkFwAk();

    ////////////////////////////////////////

    QByteArray current_firmware_version;
    QByteArray block;
    qint16 fw_npieces = -1;
    qint16 fw_current_piece = -1;
    quint8 flags = 0;
    int device_type=-1;
};




#endif // GEOSTREAMPACKETVIRTUAL_H
