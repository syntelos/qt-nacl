# DEFINES += QT_PEPPER_USE_PEPPER_FONT_ENGINE
DEFINES += QT_PEPPER_DELAY_GRAPHICSCONTEXT_CREATION

TARGET     = QtNacl

include($$PWD/../../../qbase.pri)
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
           $$PWD/qpepperintegration.cpp \
           $$PWD/qpepperplatformwindow.cpp \
           $$PWD/qpepperwindowsurface.cpp \
           $$PWD/qpepperfontengine.cpp \
           $$PWD/qpepperglcontext.cpp \
           $$PWD/qpepperhelpers.cpp \
           $$PWD/qpeppercompositor.cpp \

HEADERS += $$PWD/peppereventtranslator.h \
           $$PWD/qpepperintegration.h \
           $$PWD/qpepperplatformwindow.h \
           $$PWD/qpepperwindowsurface.h \
           $$PWD/pepperinstance.h \
           $$PWD/qpepperfontdatabase.h \
           $$PWD/qpeppermain.h \
           $$PWD/qpepperscreen.h \
           $$PWD/qpepperfontengine.h \
           $$PWD/qpepperglcontext.h \
           $$PWD/qpepperhelpers.h \
           $$PWD/qpeppercompositor.h \

RESOURCES += fonts.qrc

OTHER_FILES += $$PWD/../../../../tools/nacldemoserver/check_browser.js \
               $$PWD/../../../../tools/nacldemoserver/qtnaclloader.js

CONFIG += static
LIBS += -lppapi -lppapi_cpp -lppapi_gles2 -lsrpc -limc_syscalls -lplatform -lgio
OBJECTS_DIR = .obj
MOC_DIR = .moc
TEMPLATE = lib

target.path += $$[QT_INSTALL_LIBS]
INSTALLS += target


