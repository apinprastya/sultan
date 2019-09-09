include(../external_library/o2/src/src.pri)
include(../libglobal/libglobal.pri)
include(../libprint/libprint.pri)
include(../libdb/libdb.pri)

TEMPLATE = lib
TARGET = gui
target.path = $${LIBDIR}
INSTALLS += target

QT += core gui widgets network sql concurrent

CONFIG(USE_EMBED_BROWSER) {
    DEFINES+=USE_EMBED_BROWSER
    CONFIG(USE_WEBENGINE) {
        QT += webenginewidgets
        DEFINES += USE_WEBENGINE
    } else {
        QT += webkitwidgets
    }
}

CONFIG(NO_PRINTER_SPOOL) {
    DEFINES+=NO_PRINTER_SPOOL
}

CONFIG(NO_PRINTER_DEVICE) {
    DEFINES+=NO_PRINTER_DEVICE
}

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
} else:win32 {
    DESTDIR = ../bin
    LIBS += -L$$OUT_PWD/../bin
} else {
    DESTDIR = ../bin
    QMAKE_LIBDIR = $$OUT_PWD/../bin $$QMAKE_LIBDIR
    contains(CONFIG, USE_LIBUSB) {
        DEFINES+=USE_LIBUSB
        LIBS += -lusb-1.0
    }
}

TRANSLATIONS = ../translation/libgui_id.ts

SOURCES += \
    item/importitemdialog.cpp \
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
    cashier/paycashlessdialog.cpp \
    transaction/transactionwidget.cpp \
    transaction/moneywidget.cpp \
    cashier/solditemlistdialog.cpp \
    cashier/returnitemadddialog.cpp \
    about/autoupdatedialog.cpp \
    transaction/addtransactiondialog.cpp \
    purchase/purchasereturnwidget.cpp \
    purchase/returnadddialog.cpp \
    purchase/purchaseitemselectiondialog.cpp \
    purchase/purchaseitem.cpp \
    purchase/returnresolutiondialog.cpp \
    cashier/checkpricedialog.cpp \
    admin/importexportdatabasedialog.cpp \
    admin/browserdialog.cpp \
    googledrive.cpp \
    admin/listdialog.cpp \
    checkstock/checkstockwidget.cpp \
    checkstock/checkstockadddialog.cpp \
    initialstock/initialstockwidget.cpp \
    initialstock/initialstockadddialog.cpp \
    unit/unitwidget.cpp \
    setting/datesettingdialog.cpp \
    cashier/searchcustomerdialog.cpp \
    main/splash.cpp \
    main/logindialog.cpp \
    main/restartconfirmationdialog.cpp \
    main/settingdialog.cpp \
    cashier/editpricecountdialog.cpp \
    report/stockcardwidget.cpp \
    item/stockcarddialog.cpp \
    soldreturn/solditemreturnwidget.cpp \
    soldreturn/addsoldreturndialog.cpp \
    item/restoreitemdialog.cpp \
    machine/addmachinedialog.cpp \
    cashier/cashierreportdialog.cpp \
    cashier/customercreditpaymentdialog.cpp \
    item/addingridientdialog.cpp \
    cashier/additemunavailabledialog.cpp \
    doublespinboxdelegate.cpp

HEADERS += \
    gui_global.h \
    item/importitemdialog.h \
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
    cashier/paycashlessdialog.h \
    transaction/transactionwidget.h \
    transaction/moneywidget.h \
    cashier/solditemlistdialog.h \
    cashier/returnitemadddialog.h \
    about/autoupdatedialog.h \
    transaction/addtransactiondialog.h \
    purchase/purchasereturnwidget.h \
    purchase/returnadddialog.h \
    purchase/purchaseitemselectiondialog.h \
    purchase/purchaseitem.h \
    purchase/returnresolutiondialog.h \
    cashier/checkpricedialog.h \
    admin/importexportdatabasedialog.h \
    admin/browserdialog.h \
    googledrive.h \
    admin/listdialog.h \
    checkstock/checkstockwidget.h \
    checkstock/checkstockadddialog.h \
    initialstock/initialstockwidget.h \
    initialstock/initialstockadddialog.h \
    unit/unitwidget.h \
    setting/datesettingdialog.h \
    cashier/searchcustomerdialog.h \
    main/splash.h \
    main/logindialog.h \
    main/restartconfirmationdialog.h \
    main/settingdialog.h \
    cashier/editpricecountdialog.h \
    report/stockcardwidget.h \
    item/stockcarddialog.h \
    soldreturn/solditemreturnwidget.h \
    soldreturn/addsoldreturndialog.h \
    item/restoreitemdialog.h \
    machine/addmachinedialog.h \
    cashier/cashierreportdialog.h \
    cashier/customercreditpaymentdialog.h \
    tabcloseablewidget.h \
    item/addingridientdialog.h \
    cashier/additemunavailabledialog.h \
    doublespinboxdelegate.h

FORMS += \
    item/importitemdialog.ui \
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
    cashier/paycashlessdialog.ui \
    cashier/solditemlistdialog.ui \
    cashier/returnitemadddialog.ui \
    about/autoupdatedialog.ui \
    transaction/addtransactiondialog.ui \
    purchase/returnadddialog.ui \
    purchase/purchaseitemselectiondialog.ui \
    purchase/returnresolutiondialog.ui \
    cashier/checkpricedialog.ui \
    admin/importexportdatabasedialog.ui \
    admin/browserdialog.ui \
    admin/listdialog.ui \
    checkstock/checkstockadddialog.ui \
    initialstock/initialstockadddialog.ui \
    setting/datesettingdialog.ui \
    cashier/searchcustomerdialog.ui \
    main/splash.ui \
    main/logindialog.ui \
    main/restartconfirmationdialog.ui \
    main/settingdialog.ui \
    cashier/editpricecountdialog.ui \
    item/stockcarddialog.ui \
    soldreturn/addsoldreturndialog.ui \
    item/restoreitemdialog.ui \
    machine/addmachinedialog.ui \
    cashier/cashierreportdialog.ui \
    cashier/customercreditpaymentdialog.ui \
    item/addingridientdialog.ui \
    report/datefromtowidget.ui \
    cashier/additemunavailabledialog.ui
