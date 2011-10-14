#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "kob0.h"
#include "kob1.h"

/* Return a pseudo-random number x, such that 0 <= x < 1. */
double
krandom()
{
    return rand() / ((double) RAND_MAX + 1);
}

int
trndlevel(KNable *knable)
{
    int level = 0;
    int max;

    if (knable->level == SLHEIGHT - 1)
        max = SLHEIGHT - 1;
    else
        /* Dirty Hack. */
        max = knable->level + 1;
    while (krandom() < SLPROB && level < max)
        level++;
    return level;
}

KEntry *
newentry(int level, char *name, Kob *value)
{
    KEntry *kentry;
    int i;

    kentry = (KEntry *) malloc(sizeof(KEntry));
    kentry->name = (char *) malloc((strlen(name) + 1) * sizeof(char));
    strcpy(kentry->name, name);
    kentry->value = value;
    kentry->next = (KEntry **) malloc((level + 1) * sizeof(KEntry *));
    for (i = 0; i <= level; i++)
        kentry->next[i] = NULL;
    return kentry;
}

void
delentry(KEntry **kentry)
{
    free((*kentry)->name);
    free((*kentry)->next);
    free(*kentry);
    *kentry = NULL;
}

size_t
repsizeentry(KEntry *kentry)
{
    return repsizekob(kentry->value) + strlen(kentry->name) + 2;
}

void
repentry(KEntry *kentry, char *buffer)
{
    char *brep = buffer;

    strcpy(brep, kentry->name);
    brep += strlen(kentry->name);
    strcpy(brep, ": ");
    brep += 2;
    repkob(kentry->value, brep);
}


KNable *
newnable()
{
    KNable *knable;

    /* [Re]Initialize pseudo-random number generator. */
    srand(time(NULL));

    knable = (KNable *) malloc(sizeof(KNable));
    knable->level = 0;
    knable->header = newentry(SLHEIGHT - 1, "", NULL);
    return knable;
}

void
delnable(KNable **knable)
{
    KEntry *a, *b;

    a = (*knable)->header;
    do {
        b = a->next[0];
        delentry(&a);
        a = b;
    } while (a != NULL);
    free(*knable);
    *knable = NULL;
}

size_t
repsizenable(KNable *knable)
{
    KEntry *kentry;
    size_t size = 0;
    unsigned int len = 0;

    kentry = knable->header->next[0];
    while (kentry != NULL) {
        size += repsizeentry(kentry);
        len++;
        kentry = kentry->next[0];
    }
    if (len == 0)
        return 2;
    return size + len * 2;
}

void
repnable(KNable *knable, KBuffer *kbuffer)
{
    KEntry *kentry;
    char *brep;

    setupbuffer(kbuffer, repsizenable(knable));
    brep = kbuffer->buffer;
    *brep = '{';
    brep++;
    kentry = knable->header->next[0];
    while (kentry != NULL) {
        repentry(kentry, brep);
        brep += repsizeentry(kentry);
        kentry = kentry->next[0];
        if (kentry != NULL) {
            strcpy(brep, ", ");
            brep += 2;
        }
    }
    *brep = '}';
    brep++;
    *brep = '\0';
}

void
tset(KNable *knable, char *name, Kob *value)
{
    KEntry *kentry;
    KEntry *update[SLHEIGHT];
    int found, i;

    /* Seek name. */
    kentry = knable->header;
    for (i = knable->level; i >= 0; i--) {
        while (kentry->next[i] != NULL) {
            if (strcmp(kentry->next[i]->name, name) >= 0)
                break;
            kentry = kentry->next[i];
        }
        update[i] = kentry;
    }
    kentry = kentry->next[0];
    found = (kentry != NULL);
    if (found)
        found = (strcmp(kentry->name, name) == 0);
    if (found) {
        /* Set value. */
        delkob(&kentry->value);
        kentry->value = value;
    }
    else {
        /* Name not found. */
        /* New entry. */
        int level = trndlevel(knable);

        if (level > knable->level) {
            /* Assuming Dirty Hack, level == knable->level + 1. */
            knable->level++;
            update[level] = knable->header;
        }
        kentry = newentry(level, name, value);
        /* Place the entry in the skip list. */
        for (i = 0; i <= level; i++) {
            kentry->next[i] = update[i]->next[i];
            update[i]->next[i] = kentry;
        }
    }
}

int
tget(KNable *knable, char *name, Kob **value)
{
    KEntry *kentry;
    int found, i;

    /* Seek name. */
    kentry = knable->header;
    for (i = knable->level; i >= 0; i--) {
        while (kentry->next[i] != NULL) {
            if (strcmp(kentry->next[i]->name, name) >= 0)
                break;
            kentry = kentry->next[i];
        }
    }
    kentry = kentry->next[0];
    found = (kentry != NULL);
    if (found)
        found = (strcmp(kentry->name, name) == 0);
    if (found) {
        /* Get value. */
        *value = kentry->value;
        return 1;
    }
    /* Name not found. */
    return 0;
}

int
tremove(KNable *knable, char *name)
{
    KEntry *kentry;
    KEntry *update[SLHEIGHT];
    int found, i;

    /* Seek name. */
    kentry = knable->header;
    for (i = knable->level; i >= 0; i--) {
        while (kentry->next[i] != NULL) {
            if (strcmp(kentry->next[i]->name, name) >= 0)
                break;
            kentry = kentry->next[i];
        }
        update[i] = kentry;
    }
    kentry = kentry->next[0];
    found = (kentry != NULL);
    if (found)
        found = (strcmp(kentry->name, name) == 0);
    if (found) {
        /* Remove pair. */
        for (i = 0; i <= knable->level; i++) {
            if (update[i]->next[i] != kentry)
                break;
            update[i]->next[i] = kentry->next[i];
        }
        delentry(&kentry);
        while (knable->level > 0  &&
               knable->header->next[knable->level] == NULL)
            knable->level--;
        return 1;
    }
    /* Name not found. */
    return 0;
}


KStack *
newstack()
{
    return NULL;
}

void
delstack(KStack **kstack)
{
    KStack *a, *b;

    a = *kstack;
    while (a != NULL) {
        b = a->next;
        delkob(&a->kob);
        free(a);
        a = b;
    }
    *kstack = NULL;
}

size_t
repsizestack(KStack *kstack)
{
    size_t size;

    if (kstack == NULL)
        return 1;
    size = repsizekob(kstack->kob);
    kstack = kstack->next;
    while (kstack != NULL) {
        size += 1 + repsizekob(kstack->kob);
        kstack = kstack->next;
    }
    return size;
}

void
repstack(KStack *kstack, KBuffer *kbuffer)
{
    char *brep;

    setupbuffer(kbuffer, repsizestack(kstack));
    brep = kbuffer->buffer;
    if (kstack == NULL) {
        strcpy(brep, "/");
        return;
    }
    repkob(kstack->kob, brep);
    brep += repsizekob(kstack->kob);
    kstack = kstack->next;
    while (kstack != NULL) {
        *brep = ' ';
        brep ++;
        repkob(kstack->kob, brep);
        brep += repsizekob(kstack->kob);
        kstack = kstack->next;
    }
}


void
spush(KStack **kstack, Kob *kob)
{
    KStack *pushed;

    pushed = (KStack *) malloc(sizeof(KStack));
    pushed->kob = kob;
    pushed->next = *kstack;
    *kstack = pushed;
}

Kob *
spop(KStack **kstack)
{
    KStack *poped;
    Kob* kob;

    poped = *kstack;
    *kstack = poped->next;
    kob = poped->kob;
    free(poped);
    return kob;
}


int
main_old(int argc, char *argv[])
{
    KBuffer *kbuffer;
    KBlist *blist_a, *blist_b;
    KNable *knable;
    KStack *kstack;

    kbuffer = newbuffer();
    knable = newnable();
    kstack = newstack();
    blist_a = newblist();
    blist_b = newblist();
    blist_insert(blist_a, 0, 0);
    blist_insert(blist_a, 0, 0);
    blist_insert(blist_a, 1, 1);
    blist_insert(blist_b, 0, 1);
    blist_extend(blist_b, blist_a);
    blist_remove(blist_a, 0);
    blist_extend(blist_b, blist_a);
    tset(knable, "foo", (Kob *) blist_a);
    tset(knable, "bar", (Kob *) blist_b);
    repnable(knable, kbuffer);
    printf("%s\n", kbuffer->buffer);
    repstack(kstack, kbuffer);
    printf("%s\n", kbuffer->buffer);
    spush(&kstack, (Kob *) blist_a);
    repstack(kstack, kbuffer);
    printf("%s\n", kbuffer->buffer);
    spush(&kstack, (Kob *) blist_b);
    repstack(kstack, kbuffer);
    printf("%s\n", kbuffer->buffer);
    delbuffer(&kbuffer);
    delnable(&knable);
    //delstack(&kstack);
    delblist(&blist_a);
    delblist(&blist_b);

    return 0;
}
