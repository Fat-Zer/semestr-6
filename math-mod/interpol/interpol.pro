QT       += core gui
CONFIG   += qwt

TARGET = interpol
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    csvopendialog.cpp \
    Document.cpp \
    DocumentKeeper.cpp \
    TableDocument.cpp \
    NonregularTableModel.cpp

HEADERS  += mainwindow.h \
    csvopendialog.h \
    Document.h \
    DocumentKeeper.h \
    TableDocument.h \
    NonregularTableModel.h \
    Types.h \
    Vertex.h

FORMS += \
    mainwindow.ui \
    csvopendialog.ui
