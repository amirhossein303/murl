#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "murl.h"

char *ltrim(char*);
char *rtrim(char*);
char *trim(char*);
void show(char, char*);
int split(char**, char*, char*);
int count(char*, char);
void add_params_from_url(void);
void add_record(char*, char*, char);
void add_option(char, char*);
void help(void);
void preview(void);
char* get_url_path(char*);
void parse_url_host(char*, char[]);
void parse_url_scheme(char*, char[]);
void parse_url(char* url);


Option options = {
    .url = NULL,
    .method = DEFAULT_HTTP_METHOD,
    .data = NULL,
    .headers = NULL,
    .params = NULL,
    .http_version = DEFUALT_HTTP_VERSION
};

URL url = {
    .scheme = "http://",
    .host = "",
    .port = 80,
    .path = NULL
};

Record records[MAX_RECORDS];
RecordCounter record_counter = {0,0,0};
int last_record_index = 0;


void help(void) {
    printf("Usage: %s [options...] <url>\n\n", PROG_NAME);
    printf("Arguments:\n");
    printf(" -H, --header               Headers of the request, e.g: 'Accept: plain/text'\n");
    printf(" -X, --method               HTTP method for the request. default: GET\n");
    printf(" -d, --data                 Data or body of the request, e.g: 'username=admin'\n");
    printf(" -p, --params               Query parameters of the request, e.g: 'param1=value'\n");
    printf(" -h, --help                 Show this text\n");
    printf(" -v, --version              Version number of the %s\n", PROG_NAME);
    printf(" -r, --show-records         Show all avaiables records. [ a: all | d: ata | h: headers | p: params ], default: a\n");
    printf(" -f, --format               Specify output format, [json|csv]. default json\n");
    printf(" --preview                  Display the preview of http request\n");
    printf(" --http-version             Change the HTTP version, allowed ['HTTP/1.1', 'HTTP/2', 'HTTP/3']\n");
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

int count(char* input, char c) {
    int i = 1;
    while (*(input++) != '\0') {
        if (*input == c) i++;
    }
    return i;
}

int split(char** array, char* input, char* del) {
    int i = 0;
    while ((array[i] = strtok(i == 0 ? input : NULL, del)) != NULL) {
        i++;
    }
    return i;
}

void add_params_from_url(void) {
    char* params;
    char* key;
    char* value;
    char* splitted_params[100];

    // ignore the url origin
    strtok(options.url, "?");
    
    // get params from url if exists
    params = strtok(NULL, "?");
    if (params != NULL) {
        for (int i = 0; i < split(splitted_params, params, "&"); i++) {
            key = strtok(splitted_params[i], DATA_DELIMITER);
            value = strtok(NULL, DATA_DELIMITER);
            if (key == NULL || value == NULL) {
                printf("error: invalid params from url has been set, format for params: '<key>=<value>'\n");
                printf("try --help for more information\n");
                exit(BAD_INPUT);
            }
            add_record(key, value, 'p');
        }
    }
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
        switch (type) {
        case 'h': record_counter.headers_count++; break;
        case 'd': record_counter.data_count++; break;
        case 'p': record_counter.params_count++; break;
        }
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
    int l, i;
    char* splitted_records[100];
    switch (type) {
        case 'd':
            // e.g: "username=admin&password=1234"
            l = split(splitted_records, input, "&");
            for (i = 0;i < l; i++) {
                key = strtok(splitted_records[i], DATA_DELIMITER);
                value = strtok(NULL, DATA_DELIMITER);
                if (key == NULL || value == NULL) {
                    printf("error: invalid input --data given, format for data: '<key>=<value>'\n");
                    printf("try --help for more information\n");
                    exit(BAD_INPUT);
                }
                add_record(key, value, type);
            }
            break;
        case 'h':
            // e.g: "Content-type: application/json"
            l = split(splitted_records, input, "\n");
            for (i = 0;i < l; i++) { 
                key = strtok(splitted_records[i], HEADER_DELIMITER);
                value = strtok(NULL, HEADER_DELIMITER);
                if (key == NULL || value == NULL) {
                    printf("error: invalid input --header given, format for headear: '<key>: <value>'\n");
                    printf("try --help for more information\n");
                    exit(BAD_INPUT);
                }
                add_record(trim(key), trim(value), type);
            }
            break;
        case 'p':
            // e.g: "page=1&flag=true"
            l = split(splitted_records, input, "&");
            for (i = 0; i < l; i++) {
                key = strtok(splitted_records[i], DATA_DELIMITER);
                value = strtok(NULL, DATA_DELIMITER);
                if (key == NULL || value == NULL) {
                    printf("error: invalid input --params given, format for params: '<key>=<value>'\n");
                    printf("try --help for more information\n");
                    exit(BAD_INPUT);
                }
                add_record(key, value, type);
            }
            break;
        default:
            printf("error: unknown option type '%c'\n", type);
            printf("try --help for more information\n");
            exit(BAD_INPUT);
    }
}


char* get_url_path(char* url) {
    short int slash = 0;
    while (*url != '\0') {
        if (*url == '/') slash++;
        if (slash == 3) {
            return url;
            break;
        }
        url++;
    }
    return "/";
}

void parse_url_host(char* url, char buff[]) {
    short int slash = 0;
    short int i = 0;

    while (*url != '\0') {
        if (slash == 2) {
            buff[i++] = *url;
        } else if (slash == 3) {
            break;
        }
        if (*url == '/') {
            slash++;
        }
        url++;
    }
    buff[i-1] = '\0';
}

void parse_url_scheme(char* url, char buff[]) {
    short int slash = 0;
    short int i = 0;
    while (*url != '\0') {
        if (slash < 2) {
            buff[i++] = *url;
        }
        if (*url == '/') {
            slash++;
        }
        url++;
    }
    buff[i] = '\0';
}

void parse_url(char* given_url) {
    url.path = get_url_path(given_url);
    parse_url_scheme(given_url, url.scheme);
    parse_url_host(given_url, url.host);

    strtok(url.host, ":");
    url.port = atol(strtok(NULL, ":"));
}


void preview(void) {
    int i, j;
    printf("%s %s HTTP/%s\n", options.method, url.path, options.http_version);
    printf("Host %s\n", url.host);

    // display the headers
    for (i = 0; i<last_record_index ; i++) {
        if (records[i].type == 'h') {
            printf("%s: %s\n", records[i].key, records[i].value);
        }
    }
    printf("\n");

    // display the 
    for (i = 0, j = 0; i<last_record_index; i++) {
        if (records[i].type == 'd') {
            j++;
            printf("%s=%s%s", records[i].key, records[i].value, (j < record_counter.data_count ? "&" : ""));
        }
    }    
    printf("\n");
}