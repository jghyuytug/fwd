TARGET = DnfLogin_static
CONFIG += console static
CONFIG -= app_bundle

QT += core widgets network
QT -= gui

# 强制静态链接
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++

SOURCES += \
    main.cpp \
    widget.cpp \
    httpaesrequest.cpp \
    util.cpp \
    aes_cbc.cpp \
    checkqlineedit.cpp \
    clickablelabel.cpp \
    forgetdialog.cpp \
    regeditdialog.cpp \
    repasswddialog.cpp

HEADERS += \
    widget.h \
    httpaesrequest.h \
    util.h \
    aes_cbc.h \
    checkqlineedit.h \
    clickablelabel.h \
    forgetdialog.h \
    regeditdialog.h \
    repasswddialog.h

FORMS += \
    widget.ui \
    forgetdialog.ui \
    regeditdialog.ui \
    repasswddialog.ui

RESOURCES += \
    DnfLogin.qrc