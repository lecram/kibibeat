#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ker.h"
#include "kob0.h"
#include "kob1.h"
#include "ktrk.h"
#include "kps.h"
#include "krt.h"
#include "kop.h"

KRuntime *
newruntime()
{
    KRuntime *kruntime;

    kruntime = (KRuntime *) malloc(sizeof(KRuntime));
    kruntime->stack = newstack();
    kruntime->nable = newnable();
    kruntime->track = NULL;
    kruntime->comment = 0;
    return kruntime;
}

void
delruntime(KRuntime **kruntime)
{
    delstack(&(*kruntime)->stack);
    delnable(&(*kruntime)->nable);
    deltrack(&(*kruntime)->track);
    free(*kruntime);
    *kruntime = NULL;
}


Error
kprocess(KRuntime *kruntime, char *token)
{
    Error error = E_OK;

    if (!strcmp(token, "~"))
        error = kinvert(kruntime);
    else if (!strcmp(token, "@"))
        error = kclear(kruntime);
    else if (!strcmp(token, "|"))
        error = kor(kruntime);
    else if (!strcmp(token, "&"))
        error = kand(kruntime);
    else if (!strcmp(token, "*"))
        error = krepeat(kruntime);
    else if (!strcmp(token, "^"))
        error = kpattern(kruntime);
    else if (!strcmp(token, "<<"))
        error = krleft(kruntime);
    else if (!strcmp(token, ">>"))
        error = krright(kruntime);
    else if (!strcmp(token, "<"))
        error = kpleft(kruntime);
    else if (!strcmp(token, ">"))
        error = kpright(kruntime);
    else if (!strcmp(token, "/"))
        error = ksubdiv(kruntime);
    else if (!strcmp(token, "="))
        error = kset(kruntime);
    else if (!strcmp(token, "x"))
        error = kmix(kruntime);
    else {
        Kob *kob;

        kob = kparse(token);
        if (kob == EMPTY)
            return E_SYNTAX;
        spush(&kruntime->stack, kob);
    }
    return error;
}
