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
            puts("Ok.");
            break;
        case E_TYPE:
            puts("Type error.");
            break;
        case E_ARITY:
            puts("Arity error.");
            break;
        case E_NAME:
            puts("Name error.");
            break;
        case E_SYNTAX:
            puts("Syntax error.");
            break;
        case E_FILE:
            puts("File error.");
            break;
    }
}
