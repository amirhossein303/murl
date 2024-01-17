#define PROG_NAME "murl"
#define VERSION "0.0.1"
#define AGENT PROG_NAME "/" VERSION
#define MAX_RECORDS 1000
#define KEY_LEN 100
#define VALUE_LEN 255

#define DATA_DELIMITER "="
#define HEADER_DELIMITER ":"
#define RECORD_TYPE(t) ((t == 'd') ? "data" : (t == 'h') ? "header" : (t == 'p') ? "parameter" : (t == 'a') ? "all" : "unknown")
#define RECORD_PRETTY_JSON "    {\n" \
                      "        \"key\": \"%s\",\n" \
                      "        \"value\": \"%s\",\n" \
                      "        \"type\": \"%s\"\n" \
                      "    }%c\n"
#define DEFAULT_HTTP_METHOD "GET"
#define DEFUALT_HTTP_VERSION "1.1"
#define DEFAULT_PORT 80


enum error_codes { 
    NO_URL_SPECIFIED = 1,
    NO_OPTIONS,
    UNKNOWN_OPTION,
    BAD_INPUT,
    MAX_RECORD_REACHED
};


typedef struct {
    char*           url;
    char*           method;
    char*           data;
    char*           headers;
    char*           params;
    char*           http_version; // defualt HTTP/1.1
} Option;

typedef struct {
    char*           key;
    char*           value;
    char            type;                      // 'D': Data, 'P': Parameters, 'H': Header   
} Record;

typedef struct {
    int             headers_count;
    int             params_count;
    int             data_count;
} RecordCounter;

typedef struct {
    char scheme[10];
    char host[32];
    int port;
    char* path;
} URL;
