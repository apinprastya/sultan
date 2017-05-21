#ifndef PRINT_CONSTANT_H
#define PRINT_CONSTANT_H

namespace LibPrint {

namespace BORDER {
    /*const char TOP_LEFT          = 0xDA;
    const char TOP_MIDDLE        = 0xCA;
    const char TOP_RIGHT         = 0xBF;
    const char RIGHT_MIDDLE      = 0xB4;
    const char BOTTOM_RIGHT      = 0xD9;
    const char BOTTOM_MIDDLE     = 0xC1;
    const char BOTTOM_LEFT       = 0xC0;
    const char LEFT_MIDDLE       = 0xC3;
    const char VERTICAL          = 0xB3;
    const char HORIZONTAL        = 0xC4;
    const char MIDDLE            = 0xC5;*/
    const char TOP_LEFT          = '-';
    const char TOP_MIDDLE        = '-';
    const char TOP_RIGHT         = '-';
    const char RIGHT_MIDDLE      = '-';
    const char BOTTOM_RIGHT      = '-';
    const char BOTTOM_MIDDLE     = '-';
    const char BOTTOM_LEFT       = '-';
    const char LEFT_MIDDLE       = '-';
    const char VERTICAL          = '|';
    const char HORIZONTAL        = '-';
    const char MIDDLE            = '-';
}

namespace ESCP {
    const char ESCP                     = 0x1B;
    const char LINE_FEED                = 0xA;
}

}

#endif // PRINT_CONSTANT_H
