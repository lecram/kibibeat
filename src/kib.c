#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ker.h"
#include "kob0.h"
#include "kob1.h"
#include "krt.h"

#define MAX_LINE_LEN 256
#define MAX_TOKEN_LEN 32

Error
interactive()
{
    char line[MAX_LINE_LEN];
    char token[MAX_TOKEN_LEN];
    char *c;
    int i;
    unsigned int token_id;
    KRuntime *kruntime;
    KBuffer *kbuffer;
    Error error;

    kruntime = newruntime();
    kbuffer = newbuffer();

    while(1) {
        printf("> ");
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL)
            break;
        if (strcmp(line, "exit\n") == 0)
            break;
        else {
            token_id = 0;
            c = line;
            while (1) {
                while (isspace(*c) && *c != '\n')
                    c++;
                if (*c == '\n')
                    break;
                token_id++;
                for (i = 0; !isspace(*c); i++, c++)
                    token[i] = *c;
                token[i] = '\0';
                error = kprocess(kruntime, token);
                if (error != E_OK) return error;
            }
        repnable(kruntime->nable, kbuffer);
        printf("%s\n", kbuffer->buffer);
        }
    }
    delruntime(&kruntime);
    delbuffer(&kbuffer);
    return E_OK;
}

int
main(int argc, char *argv[])
{
    if (argc == 1)
        interactive();

    return 0;
}
