DEFINES += QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION

contains (QT_CONFIG, nacl) {
    TARGET = hellogl.nexe
}

TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
SOURCES += glwidget.cpp
SOURCES += bubble.cpp

HEADERS += glwidget.h
HEADERS += bubble.h

RESOURCES += texture.qrc
QT += opengl

# install
