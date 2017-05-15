include(../external_library/easyloggingpp/easyloggingpp/easyloggingpp.pri)
include(../libglobal/libglobal.pri)
include(../libdb/libdb.pri)

TEMPLATE = lib
TARGET = gui
target.path = $${LIBDIR}
INSTALLS += target

QT += core gui widgets

CONFIG(static) {
    CONFIG += staticlib
}

CONFIG += c++11

CONFIG(staticlib) {
    DEFINES += GUI_LIBRARY_STATIC
} else {
    DEFINES += GUI_LIBRARY
}
contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols

macx {
    DESTDIR = ../bin/Sultan.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
} else {
    DESTDIR = ../bin
}
DLLDESTDIR = ../

macx {
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
    LIBS += -framework Foundation
} else:win32 {
    LIBS += -L$$OUT_PWD/../bin
} else {
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
}


SOURCES += \
    mainwindow.cpp \
    tabwidget.cpp \
    setting/settingwidget.cpp \
    user/userwidget.cpp \
    horizontalheader.cpp \
    headerwidget.cpp \
    tableview.cpp \
    tablewidget.cpp \
    tablemodel.cpp \
    tableitem.cpp \
    suplier/suplierwidget.cpp \
    rowdata.cpp \
    suplier/suplieradddialog.cpp \
    guiutil.cpp \
    user/useradddialog.cpp \
    about/aboutdialog.cpp \
    cashier/cashierwidget.cpp \
    cashier/cashieritem.cpp \
    cashier/cashiertablemodel.cpp \
    keyevent.cpp \
    user/userpermissiondialog.cpp \
    cashier/paycashdialog.cpp \
    category/categorywidget.cpp \
    purchase/purchasewidget.cpp \
    category/categorytreewidget.cpp \
    category/categotyadddialog.cpp \
    item/itemwidget.cpp

HEADERS += \ 
    gui_global.h \
    mainwindow.h \
    tabwidget.h \
    setting/settingwidget.h \
    user/userwidget.h \
    horizontalheader.h \
    headerwidget.h \
    tableview.h \
    tablewidget.h \
    tablemodel.h \
    tableitem.h \
    suplier/suplierwidget.h \
    rowdata.h \
    suplier/suplieradddialog.h \
    guiutil.h \
    user/useradddialog.h \
    about/aboutdialog.h \
    cashier/cashierwidget.h \
    cashier/cashieritem.h \
    cashier/cashiertablemodel.h \
    keyevent.h \
    user/userpermissiondialog.h \
    cashier/paycashdialog.h \
    category/categorywidget.h \
    purchase/purchasewidget.h \
    category/categorytreewidget.h \
    category/categotyadddialog.h \
    item/itemwidget.h

FORMS += \
    mainwindow.ui \
    setting/settingwidget.ui \
    normalwidget.ui \
    suplier/suplieradddialog.ui \
    user/useradddialog.ui \
    about/aboutdialog.ui \
    cashier/cashierwidget.ui \
    user/userpermissiondialog.ui \
    cashier/paycashdialog.ui \
    category/categoryadddialog.ui \
    item/itemwidget.ui
