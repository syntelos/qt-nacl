# -------------------------------------------------
# Project created by QtCreator 2009-11-17T06:06:52
# -------------------------------------------------
QT += network
TARGET = nacldemoserver
TEMPLATE = app
CONFIG-=app_bundle
SOURCES += main.cpp \
    serverwidget.cpp \
    httpserver.cpp
HEADERS += serverwidget.h \
    httpserver.h
FORMS += serverwidget.ui
RESOURCES += nacldemoserver.qrc
OTHER_FILES = \
    naclhtmltemplate.html \
    naclnmftemplate.nmf \
    check_browser.js \
    qtnaclloader.js \

