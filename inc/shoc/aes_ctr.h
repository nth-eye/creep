#ifndef SHOC_AES_CTR_H
#define SHOC_AES_CTR_H

#include "shoc/aes.h"

namespace shoc {

template<size_t L>
inline void ctr_inc_counter(uint8_t *block)
{
    size_t i = 15;
    while (++block[i--] == 0 && i > 15 - L);
}

template<size_t L = 4>
inline bool ctr_encrypt(const uint8_t *key, const uint8_t *iv, const uint8_t *in, uint8_t *out, size_t len)
{
    if (!key || !iv || !in || !out || !len)
        return false;

    uint8_t buf[16];
    uint8_t nonce[16];
    copy(nonce, iv, 16);

    Aes ctx{key};

    auto end = out + len;
    auto idx = 0;
    
    while (out < end) {
        if ((idx &= 0xf) == 0) {
            ctx.encrypt(nonce, buf);
            ctr_inc_counter<L>(nonce);
        }
        *out++ = buf[idx++] ^ *in++;
    }
    return true;       
}

template<size_t L = 4>
inline bool ctr_decrypt(const uint8_t *key, const uint8_t *iv, const uint8_t *in, uint8_t *out, size_t len)
{
    return ctr_encrypt<L>(key, iv, in, out, len);
}

}

#endif