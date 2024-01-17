/*

           m    m mmmmm  m     
    mmmmm  #    # #   "# #     
    # # #  #    # #mmmm" #     
    # # #  #    # #   "m #     
    # # #  "mmmm" #    " #mmmmm

    MURL: murl is the acronym for "my curl"

    Desc:
        It's just a simple program written in c that handle http requests like:
        {POST, HEAD, GET, DELETE} in a very simple way
*/

#include <stdio.h>

#include "core.h"


int main(int argc, char* argv[]) {
    short int url_passed = 0;
    short int show_preview = 0;
    short int show_options = 0;
    char show_record = 'a';
    short int method_passed = 0;
    char* fmt = "json";
    if (argc < 2) {
        help();
        return 1;
    }
    for (argc--,argv++; argc >= 0; argc--,argv++) {
        if (*argv == NULL) {break;}
        if ((**argv == '-') || ((**argv == '-') && (*((*argv)+1) == '-'))) {
            if (strcmp(*argv, "--help") == 0 || strcmp(*argv, "-h") == 0) {
                help();
                return 0;
            }
            if (strcmp(*argv, "--version") == 0 || strcmp(*argv, "-v") == 0) {
                printf("%s\n", VERSION);
                return 0;
            }
            if (strcmp(*argv, "--show-records") == 0 || strcmp(*argv, "-r") == 0) {
                show_options = 1;
                if (*(argv+1) != NULL && strlen(*(argv+1)) == 1 && **(argv+1) != '-') {
                    show_record = *(++argv)[0];
                    --argc;
                }
                continue;
            }
            if (strcmp(*argv, "--preview") == 0) {
                show_preview = 1;
                continue;
            }
            if (strcmp(*argv, "--format") == 0 || strcmp(*argv, "-f") == 0) {
                if ((*(argv+1) != NULL) && (strcmp(*(argv+1), "json") == 0 || strcmp(*(argv+1), "csv") == 0)) {
                    fmt = *(++argv);
                    --argc;
                    continue;
                } else {
                    printf("error: invalid --format given: '%s'\n", *(argv+1));
                    printf("try --help for more information\n");
                    exit(BAD_INPUT);
                }
            }
            if (strcmp(*argv, "--method") == 0 || strcmp(*argv, "-X") == 0) {
                if (method_passed) {
                    printf("error: method already taken!\n");
                    printf("try --help for more information\n");
                    exit(BAD_INPUT);
                }
                if (!((strcmp(*(argv+1), "GET") == 0) || (strcmp(*(argv+1), "POST") == 0)
                    || (strcmp(*(argv+1), "PUT") == 0) || (strcmp(*(argv+1), "DELETE") == 0)
                    || (strcmp(*(argv+1), "OPTIONS") == 0) || (strcmp(*(argv+1), "HEAD") == 0))) {
                    printf("error: invalid --method given: '%s'\n", *(argv+1));
                    printf("try --help for more information\n");
                    exit(BAD_INPUT);
                }
                options.method = *(++argv);
                method_passed = 1;
                argc--;
                continue;
            }
            if (strcmp(*argv, "--data") == 0 || strcmp(*argv, "-d") == 0) {
                add_option('d', *(++argv));
                argc--;
                continue;
            }
            if (strcmp(*argv, "--header") == 0 || strcmp(*argv, "-H") == 0) {
                add_option('h', *(++argv));
                argc--;
                continue;
            }
            if (strcmp(*argv, "--params") == 0 || strcmp(*argv, "-p") == 0) {
                add_option('p', *(++argv));
                argc--;
                continue;
            }
            if (strcmp(*argv, "--http-version") == 0) {
                if (strcmp(*(argv+1), "1.1") == 0 || strcmp(*(argv+1), "3") == 0 || strcmp(*(argv+1), "2") == 0) {
                    options.http_version = *(++argv);
                    argc--;
                    continue;
                } 
                printf("error: only http version {HTTP/1.1, HTTP/2, HTTP/3} allowed, it's '%s'\n", *(argv+1));
                printf("note: only pass the version number, not the 'HTTP/' phrase\n");
                exit(BAD_INPUT);
            }
            printf("error: option '%s' is unknown, try --help\n", *argv);
            exit(UNKNOWN_OPTION);
            
        } else {
            if (options.url != NULL) {
                printf("error: url already taken!\n");
                printf("try --help for more information\n");
                exit(BAD_INPUT);
            }
            options.url = *argv;
            argc--;
        }
    }
    if (options.url == NULL) {
        printf("error: 'url' not specified\n");
        printf("try --help for more information\n");
        exit(NO_URL_SPECIFIED);
    }
    printf("%s\n", options.url);
    parse_url(options.url);
    add_require_headers();
    if (show_preview) {
        preview();
        exit(0);
    }

    if (show_options) {
        show(show_record, fmt);
    }
    return 0;
};