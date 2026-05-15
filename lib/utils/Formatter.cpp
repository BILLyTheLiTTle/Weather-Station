#include "Formatter.h"

void formatDateTime(char* buffer, uint8_t d, uint8_t m, uint16_t y, uint8_t h, uint8_t min) {
    buffer[0] = (d / 10) + '0';
    buffer[1] = (d % 10) + '0';
    buffer[2] = '/';

    buffer[3] = (m / 10) + '0';
    buffer[4] = (m % 10) + '0';
    buffer[5] = '/';

    IT_TO_STR(y, buffer + 6); 
    
    buffer[10] = ' ';

    buffer[11] = (h / 10) + '0';
    buffer[12] = (h % 10) + '0';
    buffer[13] = ':';

    buffer[14] = (min / 10) + '0';
    buffer[15] = (min % 10) + '0';

    buffer[16] = '\0';
}