#ifndef MD5_H
#define MD5_H

#include <stdlib.h>
#include <stdint.h>

typedef struct md5 md5;

/* Allocate an initialized MD5 context. Context must be free()'d by caller. NULL on error. */
md5 *md5_init(void);

/* Add len bytes of data to the MD5 context */
void md5_add(md5 *ctx, size_t len, char *data);

/* Finalize the MD5 context and get a 16-byte result in digest. */
void md5_finalize(md5 *ctx, char *digest);

/* Reinitialize an MD5 context, wiping all previous data and allowing it to be used for a new data stream. */
void md5_reinit(md5 *ctx);

#endif /* MD5_H */
