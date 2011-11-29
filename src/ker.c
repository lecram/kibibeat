#include <stdio.h>

#include "ker.h"

void
printreport(KReport kreport)
{
    printf("%s:%u:%u: ",
           kreport.filename,
           kreport.line_id,
           kreport.token_id);
    switch(kreport.error) {
        case E_OK:
            (void) puts("Ok.");
            break;
        case E_TYPE:
            (void) puts("Type error.");
            break;
        case E_ARITY:
            (void) puts("Arity error.");
            break;
        case E_NAME:
            (void) puts("Name error.");
            break;
        case E_SYNTAX:
            (void) puts("Syntax error.");
            break;
        case E_FILE:
            (void) puts("File error.");
            break;
    }
}
