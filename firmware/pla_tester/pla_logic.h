#pragma once
#include <stdint.h>

// Niveles electricos, en el orden de bits documentado en ../../CONEXIONES.md.
inline uint8_t expected(uint16_t vector) {
    const bool a13 = (vector >> 0) & 1U;
    const bool a14 = (vector >> 1) & 1U;
    const bool a15 = (vector >> 2) & 1U;
    const bool va14 = (vector >> 3) & 1U;
    const bool charen = (vector >> 4) & 1U;
    const bool hiram = (vector >> 5) & 1U;
    const bool loram = (vector >> 6) & 1U;
    const bool cas = (vector >> 7) & 1U;
    const bool va12 = (vector >> 8) & 1U;
    const bool va13 = (vector >> 9) & 1U;
    const bool game = (vector >> 10) & 1U;
    const bool exrom = (vector >> 11) & 1U;
    const bool rw = (vector >> 12) & 1U;
    const bool aec = (vector >> 13) & 1U;
    const bool ba = (vector >> 14) & 1U;
    const bool a12 = (vector >> 15) & 1U;
    const bool cpu_read = ((!aec) && rw);
    const bool d_page = (a15 && a14 && (!a13) && a12);
    const bool normal = (game || (!exrom));
    const bool ultimax = (exrom && (!game));
    const bool basic = (loram && hiram && a15 && (!a14) && a13 && cpu_read && game);
    const bool kernal = (hiram && a15 && a14 && a13 && cpu_read && normal);
    const bool char_cpu = ((!charen) && ((hiram && normal) || (loram && game)));
    const bool char_vic = (va14 && aec && normal && (!va13) && va12);
    const bool charom = ((d_page && cpu_read && char_cpu) || char_vic);
    const bool io = (d_page && (!aec) && ((!rw) || ba) && ((charen && (hiram || loram) && normal) || ultimax));
    const bool grw = ((!cas) && d_page && (!aec) && (!rw));
    const bool roml = (a15 && (!a14) && (!a13) && (!aec) && ((loram && hiram && rw && (!exrom)) || ultimax));
    const bool romh = ((hiram && a15 && (!a14) && a13 && cpu_read && (!exrom) && (!game)) || (a15 && a14 && a13 && (!aec) && ultimax) || (aec && ultimax && va13 && va12));
    const bool hole = (ultimax && (((!a15) && (!a14) && (a12 || a13)) || ((!a15) && a14) || (a15 && (!a14) && a13) || (a15 && a14 && (!a13) && (!a12))));
    const bool casram = cas || basic || kernal || charom || io || roml || romh || hole;
    return uint8_t(casram) | (uint8_t(!basic) << 1) | (uint8_t(!kernal) << 2) | (uint8_t(!charom) << 3) | (uint8_t(!grw) << 4) | (uint8_t(!io) << 5) | (uint8_t(!roml) << 6) | (uint8_t(!romh) << 7);
}
