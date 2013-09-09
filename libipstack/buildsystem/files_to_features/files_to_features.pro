#-------------------------------------------------
#
# Project created by QtCreator 2013-09-02T03:56:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = files_to_features
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    fileModel.cpp \
    pickdependencies.cpp \
    dependencyModel.cpp \
    componentModel.cpp \
    componentmodelbaseitem.cpp \
    componentmodelfileitem.cpp \
    componentModelItem.cpp \
    qlistwidgetitemwithcompmodelindex.cpp

HEADERS  += mainwindow.h \
    picojson.h \
    options.h \
    fileModel.h \
    pickdependencies.h \
    dependencyModel.h \
    componentModel.h \
    componentModelItem.h \
    componentmodelbaseitem.h \
    componentmodelfileitem.h \
    qlistwidgetitemwithcompmodelindex.h

FORMS    += mainwindow.ui \
    pickdependencies.ui
