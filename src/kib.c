#include <stdlib.h>
#include <stdio.h>

#include "ker.h"
#include "kob0.h"
#include "kob1.h"
#include "krt.h"

int
main(int argc, char *argv[])
{
    char *tokens[] = {"$bar", "$foo", "[+-+-]", "[++--]", "|", "=", "~", "=", ""};
    int i;
    KRuntime *kruntime;
    KBuffer *kbuffer;

    kruntime = newruntime();
    kbuffer = newbuffer();
    for (i = 0; *tokens[i] != '\0'; i++)
        kprocess(kruntime, tokens[i]);
    repnable(kruntime->nable, kbuffer);
    printf("%s\n", kbuffer->buffer);

    delruntime(&kruntime);

    return 0;
}
