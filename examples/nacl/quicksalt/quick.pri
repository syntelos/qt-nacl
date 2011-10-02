QT += declarative
CONFIG += release
INCLUDEPATH += $$PWD

SOURCES += $$PWD/declarativeviewer.cpp \
           $$PWD/quick.cpp
HEADERS += $$PWD/declarativeviewer.h
OTHER_FILES += quick.nmf index.html quick.qml

