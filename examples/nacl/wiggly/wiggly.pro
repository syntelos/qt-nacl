# DEFINES += QT_PEPPER_USE_PEPPER_FONT_ENGINE

contains (QT_CONFIG, nacl) {
    include($$PWD/../../../src/plugins/platforms/nacl/ppapi-plugin.pri)
    TARGET = wiggly.nexe
} else {
    TARGET = wiggly
}

TEMPLATE = app

SOURCES += main.cpp wigglywidget.cpp dialog.cpp
HEADERS += wigglywidget.h dialog.h
