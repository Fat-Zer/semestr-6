QT       += core gui
CONFIG   += qwt

TARGET = interpol
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    csvopendialog.cpp \
    documentmediator.cpp

HEADERS  += mainwindow.h \
    csvopendialog.h \
    documentmediator.h

FORMS += \
    mainwindow.ui \
    csvopendialog.ui
