/*
 * Most of the code here are copied from CUPS
 * https://github.com/apple/cups/blob/master/backend/usb-libusb.c
 * */

#include "usb.h"
#ifdef USE_LIBUSB
#include <libusb-1.0/libusb.h>
#endif

#include <QDebug>

int open_device(UsbPrinter *printer) {
#ifdef USE_LIBUSB
    struct libusb_device_descriptor devdesc;
    struct libusb_config_descriptor *confptr = NULL;
    int number1 = -1, errcode = 0;
    char  current;

    printer->usblp_attached = 0;
    if (printer->handle == nullptr)
        return 1;
    libusb_get_device_descriptor(printer->device, &devdesc);
    errcode = libusb_kernel_driver_active(printer->handle, printer->iface);
    if (errcode == 0)
        printer->usblp_attached = 0;
    else if (errcode == 1)
    {
        printer->usblp_attached = 1;
        libusb_detach_kernel_driver(printer->handle, printer->iface);
    }
    else
    {
        printer->usblp_attached = 0;
        if (errcode != LIBUSB_ERROR_NOT_SUPPORTED)
        {
            qDebug() << "Usb printer not supported";
            return 1;
        }
    }
    if (libusb_control_transfer(printer->handle,
                                LIBUSB_REQUEST_TYPE_STANDARD | LIBUSB_ENDPOINT_IN |
                                LIBUSB_RECIPIENT_DEVICE,
                                8, /* GET_CONFIGURATION */
                                0, 0, (unsigned char *)&current, 1, 5000) < 0)
        current = 0;

    printer->origconf = current;

    libusb_get_config_descriptor (printer->device, printer->conf, &confptr);

    number1 = confptr->bConfigurationValue;

    if (number1 != current)
        libusb_set_configuration(printer->handle, number1);

    number1 = confptr->interface[printer->iface].
              altsetting[printer->altset].bInterfaceNumber;

    libusb_claim_interface(printer->handle, number1);

    libusb_free_config_descriptor(confptr);
#endif
    return 0;
}

void close_device(UsbPrinter *printer) {
#ifdef USE_LIBUSB
    if(printer->usblp_attached) {
        libusb_attach_kernel_driver(printer->handle, printer->iface);
    }
    libusb_close(printer->handle);
#endif
}

Usb::Usb()
{

}

void Usb::init()
{
#ifdef USE_LIBUSB
    libusb_init(NULL);
#endif
}

void Usb::destroy()
{
#ifdef USE_LIBUSB
    libusb_exit(NULL);
#endif
}

QList<UsbDevice> Usb::getUsbPrinterList(UsbPrinter *printer)
{
    QList<UsbDevice> list;
#ifdef USE_LIBUSB
    libusb_device **devices;
    libusb_device *device = NULL; /* Current device */
    struct libusb_device_descriptor devdesc;
    struct libusb_device_handle *handle = NULL;
    struct libusb_config_descriptor *confptr = NULL;
    const struct libusb_interface *ifaceptr = NULL;
    const struct libusb_interface_descriptor *altptr = NULL;
    const struct libusb_endpoint_descriptor *endpptr = NULL;
    uint8_t   conf,   /* Current configuration */
                    iface,    /* Current interface */
                    altset,   /* Current alternate setting */
                    protocol, /* Current protocol */
                    endp,   /* Current endpoint */
                    read_endp,  /* Current read endpoint */
                    write_endp; /* Current write endpoint */

    int ret = libusb_get_device_list(NULL, &devices);
    for(int i = 0; i < ret; i++) {
        unsigned char manufacture[200];
        device = devices[i];

        if (libusb_get_device_descriptor(device, &devdesc) < 0)
            continue;

        if (!devdesc.bNumConfigurations || !devdesc.idVendor ||
                !devdesc.idProduct)
            continue;

        if(printer != nullptr) {
            if(devdesc.idProduct != printer->produkId && devdesc.idVendor != printer->vendorId) continue;
            printer->device = device;
            printer->isFound = true;
        }

        for (conf = 0; conf < devdesc.bNumConfigurations; conf++) {
            if (libusb_get_config_descriptor(device, conf, &confptr) < 0)
                continue;
            for (iface = 0, ifaceptr = confptr->interface;
                    iface < confptr->bNumInterfaces;
                    iface ++, ifaceptr ++)
            {
                protocol   = 0;
                for (altset = 0, altptr = ifaceptr->altsetting;
                        altset < ifaceptr->num_altsetting;
                        altset ++, altptr ++)
                {
                    if (((altptr->bInterfaceClass != LIBUSB_CLASS_PRINTER ||
                            altptr->bInterfaceSubClass != 1)) ||
                            (altptr->bInterfaceProtocol != 1 && /* Unidirectional */
                             altptr->bInterfaceProtocol != 2) ||  /* Bidirectional */
                            altptr->bInterfaceProtocol < protocol) {
                        continue;
                    }
                    read_endp  = 0xff;
                    write_endp = 0xff;

                    for (endp = 0, endpptr = altptr->endpoint;
                            endp < altptr->bNumEndpoints;
                            endp ++, endpptr ++)
                        if ((endpptr->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) ==
                                LIBUSB_TRANSFER_TYPE_BULK)
                        {
                            if (endpptr->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
                                read_endp = endp;
                            else
                                write_endp = endp;
                        }
                    if (write_endp != 0xff) {
                        protocol = altptr->bInterfaceProtocol;
                        if(printer != nullptr) {
                            printer->altset     = altset;
                            printer->write_endp = write_endp;
                            if (protocol > 1)
                                printer->read_endp = read_endp;
                            else
                                printer->read_endp = -1;
                        }
                    }
                }

                if (protocol > 0)
                {
                    if(printer != nullptr) {
                        printer->conf = conf;
                        printer->iface = iface;
                        printer->protocol = protocol;
                        printer->handle = nullptr;
                    }
                    if (!libusb_open(device, &handle))
                    {
                        UsbDevice d;
                        d.productId = devdesc.idProduct;
                        d.vendorId = devdesc.idVendor;
                        libusb_get_string_descriptor_ascii(handle, devdesc.iProduct, manufacture, 200);
                        d.name = QString::fromLatin1((const char*)manufacture);
                        list.append(d);

                        if(printer != nullptr) {
                            printer->handle = handle;
                            open_device(printer);
                            printer->write_endp = confptr->interface[printer->iface].
                                                 altsetting[printer->altset].
                                                 endpoint[printer->write_endp].
                                                 bEndpointAddress;
                        }
                        return list;
                        libusb_close(handle);
                    }
                }
                //TODO : this code is caused crash, usb bad keyboard, this will cause memory leak
                //just let it be for now, unless application not open setting over and over
                //libusb_free_config_descriptor(confptr);
            }
        }
    }
#endif
    return list;
}

bool Usb::sendData(uint16_t vendorId, uint16_t produkId, const QByteArray &data)
{
#ifdef USE_LIBUSB
    UsbPrinter printer;
    printer.produkId = produkId;
    printer.vendorId = vendorId;
    getUsbPrinterList(&printer);
    if(printer.isFound) {
        int act = 0;
        open_device(&printer);
        libusb_bulk_transfer(printer.handle, printer.write_endp, (unsigned char*)data.data(), data.size(), &act, 0);
        close_device(&printer);
    } else {
        return false;
    }
#endif
}
