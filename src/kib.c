#include <stdlib.h>
#include <stdio.h>

#include "ker.h"
#include "kob0.h"
#include "kob1.h"
#include "krt.h"

int
main(int argc, char *argv[])
{
    KRuntime *kruntime;
    KBuffer *kbuffer;

    kruntime = newruntime();
    kbuffer = newbuffer();
    kprocess(kruntime, "$bar");
    kprocess(kruntime, "$foo");
    kprocess(kruntime, "[+-+]");
    kprocess(kruntime, "=");
    kprocess(kruntime, "~");
    kprocess(kruntime, "=");
    repnable(kruntime->nable, kbuffer);
    printf("%s\n", kbuffer->buffer);

    delruntime(&kruntime);

    return 0;
}
