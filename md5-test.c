#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "md5.h"

static char *readfile(FILE *inf, size_t *len)
{
    char *data;
    char tmp[1024];
    size_t res, max;

    *len = 0;

    if ((data = malloc(1024)) == NULL) {
        return NULL;
    }

    max = 1024;

    while ((res = fread(tmp, 1, 1024, inf)) > 0) {
        if ((*len + res) > max) {
            if ((data = realloc(data, max * 2)) == NULL) {
                return NULL;
            }

            max *= 2;
        }

        memcpy(data + *len, tmp, res);
        *len += res;
    }

    return data;
}

int main(int argc, char *argv[])
{
    uint8_t digest[16];
    char *data;
    size_t len;
    FILE *inf;
    int i, j;

    for (i = 1; i < argc; i++) {
        if ((inf = fopen(argv[i], "rb")) == NULL) {
            perror(argv[i]);
            continue;
        }

        if ((data = readfile(inf, &len)) == NULL) {
            fprintf(stderr, "memory error on %s\n", argv[i]);
            fclose(inf);
            exit(EXIT_FAILURE);
        }

        if (md5(data, len, digest) != 0) {
            fprintf(stderr, "MD5 error on %s\n", argv[i]);
            fclose(inf);
            free(data);
            exit(EXIT_FAILURE);
        }

        for (j = 0; j < 16; j++) {
            printf("%02x", digest[j]);
        }

        printf("\t%s\n", argv[i]);
        free(data);
        fclose(inf);
    }

    return 0;
}
