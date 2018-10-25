#-------------------------------------------------
#
# Project created by QtCreator 2018-10-12T09:28:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = jvoice
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#setup armadillo
ARMADILLO_HEADER_PATH = $$PWD/armadillo-9.100.5/include
#DEFINES += ARMA_USE_LAPACK
INCLUDEPATH += $$ARMADILLO_HEADER_PATH
VPATH += $$ARMADILLO_HEADER_PATH

SOURCES += main.cpp\
        mainwindow.cpp \
    jfft/jfft.cpp \
    dsp.cpp \
    jaudio.cpp \
    led.cpp

HEADERS  += mainwindow.h \
    jfft/jfft.h \
    dsp.h \
    jaudio.h \
    armadillo-9.100.5/include/armadillo \
    led.h

FORMS    += mainwindow.ui

win32 {
LIBS += -lm\
        -lwinmm\
        -lws2_32\
        -lpthread
}

#for windows build
win32:{
win32:contains(QMAKE_HOST.arch, x86_64) {
  LIBS += -L$$PWD/lapack/64bit
} else {
LIBS += -L$$PWD/lapack/32bit
}
}

#strange naming on windows. you may have to call these -llapack -lblas i'm not sure
win32:{
LIBS += -lliblapack -llibblas
} else {
LIBS += -llapack -lblas
}

