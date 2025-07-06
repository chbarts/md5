#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint32_t s[64] = 
                 { 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
                   5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
                   4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
                   6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };

static uint32_t K[64] = 
                 { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                   0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                   0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                   0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                   0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                   0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                   0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                   0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                   0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                   0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                   0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                   0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                   0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                   0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                   0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                   0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

static uint32_t leftrotate(uint32_t in, uint32_t amt)
{
    uint64_t tmp = ((uint64_t)in) << amt;
    return ((in << amt) | ((uint32_t)(tmp >> 32)));
}

static void makechunk(uint8_t *input, uint64_t off, uint32_t *output)
{
    int j;
    size_t i;
    uint32_t tmp = 0;

    for (j = 0; j < 16; j++) {
        for (i = 0; i < 4; i++) {
            tmp |= (input[i+off] & 0xFF) << (i * 8);
        }

        output[j] = tmp;
        tmp = 0;
        off += 4;
    }
}

int md5(uint8_t *input, size_t len, uint8_t *digest)
{
    uint32_t a0 = 0x67452301, b0 = 0xefcdab89, c0 = 0x98badcfe, d0 = 0x10325476;
    uint32_t A, B, C, D, F, g;
    uint32_t M[16], tmp, j, k, m, n;
    uint8_t *message;
    size_t nlen, blen, i, c;

    blen = len * 8;
    nlen = len + 1;
    while ((nlen % 64) != 56) {
        nlen++;
    }

    nlen += 8;

    if ((message = malloc(nlen)) == NULL) {
        return -1;
    }

    memcpy(message, input, len);
    message[len] = 0x80;

    for (i = len + 1; i < nlen - 8; i++) {
        message[i] = 0x00;
    }

    for (i = nlen - 8; i < nlen; i++) {
        message[i] = blen & 0xFF;
        blen = blen >> 8;
    }

    for (c = 0; c < nlen; c += 64) {
        makechunk(message, c, M);
        A = a0;
        B = b0;
        C = c0;
        D = d0;
        for (i = 0; i < 64; i++) {
            if ((i >= 0) && (i <= 15)) {
                F = (B & C) | ((~B) & D);
                g = i;
            } else if ((i >= 16) && (i <= 31)) {
                F = (D & B) | ((~D) & C);
                g = (5*i + 1) % 16;
            } else if ((i >= 32) && (i <= 47)) {
                F = B ^ C ^ D;
                g = (3*i + 5) % 16;
            } else {
                F = C ^ (B | (~D));
                g = (7*i) % 16;
            }

            F = F + A + K[i] + M[g];
            A = D;
            D = C;
            C = B;
            B = B + leftrotate(F, s[i]);
        }

        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    free(message);

    memcpy(digest, &a0, 4);
    memcpy(digest + 4, &b0, 4);
    memcpy(digest + 8, &c0, 4);
    memcpy(digest + 12, &d0, 4);

    return 0;
}
