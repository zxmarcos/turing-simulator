#-------------------------------------------------
#
# Project created by QtCreator 2014-03-13T01:07:33
#
#-------------------------------------------------

QT       += core gui xml printsupport
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TuringSim
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    stateitem.cpp \
    transitionitem.cpp \
    automataview.cpp \
    tapeview.cpp \
    machinescene.cpp \
    tape.cpp \
    toolbox.cpp \
    stateeditor.cpp \
    transitioneditor.cpp \
    machinedescriptor.cpp

HEADERS  += mainwindow.h \
    stateitem.h \
    transitionitem.h \
    automataview.h \
    tapeview.h \
    machinescene.h \
    tape.h \
    toolbox.h \
    stateeditor.h \
    transitioneditor.h \
    acceptstate.h \
    machinedescriptor.h

RESOURCES += \
    rsrc.qrc

OTHER_FILES += \
    styler.css \
    about.html
