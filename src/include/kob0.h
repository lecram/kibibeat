#define MAX_NAME_LEN 32

typedef enum {EMPTY, T_NUMBER, T_BEAT, T_BLIST, T_NAME} Kob;

typedef struct {
    Kob type;
    unsigned int value;
} KNumber;

typedef struct {
    Kob type;
    int value;
} KBeat;

typedef struct KBnode {
    int beat;
    struct KBnode *next;
} KBnode;

typedef struct {
    Kob type;
    unsigned int length;
    KBnode *first;
    KBnode *last;
} KBlist;

typedef struct {
    Kob type;
    char name[MAX_NAME_LEN];
} KName;

typedef struct {
    size_t size;
    char *buffer;
} KBuffer;

KNumber *newnumber(unsigned int value);
KBeat *newbeat(int value);
KBnode *newbnode(int value);
KBlist *newblist();
KName *newname(char *name);

void delnumber(KNumber **knumber);
void delbeat(KBeat **kbeat);
void delbnode(KBnode **kbnode);
void delblist(KBlist **kblist);
void delname(KName **kname);

void delkob(Kob **kob);

KBuffer *newbuffer();
size_t setupbuffer(KBuffer *kbuffer, size_t size);
void delbuffer(KBuffer **kbuffer);

size_t repsizenumber(KNumber *knumber);
size_t repsizebeat(KBeat *kbeat);
size_t repsizeblist(KBlist *kblist);
size_t repsizename(KName *kname);

size_t repsizekob(Kob *kob);

void repnumber(KNumber *knumber, char *buffer);
void repbeat(KBeat *kbeat, char *buffer);
void repblist(KBlist *kblist, char *buffer);
void repname(KName *kname, char *buffer);

void repkob(Kob *kob, char *buffer);

KNumber *cpynumber(KNumber *knumber);
KBeat *cpybeat(KBeat *kbeat);
KBlist *cpyblist(KBlist *kblist);
KName *cpyname(KName *kname);

Kob *cpykob(Kob *kob);

int blist_insert(KBlist *kblist, int index, int value);
int blist_remove(KBlist *kblist, int index);
int blist_extend(KBlist *kblist, KBlist *other);
