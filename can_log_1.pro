#-------------------------------------------------
#
# Project created by QtCreator 2019-04-08T12:49:14
#
#-------------------------------------------------

QT       += core gui serialport printsupport charts

win32:RC_ICONS += icon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CBI
TEMPLATE = app

INCLUDEPATH += modules/geostreamlib/

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT


LIBS += D:\QTPROJECTS\LOG\Crypt32.lib
LIBS += D:\QTPROJECTS\LOG\AdvAPI32.lib
LIBS += D:\QTPROJECTS\LOG\libeay32MD.lib
LIBS += D:\QTPROJECTS\LOG\WS2_32.Lib
LIBS += D:\QTPROJECTS\LOG\WSock32.Lib

CONFIG += c++11

SOURCES += \
    graph.cpp \
        main.cpp \
        mainwindow.cpp \
    geostreampacketvirtual.cpp \
    #modules/geostreamlib/Crc/ModbusCrc.cpp \
    #modules/geostreamlib/Crc/ModbusCrc.cpp \
    modules/geostreamlib/geostreamlib.cpp \
    #modules/geostreamlib/geostreamlib.cpp \
    #modules/geostreamlib/geostreampacket.cpp \
    #modules/geostreamlib/geostreampacket.cpp \
    #modules/geostreamlib/geostreampacketauth.cpp \
    #modules/geostreamlib/geostreampacketauth.cpp \
    modules/geostreamlib/geostreampacketdata.cpp \
    #modules/geostreamlib/geostreampacketdata.cpp \
    #modules/geostreamlib/geostreampacketvirtual.cpp \
    #modules/geostreamlib/geostreampacketvirtual.cpp \
    #modules/geostreamlib/library/aes.c \
    #modules/geostreamlib/library/aes.c \
    #modules/geostreamlib/library/aesni.c \
    #modules/geostreamlib/library/aesni.c \
    #modules/geostreamlib/library/arc4.c \
    #modules/geostreamlib/library/arc4.c \
    modules/geostreamlib/library/aria.c \
    #modules/geostreamlib/library/aria.c \
    #modules/geostreamlib/library/asn1parse.c \
    #modules/geostreamlib/library/asn1parse.c \
    #modules/geostreamlib/library/asn1write.c \
    #modules/geostreamlib/library/asn1write.c \
    #modules/geostreamlib/library/base64.c \
    #modules/geostreamlib/library/base64.c \
    #modules/geostreamlib/library/bignum.c \
    #modules/geostreamlib/library/bignum.c \
    #modules/geostreamlib/library/blowfish.c \
    #modules/geostreamlib/library/blowfish.c \
    #modules/geostreamlib/library/camellia.c \
    #modules/geostreamlib/library/camellia.c \
    #modules/geostreamlib/library/ccm.c \
    #modules/geostreamlib/library/ccm.c \
    #modules/geostreamlib/library/certs.c \
    #modules/geostreamlib/library/certs.c \
    #modules/geostreamlib/library/chacha20.c \
    #modules/geostreamlib/library/chacha20.c \
    #modules/geostreamlib/library/chachapoly.c \
    #modules/geostreamlib/library/chachapoly.c \
    #modules/geostreamlib/library/cipher.c \
    #modules/geostreamlib/library/cipher.c \
    #modules/geostreamlib/library/cipher_wrap.c \
    #modules/geostreamlib/library/cipher_wrap.c \
    modules/geostreamlib/library/cmac.c \
    #modules/geostreamlib/library/cmac.c \
    #modules/geostreamlib/library/ctr_drbg.c \
    #modules/geostreamlib/library/ctr_drbg.c \
    #modules/geostreamlib/library/debug.c \
    #modules/geostreamlib/library/debug.c \
    #modules/geostreamlib/library/des.c \
    #modules/geostreamlib/library/des.c \
    #modules/geostreamlib/library/dhm.c \
    #modules/geostreamlib/library/dhm.c \
    #modules/geostreamlib/library/ecdh.c \
    #modules/geostreamlib/library/ecdh.c \
    #modules/geostreamlib/library/ecdsa.c \
    #modules/geostreamlib/library/ecdsa.c \
    modules/geostreamlib/library/ecjpake.c \
    #modules/geostreamlib/library/ecjpake.c \
    #modules/geostreamlib/library/ecp.c \
    #modules/geostreamlib/library/ecp.c \
    #modules/geostreamlib/library/ecp_curves.c \
    #modules/geostreamlib/library/ecp_curves.c \
    #modules/geostreamlib/library/entropy.c \
    #modules/geostreamlib/library/entropy.c \
    #modules/geostreamlib/library/entropy_poll.c \
    #modules/geostreamlib/library/entropy_poll.c \
    #modules/geostreamlib/library/error.c \
    #modules/geostreamlib/library/error.c \
    #modules/geostreamlib/library/gcm.c \
    #modules/geostreamlib/library/gcm.c \
    modules/geostreamlib/library/havege.c \
    #modules/geostreamlib/library/havege.c \
    #modules/geostreamlib/library/hkdf.c \
    #modules/geostreamlib/library/hkdf.c \
    #modules/geostreamlib/library/hmac_drbg.c \
    #modules/geostreamlib/library/hmac_drbg.c \
    #modules/geostreamlib/library/md.c \
    #modules/geostreamlib/library/md.c \
    modules/geostreamlib/library/md2.c \
    #modules/geostreamlib/library/md2.c \
    modules/geostreamlib/library/md4.c \
    #modules/geostreamlib/library/md4.c \
    #modules/geostreamlib/library/md5.c \
    #modules/geostreamlib/library/md5.c \
    #modules/geostreamlib/library/md_wrap.c \
    #modules/geostreamlib/library/md_wrap.c \
    modules/geostreamlib/library/memory_buffer_alloc.c \
    #modules/geostreamlib/library/memory_buffer_alloc.c \
    #modules/geostreamlib/library/net_sockets.c \
    #modules/geostreamlib/library/net_sockets.c \
    modules/geostreamlib/library/nist_kw.c \
    #modules/geostreamlib/library/nist_kw.c \
    #modules/geostreamlib/library/oid.c \
    #modules/geostreamlib/library/oid.c \
    #modules/geostreamlib/library/padlock.c \
    #modules/geostreamlib/library/padlock.c \
    #modules/geostreamlib/library/pem.c \
    #modules/geostreamlib/library/pem.c \
    #modules/geostreamlib/library/pk.c \
    #modules/geostreamlib/library/pk.c \
    #modules/geostreamlib/library/pk_wrap.c \
    #modules/geostreamlib/library/pk_wrap.c \
    modules/geostreamlib/library/pkcs11.c \
    #modules/geostreamlib/library/pkcs11.c \
    #modules/geostreamlib/library/pkcs12.c \
    #modules/geostreamlib/library/pkcs12.c \
    #modules/geostreamlib/library/pkcs5.c \
    #modules/geostreamlib/library/pkcs5.c \
    #modules/geostreamlib/library/pkparse.c \
    #modules/geostreamlib/library/pkparse.c \
    #modules/geostreamlib/library/pkwrite.c \
    #modules/geostreamlib/library/pkwrite.c \
    #modules/geostreamlib/library/platform.c \
    #modules/geostreamlib/library/platform.c \
    #modules/geostreamlib/library/platform_util.c \
    #modules/geostreamlib/library/platform_util.c \
    #modules/geostreamlib/library/poly1305.c \
    #modules/geostreamlib/library/poly1305.c \
    #modules/geostreamlib/library/ripemd160.c \
    #modules/geostreamlib/library/ripemd160.c \
    #modules/geostreamlib/library/rsa.c \
    #modules/geostreamlib/library/rsa.c \
    #modules/geostreamlib/library/rsa_internal.c \
    #modules/geostreamlib/library/rsa_internal.c \
    #modules/geostreamlib/library/sha1.c \
    #modules/geostreamlib/library/sha1.c \
    #modules/geostreamlib/library/sha256.c \
    #modules/geostreamlib/library/sha256.c \
    #modules/geostreamlib/library/sha512.c \
    #modules/geostreamlib/library/sha512.c \
    #modules/geostreamlib/library/ssl_cache.c \
    #modules/geostreamlib/library/ssl_cache.c \
    #modules/geostreamlib/library/ssl_ciphersuites.c \
    #modules/geostreamlib/library/ssl_ciphersuites.c \
    #modules/geostreamlib/library/ssl_cli.c \
    #modules/geostreamlib/library/ssl_cli.c \
    #modules/geostreamlib/library/ssl_cookie.c \
    #modules/geostreamlib/library/ssl_cookie.c \
    #modules/geostreamlib/library/ssl_srv.c \
    #modules/geostreamlib/library/ssl_srv.c \
    #modules/geostreamlib/library/ssl_ticket.c \
    #modules/geostreamlib/library/ssl_ticket.c \
    #modules/geostreamlib/library/ssl_tls.c \
    #modules/geostreamlib/library/ssl_tls.c \
    modules/geostreamlib/library/threading.c \
    #modules/geostreamlib/library/threading.c \
    #modules/geostreamlib/library/timing.c \
    #modules/geostreamlib/library/timing.c \
    #modules/geostreamlib/library/version.c \
    #modules/geostreamlib/library/version.c \
    #modules/geostreamlib/library/version_features.c \
    #modules/geostreamlib/library/version_features.c \
    #modules/geostreamlib/library/x509.c \
    #modules/geostreamlib/library/x509.c \
    #modules/geostreamlib/library/x509_create.c \
    #modules/geostreamlib/library/x509_create.c \
    #modules/geostreamlib/library/x509_crl.c \
    #modules/geostreamlib/library/x509_crl.c \
    #modules/geostreamlib/library/x509_crt.c \
    #modules/geostreamlib/library/x509_crt.c \
    #modules/geostreamlib/library/x509_csr.c \
    #modules/geostreamlib/library/x509_csr.c \
    #modules/geostreamlib/library/x509write_crt.c \
    #modules/geostreamlib/library/x509write_crt.c \
    #modules/geostreamlib/library/x509write_csr.c \
    #modules/geostreamlib/library/x509write_csr.c \
    #modules/geostreamlib/library/xtea.c \
    #modules/geostreamlib/library/xtea.c \
    qcustomplot.cpp \
    read_thread.cpp \
    update_thread.cpp \
    modules/geostreamlib/Crc/ModbusCrc.cpp \
    modules/geostreamlib/geostreampacket.cpp \
    modules/geostreamlib/geostreampacketauth.cpp \
    modules/geostreamlib/library/aes.c \
    modules/geostreamlib/library/aesni.c \
    modules/geostreamlib/library/arc4.c \
    modules/geostreamlib/library/aria.c \
    modules/geostreamlib/library/asn1parse.c \
    modules/geostreamlib/library/asn1write.c \
    modules/geostreamlib/library/base64.c \
    modules/geostreamlib/library/bignum.c \
    modules/geostreamlib/library/blowfish.c \
    modules/geostreamlib/library/camellia.c \
    modules/geostreamlib/library/ccm.c \
    modules/geostreamlib/library/certs.c \
    modules/geostreamlib/library/chacha20.c \
    modules/geostreamlib/library/chachapoly.c \
    modules/geostreamlib/library/cipher.c \
    modules/geostreamlib/library/cipher_wrap.c \
    modules/geostreamlib/library/cmac.c \
    modules/geostreamlib/library/ctr_drbg.c \
    modules/geostreamlib/library/debug.c \
    modules/geostreamlib/library/des.c \
    modules/geostreamlib/library/dhm.c \
    modules/geostreamlib/library/ecdh.c \
    modules/geostreamlib/library/ecdsa.c \
    modules/geostreamlib/library/ecjpake.c \
    modules/geostreamlib/library/ecp.c \
    modules/geostreamlib/library/ecp_curves.c \
    modules/geostreamlib/library/entropy.c \
    modules/geostreamlib/library/entropy_poll.c \
    modules/geostreamlib/library/error.c \
    modules/geostreamlib/library/gcm.c \
    modules/geostreamlib/library/havege.c \
    modules/geostreamlib/library/hkdf.c \
    modules/geostreamlib/library/hmac_drbg.c \
    modules/geostreamlib/library/md.c \
    modules/geostreamlib/library/md2.c \
    modules/geostreamlib/library/md4.c \
    modules/geostreamlib/library/md5.c \
    modules/geostreamlib/library/md_wrap.c \
    modules/geostreamlib/library/memory_buffer_alloc.c \
    modules/geostreamlib/library/net_sockets.c \
    modules/geostreamlib/library/nist_kw.c \
    modules/geostreamlib/library/oid.c \
    modules/geostreamlib/library/padlock.c \
    modules/geostreamlib/library/pem.c \
    modules/geostreamlib/library/pk.c \
    modules/geostreamlib/library/pk_wrap.c \
    modules/geostreamlib/library/pkcs11.c \
    modules/geostreamlib/library/pkcs12.c \
    modules/geostreamlib/library/pkcs5.c \
    modules/geostreamlib/library/pkparse.c \
    modules/geostreamlib/library/pkwrite.c \
    modules/geostreamlib/library/platform.c \
    modules/geostreamlib/library/platform_util.c \
    modules/geostreamlib/library/poly1305.c \
    modules/geostreamlib/library/ripemd160.c \
    modules/geostreamlib/library/rsa.c \
    modules/geostreamlib/library/rsa_internal.c \
    modules/geostreamlib/library/sha1.c \
    modules/geostreamlib/library/sha256.c \
    modules/geostreamlib/library/sha512.c \
    modules/geostreamlib/library/ssl_cache.c \
    modules/geostreamlib/library/ssl_ciphersuites.c \
    modules/geostreamlib/library/ssl_cli.c \
    modules/geostreamlib/library/ssl_cookie.c \
    modules/geostreamlib/library/ssl_srv.c \
    modules/geostreamlib/library/ssl_ticket.c \
    modules/geostreamlib/library/ssl_tls.c \
    modules/geostreamlib/library/threading.c \
    modules/geostreamlib/library/timing.c \
    modules/geostreamlib/library/version.c \
    modules/geostreamlib/library/version_features.c \
    modules/geostreamlib/library/x509.c \
    modules/geostreamlib/library/x509_create.c \
    modules/geostreamlib/library/x509_crl.c \
    modules/geostreamlib/library/x509_crt.c \
    modules/geostreamlib/library/x509_csr.c \
    modules/geostreamlib/library/x509write_crt.c \
    modules/geostreamlib/library/x509write_csr.c \
    modules/geostreamlib/library/xtea.c


HEADERS += \
    graph.h \
        mainwindow.h \
    geostreampacketvirtual.h \
    modules/geostreamlib/Crc/ModbusCrc.h \
    modules/geostreamlib/Crc/ModbusCrc.h \
    modules/geostreamlib/geostreamlib.h \
    modules/geostreamlib/geostreamlib.h \
    modules/geostreamlib/geostreamlib_global.h \
    modules/geostreamlib/geostreamlib_global.h \
    modules/geostreamlib/geostreampacket.h \
    modules/geostreamlib/geostreampacket.h \
    modules/geostreamlib/geostreampacketauth.h \
    modules/geostreamlib/geostreampacketauth.h \
    modules/geostreamlib/geostreampacketdata.h \
    modules/geostreamlib/geostreampacketdata.h \
    modules/geostreamlib/geostreampacketvirtual.h \
    modules/geostreamlib/mbedtls/aes.h \
    modules/geostreamlib/mbedtls/aes.h \
    modules/geostreamlib/mbedtls/aesni.h \
    modules/geostreamlib/mbedtls/aesni.h \
    modules/geostreamlib/mbedtls/arc4.h \
    modules/geostreamlib/mbedtls/arc4.h \
    modules/geostreamlib/mbedtls/aria.h \
    modules/geostreamlib/mbedtls/aria.h \
    modules/geostreamlib/mbedtls/asn1.h \
    modules/geostreamlib/mbedtls/asn1.h \
    modules/geostreamlib/mbedtls/asn1write.h \
    modules/geostreamlib/mbedtls/asn1write.h \
    modules/geostreamlib/mbedtls/base64.h \
    modules/geostreamlib/mbedtls/base64.h \
    modules/geostreamlib/mbedtls/bignum.h \
    modules/geostreamlib/mbedtls/bignum.h \
    modules/geostreamlib/mbedtls/blowfish.h \
    modules/geostreamlib/mbedtls/blowfish.h \
    modules/geostreamlib/mbedtls/bn_mul.h \
    modules/geostreamlib/mbedtls/bn_mul.h \
    modules/geostreamlib/mbedtls/camellia.h \
    modules/geostreamlib/mbedtls/camellia.h \
    modules/geostreamlib/mbedtls/ccm.h \
    modules/geostreamlib/mbedtls/ccm.h \
    modules/geostreamlib/mbedtls/certs.h \
    modules/geostreamlib/mbedtls/certs.h \
    modules/geostreamlib/mbedtls/chacha20.h \
    modules/geostreamlib/mbedtls/chacha20.h \
    modules/geostreamlib/mbedtls/chachapoly.h \
    modules/geostreamlib/mbedtls/chachapoly.h \
    modules/geostreamlib/mbedtls/check_config.h \
    modules/geostreamlib/mbedtls/check_config.h \
    modules/geostreamlib/mbedtls/cipher.h \
    modules/geostreamlib/mbedtls/cipher.h \
    modules/geostreamlib/mbedtls/cipher_internal.h \
    modules/geostreamlib/mbedtls/cipher_internal.h \
    modules/geostreamlib/mbedtls/cmac.h \
    modules/geostreamlib/mbedtls/cmac.h \
    modules/geostreamlib/mbedtls/compat-1.3.h \
    modules/geostreamlib/mbedtls/compat-1.3.h \
    modules/geostreamlib/mbedtls/config.h \
    modules/geostreamlib/mbedtls/config.h \
    modules/geostreamlib/mbedtls/ctr_drbg.h \
    modules/geostreamlib/mbedtls/ctr_drbg.h \
    modules/geostreamlib/mbedtls/debug.h \
    modules/geostreamlib/mbedtls/debug.h \
    modules/geostreamlib/mbedtls/des.h \
    modules/geostreamlib/mbedtls/des.h \
    modules/geostreamlib/mbedtls/dhm.h \
    modules/geostreamlib/mbedtls/dhm.h \
    modules/geostreamlib/mbedtls/ecdh.h \
    modules/geostreamlib/mbedtls/ecdh.h \
    modules/geostreamlib/mbedtls/ecdsa.h \
    modules/geostreamlib/mbedtls/ecdsa.h \
    modules/geostreamlib/mbedtls/ecjpake.h \
    modules/geostreamlib/mbedtls/ecjpake.h \
    modules/geostreamlib/mbedtls/ecp.h \
    modules/geostreamlib/mbedtls/ecp.h \
    modules/geostreamlib/mbedtls/ecp_internal.h \
    modules/geostreamlib/mbedtls/ecp_internal.h \
    modules/geostreamlib/mbedtls/entropy.h \
    modules/geostreamlib/mbedtls/entropy.h \
    modules/geostreamlib/mbedtls/entropy_poll.h \
    modules/geostreamlib/mbedtls/entropy_poll.h \
    modules/geostreamlib/mbedtls/error.h \
    modules/geostreamlib/mbedtls/error.h \
    modules/geostreamlib/mbedtls/gcm.h \
    modules/geostreamlib/mbedtls/gcm.h \
    modules/geostreamlib/mbedtls/havege.h \
    modules/geostreamlib/mbedtls/havege.h \
    modules/geostreamlib/mbedtls/hkdf.h \
    modules/geostreamlib/mbedtls/hkdf.h \
    modules/geostreamlib/mbedtls/hmac_drbg.h \
    modules/geostreamlib/mbedtls/hmac_drbg.h \
    modules/geostreamlib/mbedtls/md.h \
    modules/geostreamlib/mbedtls/md.h \
    modules/geostreamlib/mbedtls/md2.h \
    modules/geostreamlib/mbedtls/md2.h \
    modules/geostreamlib/mbedtls/md4.h \
    modules/geostreamlib/mbedtls/md4.h \
    modules/geostreamlib/mbedtls/md5.h \
    modules/geostreamlib/mbedtls/md5.h \
    modules/geostreamlib/mbedtls/md_internal.h \
    modules/geostreamlib/mbedtls/md_internal.h \
    modules/geostreamlib/mbedtls/memory_buffer_alloc.h \
    modules/geostreamlib/mbedtls/memory_buffer_alloc.h \
    modules/geostreamlib/mbedtls/net.h \
    modules/geostreamlib/mbedtls/net.h \
    modules/geostreamlib/mbedtls/net_sockets.h \
    modules/geostreamlib/mbedtls/net_sockets.h \
    modules/geostreamlib/mbedtls/nist_kw.h \
    modules/geostreamlib/mbedtls/nist_kw.h \
    modules/geostreamlib/mbedtls/oid.h \
    modules/geostreamlib/mbedtls/oid.h \
    modules/geostreamlib/mbedtls/padlock.h \
    modules/geostreamlib/mbedtls/padlock.h \
    modules/geostreamlib/mbedtls/pem.h \
    modules/geostreamlib/mbedtls/pem.h \
    modules/geostreamlib/mbedtls/pk.h \
    modules/geostreamlib/mbedtls/pk.h \
    modules/geostreamlib/mbedtls/pk_internal.h \
    modules/geostreamlib/mbedtls/pk_internal.h \
    modules/geostreamlib/mbedtls/pkcs11.h \
    modules/geostreamlib/mbedtls/pkcs11.h \
    modules/geostreamlib/mbedtls/pkcs12.h \
    modules/geostreamlib/mbedtls/pkcs12.h \
    modules/geostreamlib/mbedtls/pkcs5.h \
    modules/geostreamlib/mbedtls/pkcs5.h \
    modules/geostreamlib/mbedtls/platform.h \
    modules/geostreamlib/mbedtls/platform.h \
    modules/geostreamlib/mbedtls/platform_time.h \
    modules/geostreamlib/mbedtls/platform_time.h \
    modules/geostreamlib/mbedtls/platform_util.h \
    modules/geostreamlib/mbedtls/platform_util.h \
    modules/geostreamlib/mbedtls/poly1305.h \
    modules/geostreamlib/mbedtls/poly1305.h \
    modules/geostreamlib/mbedtls/ripemd160.h \
    modules/geostreamlib/mbedtls/ripemd160.h \
    modules/geostreamlib/mbedtls/rsa.h \
    modules/geostreamlib/mbedtls/rsa.h \
    modules/geostreamlib/mbedtls/rsa_internal.h \
    modules/geostreamlib/mbedtls/rsa_internal.h \
    modules/geostreamlib/mbedtls/sha1.h \
    modules/geostreamlib/mbedtls/sha1.h \
    modules/geostreamlib/mbedtls/sha256.h \
    modules/geostreamlib/mbedtls/sha256.h \
    modules/geostreamlib/mbedtls/sha512.h \
    modules/geostreamlib/mbedtls/sha512.h \
    modules/geostreamlib/mbedtls/ssl.h \
    modules/geostreamlib/mbedtls/ssl.h \
    modules/geostreamlib/mbedtls/ssl_cache.h \
    modules/geostreamlib/mbedtls/ssl_cache.h \
    modules/geostreamlib/mbedtls/ssl_ciphersuites.h \
    modules/geostreamlib/mbedtls/ssl_ciphersuites.h \
    modules/geostreamlib/mbedtls/ssl_cookie.h \
    modules/geostreamlib/mbedtls/ssl_cookie.h \
    modules/geostreamlib/mbedtls/ssl_internal.h \
    modules/geostreamlib/mbedtls/ssl_internal.h \
    modules/geostreamlib/mbedtls/ssl_ticket.h \
    modules/geostreamlib/mbedtls/ssl_ticket.h \
    modules/geostreamlib/mbedtls/threading.h \
    modules/geostreamlib/mbedtls/threading.h \
    modules/geostreamlib/mbedtls/timing.h \
    modules/geostreamlib/mbedtls/timing.h \
    modules/geostreamlib/mbedtls/version.h \
    modules/geostreamlib/mbedtls/version.h \
    modules/geostreamlib/mbedtls/x509.h \
    modules/geostreamlib/mbedtls/x509.h \
    modules/geostreamlib/mbedtls/x509_crl.h \
    modules/geostreamlib/mbedtls/x509_crl.h \
    modules/geostreamlib/mbedtls/x509_crt.h \
    modules/geostreamlib/mbedtls/x509_crt.h \
    modules/geostreamlib/mbedtls/x509_csr.h \
    modules/geostreamlib/mbedtls/x509_csr.h \
    modules/geostreamlib/mbedtls/xtea.h \
    modules/geostreamlib/mbedtls/xtea.h \
    qcustomplot.h \
    read_thread.h \
    \
    modules/geostreamlib/Crc/ModbusCrc.h \
    modules/geostreamlib/geostreampacket.h \
    modules/geostreamlib/geostreampacketauth.h \
    modules/geostreamlib/mbedtls/aes.h \
    modules/geostreamlib/mbedtls/aesni.h \
    modules/geostreamlib/mbedtls/arc4.h \
    modules/geostreamlib/mbedtls/aria.h \
    modules/geostreamlib/mbedtls/asn1.h \
    modules/geostreamlib/mbedtls/asn1write.h \
    modules/geostreamlib/mbedtls/base64.h \
    modules/geostreamlib/mbedtls/bignum.h \
    modules/geostreamlib/mbedtls/blowfish.h \
    modules/geostreamlib/mbedtls/bn_mul.h \
    modules/geostreamlib/mbedtls/camellia.h \
    modules/geostreamlib/mbedtls/ccm.h \
    modules/geostreamlib/mbedtls/certs.h \
    modules/geostreamlib/mbedtls/chacha20.h \
    modules/geostreamlib/mbedtls/chachapoly.h \
    modules/geostreamlib/mbedtls/check_config.h \
    modules/geostreamlib/mbedtls/cipher.h \
    modules/geostreamlib/mbedtls/cipher_internal.h \
    modules/geostreamlib/mbedtls/cmac.h \
    modules/geostreamlib/mbedtls/compat-1.3.h \
    modules/geostreamlib/mbedtls/config.h \
    modules/geostreamlib/mbedtls/ctr_drbg.h \
    modules/geostreamlib/mbedtls/debug.h \
    modules/geostreamlib/mbedtls/des.h \
    modules/geostreamlib/mbedtls/dhm.h \
    modules/geostreamlib/mbedtls/ecdh.h \
    modules/geostreamlib/mbedtls/ecdsa.h \
    modules/geostreamlib/mbedtls/ecjpake.h \
    modules/geostreamlib/mbedtls/ecp.h \
    modules/geostreamlib/mbedtls/ecp_internal.h \
    modules/geostreamlib/mbedtls/entropy.h \
    modules/geostreamlib/mbedtls/entropy_poll.h \
    modules/geostreamlib/mbedtls/error.h \
    modules/geostreamlib/mbedtls/gcm.h \
    modules/geostreamlib/mbedtls/havege.h \
    modules/geostreamlib/mbedtls/hkdf.h \
    modules/geostreamlib/mbedtls/hmac_drbg.h \
    modules/geostreamlib/mbedtls/md.h \
    modules/geostreamlib/mbedtls/md2.h \
    modules/geostreamlib/mbedtls/md4.h \
    modules/geostreamlib/mbedtls/md5.h \
    modules/geostreamlib/mbedtls/md_internal.h \
    modules/geostreamlib/mbedtls/memory_buffer_alloc.h \
    modules/geostreamlib/mbedtls/net.h \
    modules/geostreamlib/mbedtls/net_sockets.h \
    modules/geostreamlib/mbedtls/nist_kw.h \
    modules/geostreamlib/mbedtls/oid.h \
    modules/geostreamlib/mbedtls/padlock.h \
    modules/geostreamlib/mbedtls/pem.h \
    modules/geostreamlib/mbedtls/pk.h \
    modules/geostreamlib/mbedtls/pk_internal.h \
    modules/geostreamlib/mbedtls/pkcs11.h \
    modules/geostreamlib/mbedtls/pkcs12.h \
    modules/geostreamlib/mbedtls/pkcs5.h \
    modules/geostreamlib/mbedtls/platform.h \
    modules/geostreamlib/mbedtls/platform_time.h \
    modules/geostreamlib/mbedtls/platform_util.h \
    modules/geostreamlib/mbedtls/poly1305.h \
    modules/geostreamlib/mbedtls/ripemd160.h \
    modules/geostreamlib/mbedtls/rsa.h \
    modules/geostreamlib/mbedtls/rsa_internal.h \
    modules/geostreamlib/mbedtls/sha1.h \
    modules/geostreamlib/mbedtls/sha256.h \
    modules/geostreamlib/mbedtls/sha512.h \
    modules/geostreamlib/mbedtls/ssl.h \
    modules/geostreamlib/mbedtls/ssl_cache.h \
    modules/geostreamlib/mbedtls/ssl_ciphersuites.h \
    modules/geostreamlib/mbedtls/ssl_cookie.h \
    modules/geostreamlib/mbedtls/ssl_internal.h \
    modules/geostreamlib/mbedtls/ssl_ticket.h \
    modules/geostreamlib/mbedtls/threading.h \
    modules/geostreamlib/mbedtls/timing.h \
    modules/geostreamlib/mbedtls/version.h \
    modules/geostreamlib/mbedtls/x509.h \
    modules/geostreamlib/mbedtls/x509_crl.h \
    modules/geostreamlib/mbedtls/x509_crt.h \
    modules/geostreamlib/mbedtls/x509_csr.h \
    modules/geostreamlib/mbedtls/xtea.h \
    update_thread.h



FORMS += \
    cansendwidget.ui \
    cansetting.ui \
    convert.ui \
    graph.ui \
    mainwindow.ui \
    dialog.ui \
    mdisubwindow.ui \
    needUpdate.ui \
    packetRow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES = resources.qrc

SUBDIRS += \
    modules/geostreamlib/geostreamlib.pro \
    modules/geostreamlib/geostreamlib.pro

DISTFILES += \
    AdvAPI32.lib \
    Crypt32.lib \
    WS2_32.Lib \
    WSock32.Lib \
    libeay32MD.lib \
    libeay32MDd.lib \
    libeay32MT.lib \
    libeay32MTd.lib \
    modules/geostreamlib/geostreamlib.pri \
    modules/geostreamlib/geostreamlib.pri \
    modules/geostreamlib/library/.gitignore \
    modules/geostreamlib/library/.gitignore \
    modules/geostreamlib/library/CMakeLists.txt \
    modules/geostreamlib/library/CMakeLists.txt
