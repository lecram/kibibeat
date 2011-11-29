#include <stdlib.h>
#include <stdio.h>

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

void
writebigendian(unsigned int value, unsigned int length, FILE *file)
{
    unsigned int modulo;

    modulo = 1 << ((length - 1) * 8);
    while (modulo > 0) {
        fputc(value / modulo, file);
        value %= modulo;
        modulo >>= 8;
    }
}

void
writevlv(unsigned int value, FILE *file)
{
    unsigned int modulo;
    int position = 3;
    int byte;

    modulo = 1 << (3 * 7);
    while (modulo > 0) {
        byte = value / modulo;
        if (byte || !position) {
            if (position)
                byte |= 0x80;
            fputc(byte, file);
        }
        value %= modulo;
        modulo >>= 7;
        position--;
    }
}

void
writesmf(KTrack *ktrack, unsigned int ticksperquarter, FILE *file)
{
    KTrack *node;
    unsigned int tracklen;
    unsigned int delta;
    int running_status;

    tracklen = 5;
    node = ktrack;
    while (node != NULL) {
        delta = node->event->delta;
        while (delta >>= 7)
            tracklen++;
        tracklen += 3;
        node = node->next;
    }

    /* ---- Header Chunk ---- */
    /* Standard MIDI File signature. */
    fwrite("MThd", 1, 4, file);
    /* Header length (always 6). */
    fwrite("\x00\x00\x00\x06", 1, 4, file);
    /* Format (type 1). */
    fwrite("\x00\x01", 1, 2, file);
    /* Number of tracks (1). */
    fwrite("\x00\x01", 1, 2, file);
    /* Ticks per quarter note (360). */
    writebigendian(ticksperquarter, 2U, file);
    /* ---------------------- */

    /* ---- Track Chunk ---- */
    /* Track signature. */
    fwrite("MTrk", 1, 4, file);
    /* Track length. */
    writebigendian(ticksperquarter, 4U, file);
    /* Events. */
    running_status = 0;
    node = ktrack;
    while (node != NULL) {
        /* Note On. */
        /* Delta. */
        writevlv(node->event->delta, file);
        if (!running_status) {
            /* Event type (Note On = 9) and channel (Drums = 9). */
            /* This is the status byte. */
            fwrite("\x99", 1, 1, file);
            running_status = 1;
        }
        /* Note number. */
        fputc((int) node->event->note, file);
        /* Velocity. */
        fputc((int) node->event->volume, file);
        node = node->next;
    }
    /* Track Out. */
    fwrite("\x00\xFF\x2F\x00", 1, 4, file);
    /* --------------------- */
}
