#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "kob0.h"

KNumber *
newnumber(unsigned int value)
{
    KNumber *knumber;

    knumber = (KNumber *) malloc(sizeof(KNumber));
    knumber->type = T_NUMBER;
    knumber->value = value;
    return knumber;
}

KBeat *
newbeat(int value)
{
    KBeat *kbeat;

    kbeat = (KBeat *) malloc(sizeof(KBeat));
    kbeat->type = T_BEAT;
    kbeat->value = value;
    return kbeat;
}

KBnode *
newbnode(int value)
{
    KBnode *kbnode;

    kbnode = (KBnode *) malloc(sizeof(KBnode));
    kbnode->beat = value;
    kbnode->next = NULL;
    return kbnode;
}

KBlist *
newblist()
{
    KBlist *kblist;

    kblist = (KBlist *) malloc(sizeof(KBlist));
    kblist->type = T_BLIST;
    kblist->length = 0U;
    kblist->first = kblist->last = NULL;
    return kblist;
}

KName *
newname(char *name)
{
    KName *kname;

    kname = (KName *) malloc(sizeof(KName));
    kname->type = T_NAME;
    strncpy(kname->name, name, MAX_NAME_LEN);
    return kname;
}


void
delnumber(KNumber **knumber)
{
    free(*knumber);
    *knumber = NULL;
}

void
delbeat(KBeat **kbeat)
{
    free(*kbeat);
    *kbeat = NULL;
}

void
delbnode(KBnode **kbnode)
{
    free(*kbnode);
    *kbnode = NULL;
}

void
delblist(KBlist **kblist)
{
    KBnode *second;

    while ((*kblist)->length > 1U) {
        second = (*kblist)->first->next;
        delbnode(&(*kblist)->first);
        (*kblist)->first = second;
        (*kblist)->length--;
    }
    delbnode(&(*kblist)->first);
    free(*kblist);
    *kblist = NULL;
}

void
delname(KName **kname)
{
    free(*kname);
    *kname = NULL;
}


void
delkob(Kob **kob)
{
    switch(**kob) {
        case EMPTY:
            return;
        case T_NUMBER:
            delnumber((KNumber **) kob);
            break;
        case T_BEAT:
            delbeat((KBeat **) kob);
            break;
        case T_BLIST:
            delblist((KBlist **) kob);
            break;
        case T_NAME:
            delname((KName **) kob);
            break;
    }
}


KBuffer *
newbuffer()
{
    KBuffer *kbuffer;

    kbuffer = (KBuffer *) malloc(sizeof(KBuffer));
    kbuffer->size = 1;
    kbuffer->buffer = (char *) malloc(sizeof(char));
    return kbuffer;
}

size_t
setupbuffer(KBuffer *kbuffer, size_t size)
{
    while (size >= kbuffer->size)
        kbuffer->size *= 2;
    free(kbuffer->buffer);
    kbuffer->buffer = (char *) malloc(sizeof(char) * kbuffer->size);
    return kbuffer->size;
}

void
delbuffer(KBuffer **kbuffer)
{
    free((*kbuffer)->buffer);
    free(*kbuffer);
    *kbuffer = NULL;
}


size_t
repsizenumber(KNumber *knumber)
{
    return (size_t) log10((double) knumber->value) + 1;
}

size_t
repsizebeat(KBeat *kbeat)
{
    return 1;
}

size_t
repsizeblist(KBlist *kblist)
{
    return (size_t) kblist->length + 2;
}

size_t
repsizename(KName *kname)
{
    return strlen(kname->name);
}


size_t
repsizekob(Kob *kob)
{
    switch(*kob) {
        case T_NUMBER:
            return repsizenumber((KNumber *) kob);
        case T_BEAT:
            return repsizebeat((KBeat *) kob);
        case T_BLIST:
            return repsizeblist((KBlist *) kob);
        case T_NAME:
            return repsizename((KName *) kob);
        default:
            return 0;
    }
}


void
repnumber(KNumber *knumber, char *buffer)
{
    size_t length;

    if (knumber->value == 0U) {
        strcpy(buffer, "0");
        return;
    }
    length = (size_t) log10((double) knumber->value) + 1;
    (void) snprintf(buffer, length + 1, "%u", knumber->value);
}

void
repbeat(KBeat *kbeat, char *buffer)
{
    if (kbeat->value != 0)
        strcpy(buffer, "+");
    else
        strcpy(buffer, "-");
}

void
repblist(KBlist *kblist, char *buffer)
{
    char *brep;
    KBnode *kbnode = kblist->first;

    brep = buffer;
    *brep = '[';
    while (kbnode != NULL) {
        brep++;
        if (kbnode->beat != 0)
            *brep = '+';
        else
            *brep = '-';
        kbnode = kbnode->next;
    }
    brep++;
    *brep = ']';
    brep++;
    *brep = '\0';
}

void
repname(KName *kname, char *buffer)
{
    strcpy(buffer, kname->name);
}

void
repkob(Kob *kob, char *buffer)
{
    switch(*kob) {
        case EMPTY:
            return;
        case T_NUMBER:
            repnumber((KNumber *) kob, buffer);
            break;
        case T_BEAT:
            repbeat((KBeat *) kob, buffer);
            break;
        case T_BLIST:
            repblist((KBlist *) kob, buffer);
            break;
        case T_NAME:
            repname((KName *) kob, buffer);
            break;
    }
}


KNumber *
cpynumber(KNumber *knumber)
{
    return newnumber(knumber->value);
}

KBeat *
cpybeat(KBeat *kbeat)
{
    return newbeat(kbeat->value);
}

KBlist *
cpyblist(KBlist *kblist)
{
    KBlist *other;

    other = newblist();
    (void) blist_extend(other, kblist);
    return other;
}

KName *
cpyname(KName *kname)
{
    return newname(kname->name);
}


Kob *
cpykob(Kob *kob)
{
    switch(*kob) {
        case EMPTY:
            return (Kob *) NULL;
        case T_NUMBER:
            return (Kob *) cpynumber((KNumber *) kob);
        case T_BEAT:
            return (Kob *) cpybeat((KBeat *) kob);
        case T_BLIST:
            return (Kob *) cpyblist((KBlist *) kob);
        case T_NAME:
            return (Kob *) cpyname((KName *) kob);
    }
    return NULL;
}


int
blist_insert(KBlist *kblist, int index, int value)
{
    KBnode *kbnode;

    if (index < 0)
        index += kblist->length;
    if (index < 0 || index > (int) kblist->length)
        return 0;
    else if (index == 0) {
        kbnode = newbnode(value);
        kbnode->next = kblist->first;
        kblist->first = kbnode;
        if (kblist->length == 0U)
            kblist->last = kbnode;
    }
    else if (index == (int) kblist->length) {
        kbnode = newbnode(value);
        kbnode->next = NULL;
        kblist->last->next = kbnode;
        kblist->last = kbnode;
    }
    else {
        int curidx = 0;
        KBnode *prev = kblist->first;
        KBnode *next;

        while (curidx < index - 1) {
            prev = prev->next;
            curidx++;
        }
        next = prev->next;
        prev->next = newbnode(value);
        prev->next->next = next;
    }
    kblist->length++;
    return 1;
}

int
blist_remove(KBlist *kblist, int index)
{
    int value;

    if (index < 0)
        index += kblist->length;
    if (index < 0 || index >= (int) kblist->length)
        return -1;
    if (index == 0) {
        if (kblist->first == kblist->last) {
            value = kblist->first->beat;
            delbnode(&kblist->first);
            kblist->first = NULL;
            kblist->last = NULL;
        }
        else {
            KBnode *second;

            second = kblist->first->next;
            value = kblist->first->beat;
            delbnode(&kblist->first);
            kblist->first = second;
        }
    }
    else {
        int curidx = 0;
        KBnode *prev = kblist->first;
        KBnode *next;

        while (curidx < index - 1) {
            prev = prev->next;
            curidx++;
        }
        next = prev->next->next;
        value = prev->next->beat;
        delbnode(&prev->next);
        prev->next = next;
    }
    kblist->length--;
    return value;
}

int
blist_extend(KBlist *kblist, KBlist *other)
{
    unsigned int count = 0;
    unsigned int length = other->length;
    KBnode *ecur = other->first;

    if (kblist->length == 0U) {
        kblist->first = kblist->last = newbnode(ecur->beat);
        kblist->length++;
        count++;
        ecur = ecur->next;
    }
    for (; count < length; count++) {
        kblist->last->next = newbnode(ecur->beat);
        kblist->last = kblist->last->next;
        kblist->length++;
        ecur = ecur->next;
    }
    return (int) length;
}
