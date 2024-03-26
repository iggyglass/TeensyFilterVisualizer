#ifndef __UTILS_H
#define __UTILS_H

#define __CONCAT_(A,B) A ## B
#define CONCAT(A,B) __CONCAT_(A,B)

typedef uint16_t tft_color_t;

namespace tft
{
    typedef uint16_t color_t;

    inline color_t color565(uint8_t r, uint8_t g, uint8_t b)
    {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
}

#endif
