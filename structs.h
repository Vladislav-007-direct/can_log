#pragma once

#include <QString>


struct parentstruct {
    int strlen16(const ushort* strarg) {
       if (!strarg) return -1;
       const ushort* str = strarg;
       for(;*str;++str);
       return str - strarg;
    }
    QString toStringTime(quint32 elem) {
        uchar ss = elem % 60;
        uchar mm = ((elem - ss) / 60) % 60;
        uchar hh = ((((elem - ss) / 60) - mm) / 60) % 24;
        quint32 dd = (((((elem - ss) / 60) - mm) / 60) - hh) / 24;
        QString time;
        if (dd > 0) time += QString::number(dd) + " ";
        if (hh < 10) time += "0";
        time += QString::number(hh);
        time += ":";
        if (mm < 10) time += "0";
        time += QString::number(mm);
        time += ":";
        if (ss < 10) time += "0";
        time += QString::number(ss);
        return time;
    }
    QString elemToUTF16(char* elem, quint32 size) {
        int usize = size / 2;
        ushort* utf16 = new ushort[usize];
        memset(utf16, 0, usize);
        memcpy(reinterpret_cast<void *>(utf16), elem, size);
        auto res = QString::fromUtf16(utf16, std::min(strlen16(utf16), usize));
        delete[] utf16;
        return res;
    }
};

#pragma pack(push,1)
struct can_frm
{
    quint32 frm_id;
    quint8 dlen;
    qint8 data[8];
};
#pragma pack(pop)

#pragma pack(push,1)
struct trblockheader
{
    quint8 interfaceType;
    quint8 interfaceNumber;
    quint64 timestamp;
    quint16 messageLength;
};
#pragma pack(pop)

#pragma pack(push,1)
struct trblock
{
    trblockheader header;
    can_frm canFrame;
    quint8 crc;
};
#pragma pack(pop)

#pragma pack(push, 1)

struct gsshortdata {
    gsshortdata() { memset(this,0,sizeof(gsshortdata));}
    quint16 startseq; // 1-2
    quint16 argseq; // 3-4
    qint16 nsessions;  // 5-6
    qint16 parm; // 7-8
    quint8 device_type; // 9
    quint8 session_flag; // 10
    quint32 full_length; // 11-14
    quint8 version_proto; // 15
    quint8 encr_type; // 16
    char imei[15]; // 17-31
    quint16 blocklen; // 32 - 33
    quint8 packtype; // 34 0 for our case
};

#pragma pack(pop)

#pragma pack(push, 1)

struct gsshortblock {
    quint16 blocklen;
    quint8 packtype; // 0 for our case
    qint64 ms;
    quint8 valid;
    qint32 lat;
    qint32 lon;
    quint16 speed;
    quint16 direction;
    quint16 alt;
    quint8 hdop;
    quint16 power;
    quint16 power_accum;

    QString toString() {
        QString res = "gsshortblock: {";
        res += QString("blocklen: %1, ").arg(blocklen);
        res += QString("packtype: %1, ").arg(packtype);
        res += QString("ms: %1, ").arg(ms);
        res += QString("valid: %1, ").arg(valid);
        res += QString("lat: %1, ").arg(lat);
        res += QString("lon: %1, ").arg(lon);
        res += QString("speed: %1, ").arg(speed);
        res += QString("direction: %1, ").arg(direction);
        res += QString("alt: %1, ").arg(alt);
        res += QString("hdop: %1, ").arg(hdop);
        res += QString("power: %1, ").arg(power);
        res += QString("power_accum: %1}").arg(power_accum);
        return res;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct cranestruct: public parentstruct {
    char elem0[32];
    char elem1[32];
    char elem2[64];
    char elem3[32];
    quint32 elem4;
    quint32 elem5;
    quint32 elem6;
    char elem7[32];
    char elem8[32];
    char elem9[32];
    quint32 elem10; // timestamp
    quint32 elem11;
    quint32 elem17; // timestamp
    char elem18[64];
};

#pragma pack(pop)

#pragma pack(push, 1)

struct craneinfostruct: public parentstruct {
    char elem0[32];
    char elem1[32];
    char elem2[64];
    char elem3[32];
    quint32 elem4;
    quint32 elem5;
    quint32 elem6;
    char elem7[32];
    char elem8[32];
    char elem9[32];
    quint32 elem10; // timestamp
    quint32 elem11;
    char elem12[16];
    char elem13[16];
    char elem14[64];
    char elem15[64];
    quint32 elem16;
    quint32 elem17; // timestamp
    char elem18[64];
    quint32 elem19; // timestamp
    quint32 elem20;
    quint32 elem21;
    quint32 elem22;
    quint32 elem23;
    quint32 elem24;
    quint32 elem25;
    quint32 elem26;
    quint32 elem27;
    quint32 elem28;
    quint32 elem29;
    quint32 elem30;
    float elem31;
    float elem32;

    QString elem0ToUTF16()  { return elemToUTF16(elem0,  sizeof(elem0));  }
    QString elem1ToUTF16()  { return elemToUTF16(elem1,  sizeof(elem1));  }
    QString elem2ToUTF16()  { return elemToUTF16(elem2,  sizeof(elem2));  }
    QString elem3ToUTF16()  { return elemToUTF16(elem3,  sizeof(elem3));  }
    QString elem7ToUTF16()  { return elemToUTF16(elem7,  sizeof(elem7));  }
    QString elem8ToUTF16()  { return elemToUTF16(elem8,  sizeof(elem8));  }
    QString elem9ToUTF16()  { return elemToUTF16(elem9,  sizeof(elem9));  }
    QString elem12ToUTF16() { return elemToUTF16(elem12, sizeof(elem12)); }
    QString elem13ToUTF16() { return elemToUTF16(elem13, sizeof(elem13)); }
    QString elem14ToUTF16() { return elemToUTF16(elem14, sizeof(elem14)); }
    QString elem15ToUTF16() { return elemToUTF16(elem15, sizeof(elem15)); }
    QString elem18ToUTF16() { return elemToUTF16(elem18, sizeof(elem18)); }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct peripherystruct: public parentstruct {
    quint16 arg = 0x0002;
    quint8 type_interface = 0x20;
    quint8 num_interface = 0x00;
    quint32 id = 0x00;
    quint8 start = 0x00;
    quint8 size = 0x00;
    quint8 conv = 0x00;
    quint8 order = 0x00;
    quint8 type_data = 0x00;
    quint8 reserv_1 = 0x00;
    quint8 reserv_2 = 0x00;
    quint8 reserv_3 = 0x00;
    float ratio = 0x00;
    quint8 settings = 0x00;
    char name[10];
    quint8 filter;

    char description[256];
    const quint8 STRUCT_SIZE = 31;

    QByteArray toHex() {
        QByteArray res;
        quint16 len = STRUCT_SIZE + 1 + 256;
        res.reserve(len);
        res.append(reinterpret_cast<char*>(this), STRUCT_SIZE + 1);
        res.append(description, sizeof(description));
        return res.toHex();
    }

    QString toString() {
        QString res = "";
        res += QString("{");
        res += QString("arg: %1, ").arg(arg);
        res += QString("type_interface: %1, ").arg(type_interface);
        res += QString("num_interface: %1, ").arg(num_interface);
        res += QString("id: %1, ").arg(id);
        res += QString("start: %1, ").arg(start);
        res += QString("size: %1, ").arg(size);
        res += QString("conv: %1, ").arg(conv);
        res += QString("order: %1, ").arg(order);
        res += QString("type_data: %1, ").arg(type_data);
        res += QString("reserv_1: %1, ").arg(reserv_1);
        res += QString("reserv_2: %1, ").arg(reserv_2);
        res += QString("reserv_3: %1, ").arg(reserv_3);
        res += QString("ratio: %1, ").arg(ratio);
        res += QString("settings: %1, ").arg(settings);
        res += QString("name: %1, ").arg(name);
        res += QString("filter: %1, ").arg(filter);
        res += QString("description: %1 ").arg(description);
        res += QString("}");
        return res;
    }
};

#pragma pack(pop)
