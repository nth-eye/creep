#ifndef SHOC_AES_CBC_MAC_H
#define SHOC_AES_CBC_MAC_H

#include "shoc/aes.h"

namespace shoc {

inline size_t cbc_mac(Aes &ctx, uint8_t *buf, const uint8_t *aad, size_t aad_len, size_t pos)
{
    auto end = aad + aad_len;

    while (aad != end) {
        buf[pos] ^= *aad++;
        if (pos == 15)
            ctx.encrypt(buf, buf);
        pos = (pos + 1) & 0xf;
    }
    return pos;
}

inline void cbc_mac_padded(Aes &ctx, uint8_t *buf, const uint8_t *aad, size_t aad_len, size_t start)
{
    if (size_t i = cbc_mac(ctx, buf, aad, aad_len, start)) {
        for (; i < 16; ++i)
            buf[i] ^= 0;
        ctx.encrypt(buf, buf);
    }
}

}

#endif