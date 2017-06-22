include(../libglobal/libglobal.pri)
include(../libprint/libprint.pri)
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

DLLDESTDIR = ../

macx {
    DESTDIR = ../bin/Sultan.app/Contents/Frameworks
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
    QMAKE_LIBDIR += $$OUT_PWD/../bin/Sultan.app/Contents/Frameworks
    LIBS += -framework Foundation
    copytr.commands = $$quote(cp -R $${PWD}/libgui_id.qm $${OUT_PWD}/../bin/)
} else:win32 {
    DESTDIR = ../bin
    LIBS += -L$$OUT_PWD/../bin
    PWD_WIN = $${PWD}
    DESTDIR_WIN = $$OUT_PWD/../bin/
    PWD_WIN ~= s,/,\\,g
    DESTDIR_WIN ~= s,/,\\,g
    copytr.commands = $$quote(cmd /c xcopy /S /I /Y $${PWD_WIN}\libgui_id.qm $${DESTDIR_WIN})
} else {
    DESTDIR = ../bin
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
    copytr.commands = $$quote(cp -R $${PWD}/libgui_id.qm $${OUT_PWD}/../bin/)
}

TRANSLATIONS = libgui_id.ts

QMAKE_EXTRA_TARGETS += copytr
POST_TARGETDEPS += copytr

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
    item/itemwidget.cpp \
    item/additemdialog.cpp \
    item/addpricedialog.cpp \
    purchase/purchaseadddialog.cpp \
    purchase/purchaseitemwidget.cpp \
    purchase/purchaseadditemdialog.cpp \
    cashier/paymentcashsuccessdialog.cpp \
    cashier/searchitemdialog.cpp \
    report/saleswidget.cpp \
    statusbar/statusbarwidget.cpp \
    report/reportitemwidget.cpp \
    moneylineedit.cpp \
    flashmessagemanager.cpp \
    flashmessage.cpp \
    cashier/transactionlistdialog.cpp \
    cashier/saveloadslotdialog.cpp \
    cashier/cashierhelpdialog.cpp \
    user/changepassworddialog.cpp \
    customer/customerwidget.cpp \
    customer/customeradddialog.cpp \
    customer/customer.cpp \
    cashier/advancepaymentdialog.cpp \
    customer/customercreditwidget.cpp \
    customer/addcreditpaymentdialog.cpp \
    machine/machinewidget.cpp \
    tilewidget.cpp \
    purchase/purchasepaymentdialog.cpp \
    customer/rewardwidget.cpp \
    customer/rewardadddialog.cpp \
    customer/settingpoinadddialog.cpp \
    customer/customerrewardwidget.cpp \
    customer/addpoindialog.cpp \
    bank/bankwidget.cpp \
    bank/bankadddialog.cpp \
    cashier/paycashlessdialog.cpp

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
    item/itemwidget.h \
    item/additemdialog.h \
    item/addpricedialog.h \
    purchase/purchaseadddialog.h \
    purchase/purchaseitemwidget.h \
    purchase/purchaseadditemdialog.h \
    cashier/paymentcashsuccessdialog.h \
    cashier/searchitemdialog.h \
    report/saleswidget.h \
    statusbar/statusbarwidget.h \
    report/reportitemwidget.h \
    moneylineedit.h \
    flashmessagemanager.h \
    flashmessage.h \
    cashier/transactionlistdialog.h \
    cashier/saveloadslotdialog.h \
    cashier/cashierhelpdialog.h \
    user/changepassworddialog.h \
    customer/customerwidget.h \
    customer/customeradddialog.h \
    customer/customer.h \
    cashier/advancepaymentdialog.h \
    customer/customercreditwidget.h \
    customer/addcreditpaymentdialog.h \
    machine/machinewidget.h \
    tilewidget.h \
    purchase/purchasepaymentdialog.h \
    customer/rewardwidget.h \
    customer/rewardadddialog.h \
    customer/settingpoinadddialog.h \
    customer/customerrewardwidget.h \
    customer/addpoindialog.h \
    bank/bankwidget.h \
    bank/bankadddialog.h \
    cashier/paycashlessdialog.h

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
    item/itemwidget.ui \
    item/additemdialog.ui \
    item/addpricedialog.ui \
    purchase/purchaseadddialog.ui \
    purchase/purchaseadditemdialog.ui \
    cashier/paymentcashsuccessdialog.ui \
    cashier/searchitemdialog.ui \
    statusbar/statusbarwidget.ui \
    report/saleswidget.ui \
    cashier/transactionlistdialog.ui \
    cashier/saveloadslotdialog.ui \
    cashier/cashierhelpdialog.ui \
    user/changepassworddialog.ui \
    customer/customeradddialog.ui \
    cashier/advancepaymentdialog.ui \
    customer/creditsummarywidget.ui \
    customer/addcreditpaymentdialog.ui \
    tilewidget.ui \
    purchase/purchasepaymentdialog.ui \
    customer/rewardadddialog.ui \
    customer/rewardsetting.ui \
    customer/settingpoinadddialog.ui \
    customer/addpoindialog.ui \
    bank/bankadddialog.ui \
    cashier/paycashlessdialog.ui
