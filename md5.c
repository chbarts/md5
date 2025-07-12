#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"

#ifdef DEBUG
#include <stdio.h>
#include <ctype.h>
#endif

typedef struct md5 {
    uint32_t a0, b0, c0, d0;
    uint32_t  M[16];
    uint8_t buf[64];
    size_t size;
    int bsize;
} md5;

static uint32_t s[64] =
    { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

static uint32_t K[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
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
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

md5 *md5_init(void)
{
    md5 *ctx;

    if ((ctx = malloc(sizeof(md5))) == NULL) {
        return NULL;
    }

    ctx->a0 = 0x67452301;
    ctx->b0 = 0xefcdab89;
    ctx->c0 = 0x98badcfe;
    ctx->d0 = 0x10325476;
    ctx->size = 0;
    ctx->bsize = 0;

    return ctx;
}

static uint32_t leftrotate(uint32_t in, uint32_t amt)
{
    uint64_t tmp = ((uint64_t) in) << amt;
    return ((in << amt) | ((uint32_t) (tmp >> 32)));
}

static void makechunk(uint8_t * input, uint64_t off, uint32_t * output)
{
    int j;
    size_t i;
    uint32_t tmp = 0;

    for (j = 0; j < 16; j++) {
        for (i = 0; i < 4; i++) {
            tmp |= (input[i + off] & 0xFF) << (i * 8);
        }

#ifdef DEBUG
        printf("%08lx ", (unsigned long) tmp);
        if ((j % 4) == 3)
            puts("");
#endif
        output[j] = tmp;
        tmp = 0;
        off += 4;
    }
}

static void md5round(md5 * ctx)
{
    uint32_t A, B, C, D, F, g;
    int i;

    A = ctx->a0;
    B = ctx->b0;
    C = ctx->c0;
    D = ctx->d0;
    for (i = 0; i < 64; i++) {
        if ((i >= 0) && (i <= 15)) {
            F = (B & C) | ((~B) & D);
            g = i;
        } else if ((i >= 16) && (i <= 31)) {
            F = (D & B) | ((~D) & C);
            g = (5 * i + 1) % 16;
        } else if ((i >= 32) && (i <= 47)) {
            F = B ^ C ^ D;
            g = (3 * i + 5) % 16;
        } else {
            F = C ^ (B | (~D));
            g = (7 * i) % 16;
        }

        F = F + A + K[i] + ctx->M[g];
        A = D;
        D = C;
        C = B;
        B = B + leftrotate(F, s[i]);
    }

    ctx->a0 += A;
    ctx->b0 += B;
    ctx->c0 += C;
    ctx->d0 += D;
}

void md5_add(md5 *ctx, size_t len, char *data)
{
    size_t off = 0, cs = len, nc;
    int r;

#ifdef DEBUG
    size_t z;

    for (z = 0; z < len; z++) {
        printf(" %02x ", (unsigned char) data[z]);

        if ((z % 4) == 3)
            printf(" | ");

        if ((z % 16) == 15)
            puts("");
    }

    puts("");

#endif

    if (ctx->bsize && ((ctx->bsize + len) >= 64)) {
        r = 64 - ctx->bsize;
        memcpy(ctx->buf + ctx->bsize, data, r);
        makechunk(ctx->buf, 0, ctx->M);
        md5round(ctx);
        cs = len - r;
        off += r;
        ctx->bsize = 0;
        if (0 == cs) {
            ctx->size += len;
            return;
        }

    } else if (ctx->bsize && ((ctx->bsize + len) < 64)) {
        memcpy(ctx->buf + ctx->bsize, data, len);
        ctx->bsize += len;
        ctx->size += len;
        return;
    }

    nc = cs / 64; /* number of chunks in data */

    while (nc) {
        makechunk(data, off, ctx->M);
        md5round(ctx);
        off += 64;
        nc--;
    }

    if (cs % 64) {
        memcpy(ctx->buf, data + off, cs % 64);
        ctx->bsize = cs % 64;
    }

    ctx->size += len;
}

void md5_finalize(md5 *ctx, char *digest)
{
    int i;

    if (0 == ctx->bsize) {
        ctx->buf[0] = 0x80;
        i = 1;
    } else if ((64 - ctx->bsize) >= 9) {
        ctx->buf[ctx->bsize] = 0x80;
        i = ctx->bsize + 1;
    } else if (64 == ctx->bsize) {
        makechunk(ctx->buf, 0, ctx->M);
        md5round(ctx);
        ctx->buf[0] = 0x80;
        i = 1;
    } else {
        ctx->buf[ctx->bsize] = 0x80;

        for (i = ctx->bsize + 1; i < 64; i++) {
            ctx->buf[i] = 0;
        }

        makechunk(ctx->buf, 0, ctx->M);
        md5round(ctx);
        i = 0;
    }

    for (; i < 56; i++) {
        ctx->buf[i] = 0;
    }

    for (; i < 64; i++) {
        ctx->buf[i] = ctx->size & 0xFF;
        ctx->size = ctx->size >> 8;
    }

    makechunk(ctx->buf, 0, ctx->M);
    md5round(ctx);
    memcpy(digest, &(ctx->a0), 4);
    memcpy(digest + 4, &(ctx->b0), 4);
    memcpy(digest + 8, &(ctx->c0), 4);
    memcpy(digest + 12, &(ctx->d0), 4);
}
