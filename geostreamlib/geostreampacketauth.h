#ifndef GEOSTREAMPACKETAUTH_H
#define GEOSTREAMPACKETAUTH_H

//////////////////////////////////

#include <QRandomGenerator>
#include <QTime>

#include "mbedtls/aes.h"
#include "mbedtls/entropy.h"
#include "mbedtls/dhm.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"

#include "geostreampacket.h"

//////////////////////////////////


/////////////////////////////////

class GeoStreamPacketAuth : public GeoStreamPacket
{
private:
public:
    static const int AUTH_TIME_STEP=10;
    static const int VALID_AUTH_TIME_DELTA=100;

    static const qint16 AUTH_TIME_RND_MAX=90;
    static const qint16 AUTH_TIME_RND_NUGGET=10;

    static const qint16 A2_NUM_NUGGET=2;
    static const qint16 A2_NUM_RND_MAX=7;

    GeoStreamPacketAuth();


    void initFromBufferData(QByteArray data_)
    {
        data.clear();
        data.append(data_);
    }

    ////////////////////////////////////////
    QByteArray getMyPubModulus();
    bool setPeerRSAPubKey(QByteArray key);
    mbedtls_rsa_context getPeerRsa() { return peerrsa; }

    ////////////////////////////////////////


    /////////////////

    void makeA1Req(int modifiers=0);
    bool checkA1Ak();

    void makeA2Req(int modifiers=0);
    bool checkA2Ak();
    bool checkA2AkValid(int modifiers=0);
    bool isA2Complete();


    void makeA3Req();
    bool checkA3Ak();
    bool checkA3AkValid();

    void makeA4Req();
    bool checkA4Ak();
    bool setA4Info();

    /////////////////



    ////////////////////////////////////////

    qint16 recvd_delta() { return last_a2_rcvd_refdelta; }


private:

    qint16 number_a2_total = -1;
    qint16 number_a2_sent = -1;
    qint16 number_a2_recvd = -1;
    qint16 last_a2_sent_refdelta = -1;
    qint16 last_a2_rcvd_refdelta = -1;


    QTime last_a2_sent_time;
    QString pre_auth3_hash;

    QRandomGenerator rnd;

    mbedtls_rsa_context peerrsa;
    mbedtls_rsa_context myrsa;
    mbedtls_dhm_context dhm;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

};


#endif // GEOSTREAMPACKETAUTH_H
