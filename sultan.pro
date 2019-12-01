TEMPLATE  = subdirs
CONFIG   += ordered

!contains(CONFIG, SINGLEBIN) {
    SUBDIRS += \
        external_library/pillow/pillowcore \
        libglobal \
        libdb \
        libserver
    !contains(CONFIG, SERVER_BUILD) {
        SUBDIRS += \
            libprint \
            libgui \
    }
}

SUBDIRS += sultan
