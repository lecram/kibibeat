#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "kob0.h"
#include "kps.h"

KNumber *
kparsenumber(char *token)
{
    return newnumber((unsigned int) strtol(token, NULL, 0));
}

KBeat *
kparsebeat(char *token)
{
    if (*token == '+')
        return newbeat(1);
    else
        return newbeat(0);
}

KBlist *
kparseblist(char *token)
{
    KBlist *kblist;

    kblist = newblist();
    token++;
    while (*token != ']') {
        if (*token == '+')
            blist_insert(kblist, (int) kblist->length, 1);
        else if (*token == '-')
            blist_insert(kblist, (int) kblist->length, 0);
        token++;
    }
    return kblist;
}

KName *
kparsename(char *token)
{
    return newname(token + 1);
}


int
kisnumber(char *token)
{
    for (; *token != '\0'; token++)
        if (!isdigit(*token))
            return 0;
    return 1;
}

int
kisbeat(char *token)
{
    return (!strcmp(token, "+") || !strcmp(token, "-"));
}

int
kisblist(char *token)
{
    return (*token == '[' && *(token + strlen(token) - 1) == ']');
}

int
kisname(char *token)
{
    return (*token == '$');
}


Kob *
kparse(char *token)
{
    if (kisnumber(token))
        return (Kob *) kparsenumber(token);
    else if (kisbeat(token))
        return (Kob *) kparsebeat(token);
    else if (kisblist(token))
        return (Kob *) kparseblist(token);
    else if (kisname(token))
        return (Kob *) kparsename(token);
    else
        return NULL;
}
