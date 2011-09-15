include($$PWD/../../qpluginbase.pri)
include($$PWD/../fontdatabases/genericunix/genericunix.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += $$PWD/lighthousepluginmain.cpp \
           $$PWD/pepperinstance.cpp \
           $$PWD/qpepperfontdatabase.cpp \
           $$PWD/qpeppermain.cpp \
           $$PWD/qpepperscreen.cpp \
           $$PWD/peppereventtranslator.cpp \
           $$PWD/peppermodule.cpp \
           $$PWD/qpepperintegration.cpp \
           $$PWD/qpepperplatformwindow.cpp \
           $$PWD/qpepperwindowsurface.cpp \
           $$PWD/qpepperfontengine.cpp \
           $$PWD/qpepperglcontext.cpp \

HEADERS += $$PWD/peppereventtranslator.h \
           $$PWD/peppermodule.h \
           $$PWD/qpepperintegration.h \
           $$PWD/qpepperplatformwindow.h \
           $$PWD/qpepperwindowsurface.h \
           $$PWD/pepperinstance.h \
           $$PWD/qpepperfontdatabase.h \
           $$PWD/qpeppermain.h \
           $$PWD/qpepperscreen.h \
           $$PWD/qpepperfontengine.h \
           $$PWD/qpepperglcontext.h \

RESOURCES += fonts.qrc

CONFIG += static
LIBS +=  -lppruntime -lppapi_cpp -lsrpc -limc_syscalls -lplatform -lgio -lppapi_gles2 -m32
OBJECTS_DIR = .obj/debug-static
MOC_DIR = .moc/debug-static

