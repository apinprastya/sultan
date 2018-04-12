/*
 * Most of the code here are copied from CUPS
 * https://github.com/apple/cups/blob/master/backend/usb-libusb.c
 * */

#ifndef USB_H
#define USB_H

#include "print_global.h"
#include <QString>
#include <QList>

struct UsbDevice {
    QString name;
    uint16_t vendorId;
    uint16_t productId;
};

struct UsbPrinter {
    bool isFound = false;
    struct libusb_device  *device;  /* Device info */
    int     conf,   /* Configuration */
                origconf, /* Original configuration */
                iface,    /* Interface */
                altset,   /* Alternate setting */
                write_endp, /* Write endpoint */
                read_endp,  /* Read endpoint */
                protocol, /* Protocol: 1 = Uni-di, 2 = Bi-di. */
                usblp_attached, /* "usblp" kernel module attached? */
                reset_after_job;/* Set to 1 by print_device() */
    struct libusb_device_handle *handle;  /* Open handle to device */
    uint16_t vendorId;
    uint16_t produkId;
};

class PRINTSHARED_EXPORT Usb
{
public:
    Usb();
    static void init();
    static void destroy();
    static QList<UsbDevice> getUsbPrinterList(UsbPrinter *printer = nullptr);
    static bool sendData(uint16_t vendorId, uint16_t produkId, const QByteArray &data);
};

#endif // USB_H
