#ifndef SHOC_CIPHER_AES_H
#define SHOC_CIPHER_AES_H

#include "shoc/util.h"

#define SHOC_AES_EXP true

namespace shoc {
namespace impl::aes {

using word = uint32_t;

inline constexpr byte sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16, 
};
inline constexpr byte rsbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};
inline constexpr word rconst[11] = {
    0x00000000, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
    0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000
};

constexpr auto subbyte(word x, int o)   { return sbox[(x >> o) & 0xff] << o; }
constexpr auto subword(word x)          { return subbyte(x, 24) | subbyte(x, 16) | subbyte(x, 8) | subbyte(x, 0); }
constexpr auto rotword(word x)          { return (x >> 24) | (x << 8); }
constexpr auto gf_x(byte x)             { return (x << 1) ^ ((x >> 7) * 0x1b); }
constexpr auto gf_mul(byte x, byte y) 
{
    byte r = 0; 
    while (y) {
        if (y & 1)
            r ^= x; 
        x = gf_x(x);
        y >>= 1;
    }
    return r;
}
constexpr void mult_row_col(const byte* in, byte* out)
{
    out[0] = gf_x(in[0])        ^ gf_mul(in[1], 3)  ^ in[2]             ^ in[3];
    out[1] = in[0]              ^ gf_x(in[1])       ^ gf_mul(in[2], 3)  ^ in[3];
    out[2] = in[0]              ^ in[1]             ^ gf_x(in[2])       ^ gf_mul(in[3], 3);
    out[3] = gf_mul(in[0], 3)   ^ in[1]             ^ in[2]             ^ gf_x(in[3]);
}
constexpr auto inv_mult_row_col(const byte* in, byte* out)
{
    out[0] = gf_mul(in[0], 0xe) ^ gf_mul(in[1], 0xb) ^ gf_mul(in[2], 0xd) ^ gf_mul(in[3], 0x9);
    out[1] = gf_mul(in[0], 0x9) ^ gf_mul(in[1], 0xe) ^ gf_mul(in[2], 0xb) ^ gf_mul(in[3], 0xd);
    out[2] = gf_mul(in[0], 0xd) ^ gf_mul(in[1], 0x9) ^ gf_mul(in[2], 0xe) ^ gf_mul(in[3], 0xb);
    out[3] = gf_mul(in[0], 0xb) ^ gf_mul(in[1], 0xd) ^ gf_mul(in[2], 0x9) ^ gf_mul(in[3], 0xe);
}

enum type {
    type_128,
    type_192,
    type_256,
};

template<type T = type_128>
class context {
    static constexpr size_t nk = 4 + 2 * T;
    static constexpr size_t nb = 4;
    static constexpr size_t nr = 10 + 2 * T;
public:
    static constexpr size_t state_size  = nb * 4;
    static constexpr size_t block_size  = nb * 4;
    static constexpr size_t key_size    = nk * sizeof(word);
public:
    constexpr context() = default;
    constexpr context(span_i<key_size> key) { init(key); }
    constexpr ~context()                    { deinit(); }
public:
    constexpr void init(span_i<key_size> key);
    constexpr void deinit();
    constexpr void encrypt(span_i<block_size> in, span_o<block_size> out);
    constexpr void decrypt(span_i<block_size> in, span_o<block_size> out);
private:
#if (SHOC_AES_EXP)
    constexpr void add_round_key(span_i<nb, word> key);
#else
    constexpr void add_round_key(const word* key);
#endif
    constexpr void sub_bytes();
    constexpr void shift_rows();
    constexpr void mix_columns();
    constexpr void inv_sub_bytes();
    constexpr void inv_shift_rows();
    constexpr void inv_mix_columns();
private:
    byte state[nb * 4] = {};
#if (SHOC_AES_EXP)
    word words[nr + 1][nb] = {};
#else
    word words[nb * (nr + 1)] = {};
#endif
};

#if (SHOC_AES_EXP)

template<type T> 
constexpr void context<T>::init(span_i<key_size> key)
{
    size_t i = 0;

    for (auto j = 0; j < nb; ++j) {
        words[i][j] = 
            (key[nb * i + 0] << 24) | 
            (key[nb * i + 1] << 16) |
            (key[nb * i + 2] << 8 ) | 
            (key[nb * i + 3]);
    }
    for (i = 0; i < nk; ++i) {
        words_flat[i] = 
            (key[nb * i + 0] << 24) | 
            (key[nb * i + 1] << 16) |
            (key[nb * i + 2] << 8 ) | 
            (key[nb * i + 3]);
    }
    for (; i < nb * (nr + 1); ++i) {
        auto tmp = words_flat[i - 1];
        if (i % nk == 0) {
            tmp = subword(rotword(tmp)) ^ rconst[i / nk];
        } else if (nk > 6 && i % nk == 4) {
            tmp = subword(tmp);
        }
        words_flat[i] = words_flat[i - nk] ^ tmp;
    }
}

#else

template<type T> 
constexpr void context<T>::init(span_i<key_size> key)
{
    size_t i = 0;

    for (i = 0; i < nk; ++i) {
        words[i] = 
            (key[nb * i + 0] << 24) | 
            (key[nb * i + 1] << 16) |
            (key[nb * i + 2] << 8 ) | 
            (key[nb * i + 3]);
    }
    for (; i < nb * (nr + 1); ++i) {
        auto tmp = words[i - 1];
        if (i % nk == 0) {
            tmp = subword(rotword(tmp)) ^ rconst[i / nk];
        } else if (nk > 6 && i % nk == 4) {
            tmp = subword(tmp);
        }
        words[i] = words[i - nk] ^ tmp;
    }
}

#endif

template<type T> 
constexpr void context<T>::deinit()
{
    if (std::is_constant_evaluated())
        zero(state, sizeof(state));
    else
        zero(this, sizeof(*this));
}

#if (SHOC_AES_EXP)

template<type T> 
constexpr void context<T>::encrypt(span_i<block_size> in, span_o<block_size> out)
{
    copy(state, in.data(), sizeof(state));

    add_round_key(words[0]);
    for (auto round = 1u; round < nr; ++round) {
        sub_bytes();
        shift_rows();
        mix_columns();
        add_round_key(words[round]);
    }
    sub_bytes();
    shift_rows();
    add_round_key(words[nr]);
    
    copy(out.data(), state, sizeof(state));
    zero(state, sizeof(state));
}

template<type T> 
constexpr void context<T>::decrypt(span_i<block_size> in, span_o<block_size> out)
{
    copy(state, in.data(), sizeof(state));

    add_round_key(words[nr]);
    for (auto round = nr - 1; round > 0; --round) {
        inv_shift_rows();
        inv_sub_bytes();
        add_round_key(words[round]);
        inv_mix_columns();
    }
    inv_shift_rows();
    inv_sub_bytes();
    add_round_key(words[0]);
    
    copy(out.data(), state, sizeof(state));
    zero(state, sizeof(state));
}

#else

template<type T> 
constexpr void context<T>::encrypt(span_i<block_size> in, span_o<block_size> out)
{
    copy(state, in.data(), sizeof(state));

    add_round_key(&words[nb * 0]);
    for (auto round = 1u; round < nr; ++round) {
        sub_bytes();
        shift_rows();
        mix_columns();
        add_round_key(&words[nb * round]);
    }
    sub_bytes();
    shift_rows();
    add_round_key(&words[nb * nr]);
    
    copy(out.data(), state, sizeof(state));
    zero(state, sizeof(state));
}

template<type T> 
constexpr void context<T>::decrypt(span_i<block_size> in, span_o<block_size> out)
{
    copy(state, in.data(), sizeof(state));

    add_round_key(&words[nb * nr]);
    for (auto round = nr - 1; round > 0; --round) {
        inv_shift_rows();
        inv_sub_bytes();
        add_round_key(&words[nb * round]);
        inv_mix_columns();
    }
    inv_shift_rows();
    inv_sub_bytes();
    add_round_key(&words[nb * 0]);
    
    copy(out.data(), state, sizeof(state));
    zero(state, sizeof(state));
}

#endif

template<type T>
#if (SHOC_AES_EXP) 
constexpr void context<T>::add_round_key(span_i<nb, word> key)
#else
constexpr void context<T>::add_round_key(const word *key)
#endif
{
    state[0]    ^= key[0] >> 24; 
    state[1]    ^= key[0] >> 16;
    state[2]    ^= key[0] >> 8; 
    state[3]    ^= key[0];
    state[4]    ^= key[1] >> 24; 
    state[5]    ^= key[1] >> 16;
    state[6]    ^= key[1] >> 8; 
    state[7]    ^= key[1];
    state[8]    ^= key[2] >> 24; 
    state[9]    ^= key[2] >> 16;
    state[10]   ^= key[2] >> 8; 
    state[11]   ^= key[2];
    state[12]   ^= key[3] >> 24; 
    state[13]   ^= key[3] >> 16;
    state[14]   ^= key[3] >> 8; 
    state[15]   ^= key[3];
}

template<type T> 
constexpr void context<T>::sub_bytes()
{
    for (auto &s : state)
        s = sbox[s];
}

template<type T> 
constexpr void context<T>::shift_rows()
{   
    byte tmp[nb * nk] = {
        state[0], state[5], state[10], state[15], 
        state[4], state[9], state[14], state[3],
        state[8], state[13], state[2], state[7],
        state[12], state[1], state[6], state[11],
    };
    copy(state, tmp, sizeof(state));
}

template<type T> 
constexpr void context<T>::mix_columns()
{
    byte tmp[nb * nk] = {};

    mult_row_col(&state[nb * 0], &tmp[nb * 0]);
    mult_row_col(&state[nb * 1], &tmp[nb * 1]);
    mult_row_col(&state[nb * 2], &tmp[nb * 2]);
    mult_row_col(&state[nb * 3], &tmp[nb * 3]);

    copy(state, tmp, sizeof(state));
}

template<type T> 
constexpr void context<T>::inv_sub_bytes()
{
    for (auto &s : state)
        s = rsbox[s];
}

template<type T> 
constexpr void context<T>::inv_shift_rows()
{   
    byte tmp[nb * nk] = {
        state[0], state[13], state[10], state[7], 
        state[4], state[1], state[14], state[11],
        state[8], state[5], state[2], state[15],
        state[12], state[9], state[6], state[3],
    };
    copy(state, tmp, sizeof(state));
}

template<type T> 
constexpr void context<T>::inv_mix_columns()
{
    byte tmp[nb * nk] = {};

    inv_mult_row_col(&state[nb * 0], &tmp[nb * 0]);
    inv_mult_row_col(&state[nb * 1], &tmp[nb * 1]);
    inv_mult_row_col(&state[nb * 2], &tmp[nb * 2]);
    inv_mult_row_col(&state[nb * 3], &tmp[nb * 3]);

    copy(state, tmp, sizeof(state));
}

}

using aes128 = impl::aes::context<impl::aes::type_128>;
using aes192 = impl::aes::context<impl::aes::type_192>;
using aes256 = impl::aes::context<impl::aes::type_256>;

}

#endif
