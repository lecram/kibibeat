typedef enum {E_OK, E_TYPE, E_ARITY, E_NAME, E_SYNTAX, E_FILE} Error;

typedef struct {
    Error error;
    char *filename;
    unsigned int line_id;
    unsigned int token_id;
} KReport;

void printreport(KReport kreport);
