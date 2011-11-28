typedef struct {
    unsigned int delta;
    char note;
    char volume;
} KEvent;

typedef struct KTrack {
    KEvent *event;
    struct KTrack *next;
} KTrack;

KEvent *newevent(unsigned int delta, char note, char volume);
void delevent(KEvent **kevent);
KTrack *trackfromblist(KBlist *kblist, char note, char volume,
                       unsigned int ticksperbeat);
void deltrack(KTrack **ktrack);
KTrack *trackmix(KTrack *ktrack_a, KTrack *ktrack_b);
