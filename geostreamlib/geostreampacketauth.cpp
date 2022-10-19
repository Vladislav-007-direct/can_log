////////////////////////////////

#include <QDateTime>
#include <QCryptographicHash>

////////////////////////////////

#include "geostreampacket.h"
#include "geostreampacketauth.h"

////////////////////////////////

GeoStreamPacketAuth::GeoStreamPacketAuth()
{

    rnd = QRandomGenerator::securelySeeded();

    mbedtls_rsa_init( &myrsa, MBEDTLS_RSA_PKCS_V15, 0 );
    mbedtls_rsa_init( &peerrsa, MBEDTLS_RSA_PKCS_V15, 0 );

    mbedtls_entropy_context entropy;

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

}

//////////////////////////////////////////////////////////////////////
QByteArray GeoStreamPacketAuth::getMyPubModulus()
{
    QByteArray modulus;
    QByteArray key;
    key.append("MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvIa92bkCYGKkYWgkZrK4FOIP3");
    key.append("71+G4EsXOcn88UYM2KWzOJHRr5Fq2dQ7W99Zpbec0vwTGuAhFycFzNunAM6s6Gfy");
    key.append("82X3KUR7YYn3C4fBdk2mMUyScfrV1nLWXfHOQaejYFj59tP1Hi5wy9BPR1EN9jMC");
    key.append("aJXariCkaIYXPpKX/QIDAQAB");
    modulus.append(&QByteArray::fromBase64(key).constData()[34],128);
    //qDebug() << "my pem key=" << QString::fromLocal8Bit(QByteArray::fromBase64(key).toHex('|'));
    return modulus;
}
bool GeoStreamPacketAuth::setPeerRSAPubKey(QByteArray modulus)
{

    if(modulus.size()==RSA_PUB_KEY_SIZE)
    {

     const uint8_t T1_pubExp[] =
      {
        0x01, 0x00, 0x01
      };

    int ret = 0;
    ret = mbedtls_rsa_import_raw(&peerrsa,
        reinterpret_cast<unsigned char const *>(modulus.constData()),
        RSA_PUB_KEY_SIZE,
        nullptr, 0,
        nullptr, 0,
        nullptr, 0,
        static_cast<unsigned char const *>(T1_pubExp), 3 );

        if(ret != 0)
        {
            qDebug() << "mbedtls_rsa_import_raw";
            return false;
        }
        ret = mbedtls_rsa_complete( &peerrsa );
        if(ret != 0)
        {
            qDebug() << "mbedtls_rsa_complete error " << ret; //////TODO CHECK
            return false;
        }
        ret = mbedtls_rsa_check_pubkey (&peerrsa);
        if(ret != 0)
        {
            qDebug() << "mbedtls_rsa_check_pubkey error"; //////TODO CHECK
            return false;
        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////
void GeoStreamPacketAuth::makeA1Req(int modifiers)
{
    gsheader gs;

    startPk(AUTH_SEQ, A1_REQ, &gs);
    //
    number_a2_total = A2_NUM_NUGGET+static_cast<qint16>(rnd.bounded(A2_NUM_RND_MAX));

    if(modifiers==1)
        number_a2_total = 1;

    qDebug() << "number_a2_total=" << number_a2_total;
    //
    // reset auth info
    last_a2_sent_refdelta = -1;
    last_a2_rcvd_refdelta = -1;
    last_a2_sent_time = QTime();
    pre_auth3_hash.clear();


    //gs.nsessions = number_a2_total;
    //number_a2_total--;
    //
    data.append(reinterpret_cast<char*>(&gs),sizeof(gs));
    //

    closePacket();
    //
}
bool GeoStreamPacketAuth::checkA1Ak()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq != AUTH_SEQ)
        return false;
    if(gs.argseq != A1_AK)
        return false;
    if(gs.encr_type!=NOENC)
        return false;
    if(gs.version_proto!=VER_PROTO)
        return false;

    return true;
}
////////////////////////////////////////////////////////////////
void GeoStreamPacketAuth::makeA2Req(int modifiers)
{
    gsheader gs;
    startPk(AUTH_SEQ, A2_REQ, &gs);
    //
    last_a2_sent_refdelta=AUTH_TIME_RND_NUGGET+static_cast<qint16>(rnd.bounded(AUTH_TIME_RND_MAX));
    last_a2_sent_time=QTime::currentTime();

    if(modifiers==1)
        last_a2_sent_time=QTime::currentTime().addSecs(5);

    //
    qDebug() << "number_a2_total=" << number_a2_total;
    gs.nsessions = number_a2_total;
    number_a2_total--;
    gs.parm = last_a2_sent_refdelta;
    //
    data.append(reinterpret_cast<char*>(&gs),sizeof(gs));
    //
    closePacket();


    if(number_a2_sent<0)
        number_a2_sent=0;
    number_a2_sent ++;

}
bool GeoStreamPacketAuth::checkA2Ak()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq!=AUTH_SEQ)
        return false;
    if(gs.argseq!=A2_AK)
        return false;
    if(gs.encr_type!=NOENC)
        return false;
    if(gs.version_proto!=VER_PROTO)
        return false;

    last_a2_rcvd_refdelta=gs.parm;

    qDebug() << "a2 ak ok";
    return true;
}

bool GeoStreamPacketAuth::checkA2AkValid(int modifiers)
{
    QTime currTime = QTime::currentTime();
    int refdelta = last_a2_sent_refdelta;
    int timeDelta = last_a2_sent_time.msecsTo(currTime);

    if((fabs(fabs(timeDelta)-refdelta*AUTH_TIME_STEP) < VALID_AUTH_TIME_DELTA) || modifiers==1)
    {
        pre_auth3_hash.append(QString::number(refdelta*AUTH_TIME_STEP)+"&");
        pre_auth3_hash.append(QString::number(last_a2_rcvd_refdelta*AUTH_TIME_STEP)+"&");

        if(number_a2_recvd<0)
         number_a2_recvd=0;
        number_a2_recvd ++;
        qDebug() << "a2 ak valid";

        qDebug() << "last_a2_rcvd_delta=" << last_a2_rcvd_refdelta;
        return true;
    }

    qDebug() << "timedelta=" << timeDelta;
    qDebug() << "refdelta=" << refdelta;
    qDebug() << "last_a2_sent_time" << last_a2_sent_time;

    qDebug() << "a2 ak invalid";
    return false;
}

bool GeoStreamPacketAuth::isA2Complete()
{
    qDebug() << "isComplete()";

    qDebug() << "number_a2_recvd" << number_a2_recvd;
    qDebug() << "number_a2_sent" << number_a2_sent;
    qDebug() << "number_a2_total" << number_a2_total;

    return    (
              number_a2_recvd > 0
              &&
              number_a2_sent > 0
              &&
              number_a2_sent == number_a2_recvd
              &&
              number_a2_total == 0
              );
}

////////////////////////////////////////////////////////////////
void GeoStreamPacketAuth::makeA3Req()
{
    gsheader gs;
    startPk(AUTH_SEQ, A3_REQ, &gs);
    //
    gs.encr_type = MD5;
    //
    data.append(reinterpret_cast<char*>(&gs),sizeof(gs));
    //
    pre_auth3_hash += "PbO0Ib3lI7UY";
    QByteArray hash = QCryptographicHash::hash(pre_auth3_hash.toLocal8Bit(),QCryptographicHash::Md5);
    data.append(hash.data(),MD5_DGST_SIZE);
    //
    closePacket();
}
bool GeoStreamPacketAuth::checkA3Ak()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq!=AUTH_SEQ)
        return false;
    if(gs.argseq!=A3_AK)
        return false;
    //if(gs.encr_type!=MD5)
    //    return false;
    if(gs.version_proto!=VER_PROTO)
        return false;

    return true;
}

bool GeoStreamPacketAuth::checkA3AkValid()
{
    return true;
}
////////////////////////////////////////////////////////////////
void GeoStreamPacketAuth::makeA4Req()
{
    gsheader gs;
    startPk(AUTH_SEQ, A4_REQ, &gs);
    //
    gs.encr_type = RSA_PUBKEY;
    data.append(reinterpret_cast<char*>(&gs),sizeof(gs));
    //
    QByteArray modulus = getMyPubModulus();
    data.append( modulus.constData(), RSA_PUB_KEY_SIZE);
    //
    closePacket();
}
bool GeoStreamPacketAuth::checkA4Ak()
{
    if(data.size() < MINPACKETSIZE)
        return false;
    gsheader gs;
    memcpy(reinterpret_cast<void*>(&gs),
           reinterpret_cast<void*>(data.data()),
           sizeof(gsheader));

    if(gs.startseq!=AUTH_SEQ)
        return false;
    if(gs.argseq!=A4_AK)
        return false;
    if(gs.encr_type!=RSA_PUBKEY)
        return false;
    if(gs.version_proto!=VER_PROTO)
        return false;

    return true;
}
bool GeoStreamPacketAuth::setA4Info()
{
    if(data.size() < MINPACKETSIZE+RSA_PUB_KEY_SIZE)
        return false;

    QByteArray pubkey = data.mid(sizeof(gsheader),RSA_PUB_KEY_SIZE);

    qDebug() << "recvd rsa pubkey=" << pubkey.toHex('=');
    return setPeerRSAPubKey(pubkey);
}

////////////////////////////////////////////////////////////////
