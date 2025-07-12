#ifndef MD5_H
#define MD5_H

#include <stdlib.h>
#include <stdint.h>

typedef struct md5 md5;

md5 *md5_init(void);
void md5_add(md5 *ctx, size_t len, char *data);
void md5_finalize(md5 *ctx, char *digest);
void md5_reinit(md5 *ctx);

#endif /* MD5_H */
