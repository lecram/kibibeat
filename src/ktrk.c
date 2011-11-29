#include <stdlib.h>

#include "kob0.h"
#include "ktrk.h"

KEvent *
newevent(unsigned int delta, char note, char volume)
{
    KEvent *kevent;

    kevent = (KEvent *) malloc(sizeof(KEvent));
    kevent->delta = delta;
    kevent->note = note;
    kevent->volume = volume;
    return kevent;
}

void
delevent(KEvent **kevent)
{
    free(*kevent);
    *kevent = NULL;
}

KTrack *
trackfromblist(KBlist *kblist, char note, char volume,
               unsigned int ticksperbeat)
{
    unsigned int delta;
    KBnode *kbnode;
    KTrack *onstrack, *onsnode;
    KTrack *offstrack, *offsnode;
    KEvent *kevent;

    delta = 0;
    onstrack = offstrack = NULL;
    kbnode = kblist->first;
    while (kbnode != NULL) {
        if (kbnode->beat) {
            kevent = newevent(delta, note, volume);
            if (onstrack == NULL)
                onstrack = onsnode = (KTrack *) malloc(sizeof(KTrack));
            else {
                onsnode->next = (KTrack *) malloc(sizeof(KTrack));
                onsnode = onsnode->next;
            }
            onsnode->event = kevent;
            onsnode->next = NULL;
            kevent = newevent(delta, note, 0);
            if (offstrack == NULL) {
                offstrack = offsnode = (KTrack *) malloc(sizeof(KTrack));
                kevent->delta += ticksperbeat;
            }
            else {
                offsnode->next = (KTrack *) malloc(sizeof(KTrack));
                offsnode = offsnode->next;
            }
            offsnode->event = kevent;
            offsnode->next = NULL;
            delta = ticksperbeat;
        }
        else
            delta += ticksperbeat;
        kbnode = kbnode->next;
    }
    return trackmix(onstrack, offstrack);
}

void
deltrack(KTrack **ktrack)
{
    KTrack *a, *b;

    a = *ktrack;
    while (a != NULL) {
        b = a->next;
        delevent(&a->event);
        free(a);
        a = b;
    }
    *ktrack = NULL;
}

int
closer(KTrack *ktrack_a, KTrack *ktrack_b)
{
    if (ktrack_a == NULL)
        return 1;
    if (ktrack_b == NULL)
        return 0;
    if (ktrack_a->event->delta < ktrack_b->event->delta)
        return 0;
    else
        return 1;
}

KTrack *
trackmix(KTrack *ktrack_a, KTrack *ktrack_b)
{
    unsigned int restime;
    unsigned int delta;
    KEvent *kevent;
    KTrack *mixtrack, *mixnode;

    restime = 0;
    mixtrack = NULL;
    while (ktrack_a != ktrack_b) {
        if (closer(ktrack_a, ktrack_b) == 0) {
            kevent = ktrack_a->event;
            ktrack_a = ktrack_a->next;
            if (ktrack_a != NULL)
                ktrack_a->event->delta += kevent->delta;
        }
        else {
            kevent = ktrack_b->event;
            ktrack_b = ktrack_b->next;
            if (ktrack_b != NULL)
                ktrack_b->event->delta += kevent->delta;
        }
        delta = kevent->delta - restime;
        kevent->delta = delta;
        if (mixtrack == NULL)
            mixtrack = mixnode = (KTrack *) malloc(sizeof(KTrack));
        else {
            mixnode->next = (KTrack *) malloc(sizeof(KTrack));
            mixnode = mixnode->next;
        }
        mixnode->event = kevent;
        mixnode->next = NULL;
        restime += delta;
    }
    deltrack(&ktrack_a);
    deltrack(&ktrack_b);
    return mixtrack;
}
