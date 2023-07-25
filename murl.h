#define PROG_NAME "murl"
#define VERSION "0.0.1"

enum error_codes { 
    NO_URL_SPECIFIED = 1,
    NO_OPTIONS,
    UNKNOWN_OPTION,
};


typedef struct options {
    char* url;
    char* method;
    char* data;
    char* headers;
    char* params;
} Option;
