contains (QT_CONFIG, nacl) {
    TARGET = wiggly.nexe
} else {
    TARGET = wiggly
}

TEMPLATE = app

SOURCES += main.cpp wigglywidget.cpp dialog.cpp
HEADERS += wigglywidget.h dialog.h
