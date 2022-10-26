QT       += core gui serialport printsupport charts

win32:RC_ICONS += icon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CBI
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -L$$PWD/binlibs \
        -lCrypt32 \
        -lAdvAPI32 \
        -llibeay32MD \
        -lWS2_32 \
        -lWSock32

CONFIG += c++11

SOURCES += \
        graph.cpp \
        hexvalidator.cpp \
        main.cpp \
        mainwindow.cpp \
        geostreampacketvirtual.cpp \
        mainwindow_files.cpp \
        mainwindow_graph.cpp \
        mainwindow_load.cpp \
        mainwindow_send.cpp \
        mainwindow_show.cpp \
        mainwindow_trigger.cpp \
        qcustomplot.cpp \
        read_thread.cpp \
        update_thread.cpp


HEADERS += \
        graph.h \
        hexvalidator.h \
        mainwindow.h \
        geostreampacketvirtual.h \
        qcustomplot.h \
        read_thread.h \
        structs.h \
        ui_infoCrane.h \
        ui_peripheryRow.h \
        update_thread.h



FORMS += \
    cansendwidget.ui \
    cansetting.ui \
    convert.ui \
    dialog.ui \
    graph.ui \
    mainwindow.ui \
    mdisubwindow.ui \
    needUpdate.ui \
    packetRow.ui \
    periphery.ui

RESOURCES = resources.qrc

include(./geostreamlib/geostreamlib.pri)

DISTFILES += \
    AdvAPI32.lib \
    Crypt32.lib \
    WS2_32.Lib \
    WSock32.Lib \
    libeay32MD.lib \
    libeay32MDd.lib \
    libeay32MT.lib \
    libeay32MTd.lib

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
