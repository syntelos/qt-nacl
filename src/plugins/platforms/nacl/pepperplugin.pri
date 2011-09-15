INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
CONFIG += release

LIBS += -lppruntime -lppapi_cpp -lplatform -lgio -lpthread -lsrpc  -m32

HEADERS += \
    $$PWD/pepperinstance.h\
    $$PWD/peppermodule.h
SOURCES += \
    $$PWD/pepperinstance.cpp \
    $$PWD/peppermodule.cpp


