/*
    MURL: murl is the acronym for "my curl"

    Desc:
        It's just a simple program written in c that handle http requests like:
        {POST, HEAD, GET, DELETE} in a very simple way
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "murl.h"


Option options = {NULL, "GET", NULL, NULL, NULL};
Record records[MAX_RECORDS];
int last_record_index = 0;


void help(void) {
    printf("Usage: %s [options...] <url>\n\n", PROG_NAME);
    printf("Arguments:\n");
    printf(" -H, --header               Headers of the request, e.g: 'Accept: plain/text'\n");
    printf(" -m, --method               HTTP method for the request. default: GET\n");
    printf(" -d, --data                 Data or body of the request, e.g: 'username=admin'\n");
    printf(" -p, --params               Query parameters of the request, e.g: 'param1=value'\n");
    printf(" -h, --help                 Show this text\n");
    printf(" -v, --version              Version number of the %s\n", PROG_NAME);
    printf(" -r, --show-records         Show all avaiables records. [ a: all | d: ata | h: headers | p: params ], default: a\n");
    printf(" -f, --format               Specify output format, [json|csv]. default json\n");
}


char *ltrim(char *s) {
    while(isspace(*s)) s++;
    return s;
}


char *rtrim(char *s) {
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}


char *trim(char *s) {
    return rtrim(ltrim(s)); 
}


void show(char r, char* fmt) {
    int f_records = 0;
    if (strcmp(RECORD_TYPE(r), "unknown") == 0) {
        printf("error: invalid record type given: '%c'\n", r);
        printf("try --help for more information\n");
        exit(BAD_INPUT);
    }
    if (last_record_index == 0) {
        printf("no records!\n");
        exit(0);
    }
    if (strcmp(fmt, "json") == 0) {
        for (int i = 0; i < last_record_index; i++) { if (r == 'A' || r == records[i].type) {f_records++;}}
        printf("[\n");
        for (int i = 0; i < last_record_index; i++) {
            if (r == 'a' || r == records[i].type) {
                f_records--;
                printf(RECORD_PRETTY_JSON,
                    records[i].key, records[i].value, RECORD_TYPE(records[i].type), ((f_records == 0) ? ' ' : ','));
            }
        }
        printf("]\n");
    } else if (strcmp(fmt, "csv") == 0) {
        printf("key,value,type\n");
        for (int i = 0; i < last_record_index; i++) {
            if (r == 'a' || r == records[i].type) {
                printf("%s,%s,%s\n", records[i].key, records[i].value, RECORD_TYPE(records[i].type));
            }
        }
    }
}


void add_record(char* key, char* value, char type) {
    Record r = {key, value, type};
    if (last_record_index < MAX_RECORDS) {
        records[last_record_index++] = r;
        return;
    }
    printf("error: maximum records reached!\n");
    printf("currently only %d records acceptable!\n", MAX_RECORDS);
    exit(MAX_RECORD_REACHED);
}


void add_option(char type, char* input) {
    char *key;
    char *value;
    int valid;
    switch (type) {
        case 'd':
            // e.g: "username=admin"
            key = strtok(input, DATA_DELIMITER); // username
            value = strtok(NULL, DATA_DELIMITER); // admin
            if (key == NULL || value == NULL) {
                printf("error: invalid input --data given, format for data: '<key>=<value>'\n");
                printf("try --help for more information\n");
                exit(BAD_INPUT);
            }
            add_record(key, value, type);
            break;
        case 'h':
            // e.g: "Content-type: application/json"
            key = strtok(input, HEADER_DELIMITER); // Content-type
            value = strtok(NULL, HEADER_DELIMITER); // application/json
            if (key == NULL || value == NULL) {
                printf("error: invalid input --header given, format for headear: '<key>: <value>'\n");
                printf("try --help for more information\n");
                exit(BAD_INPUT);
            }
            add_record(trim(key), trim(value), type);
            break;
        case 'p':
            // e.g: "page=1"
            key = strtok(input, DATA_DELIMITER); // page
            value = strtok(NULL, DATA_DELIMITER); // 1
            if (key == NULL || value == NULL) {
                printf("error: invalid input --params given, format for params: '<key>=<value>'\n");
                printf("try --help for more information\n");
                exit(BAD_INPUT);
            }
            add_record(key, value, type);
            break;
        default:
            printf("error: unknown option type '%c'\n", type);
            printf("try --help for more information\n");
            exit(BAD_INPUT);
    }
}



int main(int argc, char* argv[]) {
    short int url_passed = 0;
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
            if (strcmp(*argv, "--method") == 0 || strcmp(*argv, "-m") == 0) {
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

    if (show_options) {
        show(show_record, fmt);
    }
    return 0;
};