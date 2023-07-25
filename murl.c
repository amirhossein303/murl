/*
    MURL: murl is the acronym for "my curl"

    Desc:
        It's just a simple program written in c that handle http requests like:
        {POST, HEAD, GET, DELETE} in a very simple way
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "murl.h"


void help(void) {
    printf("Usage: %s [options...] <url>\n", PROG_NAME);
    printf(" -H, --header       Headers of the request\n");
    printf(" -m, --method       HTTP method for the request. default: GET\n");
    printf(" -d, --data         Data or body of the request\n");
    printf(" -p, --params       Query parameters of the request\n");
    printf(" -h, --help         Show this text\n");
    printf(" -v, --version      Version number of the %s\n", PROG_NAME);
}


Option options = {NULL, "GET", NULL, NULL, NULL};


int main(int argc, char* argv[]) {
    int url_passed = 0;
    if (argc < 2) {
        printf("error: 'url' not specified\n");
        exit(NO_URL_SPECIFIED);
    }
    for (argc--, argv++; argc > 0; argc--,argv++) {
        if ((**argv == '-') || ((**argv == '-') && (*((*argv)+1) == '-'))) {
            if (strcmp(*argv, "--help") == 0 || strcmp(*argv, "-h") == 0) {
                help();
                return 0;
            }
            if (strcmp(*argv, "--version") == 0 || strcmp(*argv, "-v") == 0) {
                printf("%s\n", VERSION);
                return 0;
            }
            if (strcmp(*argv, "--method") == 0 || strcmp(*argv, "-m") == 0) {
                options.method = *(++argv);
                argc--;
                continue;
            }
            if (strcmp(*argv, "--data") == 0 || strcmp(*argv, "-d") == 0) {
                options.data = *(++argv);
                argc--;
                continue;
            }
            if (strcmp(*argv, "--header") == 0 || strcmp(*argv, "-H") == 0) {
                options.headers = *(++argv);
                argc--;
                continue;
            }
            if (strcmp(*argv, "--params") == 0 || strcmp(*argv, "-p") == 0) {
                options.params = *(++argv);
                argc--;
                continue;
            }
            printf("error: option '%s' is unknown, try --help\n", *argv);
            exit(UNKNOWN_OPTION);
        } else {
            options.url = *argv;
        }
    }
    if (options.url == NULL) {
        printf("error: 'url' not specified\n");
        exit(NO_URL_SPECIFIED);
    }
    
    return 0;
};