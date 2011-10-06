/* rmdoc.c
 * Copyright (c) 2005
 * Jeff Nettleton
 *
 * remove the comments from a C or C++ source file
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int check_isquote(char *buf, int i)
{
    return (buf[i] == '"' && buf[i-1] != '\\') ? 1 : 0;
}

int main(int argc, char *argv[])
{
    int fd, quote = 0;
    char *outfile;
    char *buf;
    unsigned int i;
    struct stat st;
    FILE *out;

    if (argc < 2) {
        fprintf(stderr, "usage: rmdoc input_file [output_file]\n");
        return 0;
    }

    if (argc == 2) {
        /* allocate outfile to be len + ".rmd" + '\0' */
        outfile = malloc(strlen(argv[1]) + 5);
        if (NULL == outfile) {
            fprintf(stderr,
                    "ERROR: allocating memory for outfile string!\n");
            exit(1);
        }

        snprintf(outfile, strlen(argv[1]) + 5, "%s.rmd", argv[1]);
    } else {
        /* full args used */
        outfile = strdup(argv[2]);
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "ERROR: opening input file %s for reading!\n",
                argv[1]);
        exit(1);
    }

    if (NULL == (out = fopen(outfile, "w"))) {
        fprintf(stderr, "ERROR: opening output file %s for writing: %s!\n",
                outfile, strerror(errno));
        exit(1);
    }
    free(outfile);

    if (stat(argv[1], &st) == -1) {
        fprintf(stderr, "ERROR: stat() on input file %s: %s!\n",
                argv[1], strerror(errno));
        exit(1);
    }

    buf = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);

    for (i = 0; i < (unsigned int) st.st_size; ++i) {
        if (check_isquote(buf, i)) {
            /* we're at either the end or beginning of quote. */
            if (!quote) {
                /* k, the start of a quote */
                quote = 1;
                fputc(buf[i], out);
                continue;
            } else {
                quote = 0;
            }
        } else {
            if (quote) {
                fputc(buf[i], out);
                continue;
            }
        }

        if (buf[i] == '/') {
            /* possible comment start, increment and check the next char */
            if ('\0' == (buf[++i])) {
                /* wow, this code sure as hell won't compile */
                break;
            } else {
                if (buf[i] == '/') {
                    /* we have found the beginning of a c++ style comment.
                     * just skip all the characters until the next newline
                     */
                    while ('\0' != (buf[++i])) {
                        if (buf[i] == '\n') {
                            /* end of line, c++ comments don't wrap */
                            fputc('\n', out);
                            break;
                        }
                    }
                } else if (buf[i] == '*') {
                    /* we found a c style comment. read until you hit the end */
                    while ('\0' != (buf[++i])) {
                        if (buf[i] == '*') {
                            /* possible comment ending (quite likely */
                            if ('\0' == (buf[++i])) {
                                /* nevermind, end of file */
                                break;
                            } else {
                                if (buf[i] == '/') {
                                    /* definitely the end of the comment */
                                    break;
                                } else {
                                    --i;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            fputc(buf[i], out);
        }
    }

    munmap (buf, st.st_size);
    close(fd);
    fclose(out);

    return 0;
}

