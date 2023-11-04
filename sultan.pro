TEMPLATE  = subdirs
CONFIG   += ordered

!contains(CONFIG, SINGLEBIN) {
    SUBDIRS += \
        libglobal \
        libdb \
        libserver \
        unittest
    !contains(CONFIG, SERVER_BUILD) {
        SUBDIRS += \
            libprint \
            libgui \
    }
}

SUBDIRS += sultan
