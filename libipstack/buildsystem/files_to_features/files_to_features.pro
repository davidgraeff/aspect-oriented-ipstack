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
    problem_list_item.cpp \
    filterproxymodel.cpp \
    pickdependencies.cpp \
    filemodel/fileModel.cpp \
    filemodel/filemodelitem.cpp \
    familymodel/familyModel.cpp \
    kconfigmodel/kconfigModel.cpp \
    familymodel/familyBaseItem.cpp \
    familymodel/familyComponent.cpp \
    familymodel/familyFile.cpp

HEADERS  += mainwindow.h \
    options.h \
    pickdependencies.h \
    problem_list_item.h \
    filterproxymodel.h \
    filemodel/fileModel.h \
    filemodel/filemodelitem.h \
    familymodel/familyModel.h \
    kconfigmodel/kconfigModel.h \
    familymodel/familyBaseItem.h \
    familymodel/familyFile.h \
    familymodel/familyComponent.h

FORMS    += mainwindow.ui \
    pickdependencies.ui
