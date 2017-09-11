TEMPLATE  = subdirs
CONFIG   += ordered

!contains(CONFIG, SINGLEBIN) {
    SUBDIRS += \
        libglobal \
        libdb \
        libserver
    !contains(CONFIG, SERVER_BUILD) {
        SUBDIRS += \
            libprint \
            libgui \
            libguiqml
    }
}

SUBDIRS += sultan
