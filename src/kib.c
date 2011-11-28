#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ker.h"
#include "kob0.h"
#include "kob1.h"
#include "ktrk.h"
#include "krt.h"

#define MAX_LINE_LEN 256
#define MAX_TOKEN_LEN 32

void
interactive(KReport *kreport)
{
    char line[MAX_LINE_LEN];
    char token[MAX_TOKEN_LEN];
    char *c;
    int i;
    KRuntime *kruntime;
    KBuffer *kbuffer;

    kruntime = newruntime();
    kbuffer = newbuffer();

    while(1) {
        printf("> ");
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL)
            break;
        if (strcmp(line, "exit\n") == 0)
            break;
        else {
            kreport->token_id = 0;
            c = line;
            while (1) {
                while (isspace(*c) && *c != '\n')
                    c++;
                if (*c == '\n')
                    break;
                kreport->token_id++;
                for (i = 0; !isspace(*c); i++, c++)
                    token[i] = *c;
                token[i] = '\0';
                kreport->error = kprocess(kruntime, token);
                if (kreport->error != E_OK) return;
            }
            repstack(kruntime->stack, kbuffer);
            printf("%s\n", kbuffer->buffer);
            repnable(kruntime->nable, kbuffer);
            printf("%s\n", kbuffer->buffer);
        }
    }
    delruntime(&kruntime);
    delbuffer(&kbuffer);
}

void
run(KReport *kreport)
{
    FILE *fp;
    char line[MAX_LINE_LEN];
    char token[MAX_TOKEN_LEN];
    char *c;
    int i;
    KRuntime *kruntime;
    KBuffer *kbuffer;
    KTrack *ktrack;

    fp = fopen(kreport->filename, "r");
    if (fp == NULL) {
        kreport->error = E_FILE;
        return;
    }
    kruntime = newruntime();
    kbuffer = newbuffer();
    kreport->line_id = 0;
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        kreport->line_id++;
        kreport->token_id = 0;
        c = line;
        while (1) {
            while (isspace(*c) && *c != '\n')
                c++;
            if (*c == '\n')
                break;
            kreport->token_id++;
            for (i = 0; !isspace(*c); i++, c++)
                token[i] = *c;
            token[i] = '\0';
            kreport->error = kprocess(kruntime, token);
            if (kreport->error != E_OK) return;
        }
    }
    repstack(kruntime->stack, kbuffer);
    printf("%s\n", kbuffer->buffer);
    repnable(kruntime->nable, kbuffer);
    printf("%s\n", kbuffer->buffer);
    ktrack = kruntime->track;
    while (ktrack != NULL) {
        printf("delta: %u, note: %d, volume: %d\n",
               ktrack->event->delta,
               (int) ktrack->event->note,
               (int) ktrack->event->volume);
        ktrack = ktrack->next;
    }
    delruntime(&kruntime);
    delbuffer(&kbuffer);
    fclose(fp);
}

int
main(int argc, char *argv[])
{
    KReport kreport;

    kreport.error = E_OK;
    kreport.filename = "";
    kreport.line_id = 1;
    kreport.token_id = 1;
    if (argc == 1)
        interactive(&kreport);
    else if (argc == 2) {
        kreport.filename = argv[1];
        run(&kreport);
    }

    if (kreport.error != E_OK) {
        printreport(kreport);
        return 1;
    }
    return 0;
}
