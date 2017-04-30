include(config.pri)

TEMPLATE = subdirs
SUBDIRS = pillowcore tests examples

tests.depends = pillowcore
examples.depends = pillowcore

OTHER_FILES += README pillow.qbs
