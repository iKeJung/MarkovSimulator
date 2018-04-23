#-------------------------------------------------
#
# Project created by QtCreator 2018-03-30T00:54:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimuladorMarkov
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    MarkovSimulator.cpp \
    RunnableSimulation.cpp

HEADERS  += MainWindow.h \
    MarkovSimulator.h \
    RunnableSimulation.h

FORMS    += MainWindow.ui

DISTFILES += \
    ToDo.txt
	
