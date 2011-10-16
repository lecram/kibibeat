typedef enum {E_OK, E_TYPE, E_ARITY, E_NAME, E_SYNTAX} Error;

typedef struct {
    Error error;
    unsigned int line_id;
    unsigned int token_id;
} KReport;

void printreport(KReport kreport);
