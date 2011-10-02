include ($$PWD/../ppapi-plugin/src/ppapi-plugin.pri)

QT += declarative
CONFIG += release
INCLUDEPATH += $$PWD

SOURCES += $$PWD/declarativeviewer.cpp
HEADERS += $$PWD/declarativeviewer.h
OTHER_FILES += quick.nmf index.html quick.qml

