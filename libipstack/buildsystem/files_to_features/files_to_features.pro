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
    models/fileModel.cpp \
    pickdependencies.cpp \
    models/dependencyModel.cpp \
    models/componentModel.cpp \
    models/componentmodelbaseitem.cpp \
    models/componentmodelfileitem.cpp \
    models/componentModelItem.cpp \
    problem_list_item.cpp \
    models/filemodelitem.cpp

HEADERS  += mainwindow.h \
    picojson.h \
    options.h \
    models/fileModel.h \
    pickdependencies.h \
    models/dependencyModel.h \
    models/componentModel.h \
    models/componentModelItem.h \
    models/componentmodelbaseitem.h \
    models/componentmodelfileitem.h \
    problem_list_item.h \
    models/filemodelitem.h

FORMS    += mainwindow.ui \
    pickdependencies.ui
