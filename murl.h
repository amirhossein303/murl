#define PROG_NAME "murl"
#define VERSION "0.0.1"

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
} Option;

typedef struct {
    char*           key;
    char*           value;
    char            type;                      // 'D': Data, 'P': Parameters, 'H': Header   
} Record;


char *ltrim(char*);
char *rtrim(char*);
char *trim(char*);
void show(char, char*);
void add_record(char*, char*, char);
void add_option(char, char*);
void help(void);