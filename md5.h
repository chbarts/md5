#ifndef MD5_H
#define MD5_H

#include <stdlib.h>
#include <stdint.h>

int md5(uint8_t *input, size_t len, uint8_t *digest);

#endif /* MD5_H */
