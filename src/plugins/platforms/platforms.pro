TEMPLATE = subdirs

SUBDIRS += minimal
nacl: SUBDIRS += nacl

contains(QT_CONFIG, wayland) {
    SUBDIRS += wayland
}

