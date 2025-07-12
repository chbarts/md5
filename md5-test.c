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

static void help(char pnam[])
{
    printf("%s [OPTION] [FILES]...\n", pnam);
    puts("Compute the MD5 sum of files or standard input.");
    puts("");
    puts(" -h, --help        Print this help");
    puts(" -v, --version     Print version information");
    puts(" --                Stop processing arguments");
    puts("");
    puts("Report bugs to <chbarts@gmail.com>.");
}

static void version(void)
{
    puts("md5 version 0.1");
    puts("Copyright 2025, Chris Barts <chbarts@gmail.com>");
    puts("Licensed under the GNU General Public License, version 3.0 or, at your option, any later version.");
    puts("This software has NO WARRANTY, even for USABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
}

int main(int argc, char *argv[])
{
    uint8_t digest[16];
    char tmp[1024];
    size_t len;
    FILE *inf;
    int i = 1, j;
    md5 *ctx;

    if (1 == argc) {
        help(argv[0]);
        return 0;
    }

    if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
        help(argv[0]);
        return 0;
    }

    if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v")) {
        version();
        return 0;
    }

    if (!strcmp("--", argv[1])) {
        i = 2;
    }

    if ((ctx = md5_init()) == NULL) {
        fprintf(stderr, "memory error\n");
        exit(EXIT_FAILURE);
    }

    for (; i < argc; i++) {
        if (!strcmp(argv[i], "-")) {
            inf = stdin;
        } else {
            if ((inf = fopen(argv[i], "rb")) == NULL) {
                perror(argv[i]);
                continue;
            }
        }

        while ((len = fread(tmp, 1, 1024, inf)) > 0) {
            md5_add(ctx, len, tmp);
        }

        md5_finalize(ctx, digest);

        for (j = 0; j < 16; j++) {
            printf("%02x", digest[j]);
        }

        if (inf != stdin) {
            printf("\t%s\n", argv[i]);
            fclose(inf);
        } else {
            printf("\tstdin\n");
        }

        md5_reinit(ctx);
    }

    free(ctx);
    return 0;
}
