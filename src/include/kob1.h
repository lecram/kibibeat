#define SLPROB   0.5
#define SLHEIGHT 10

typedef struct KEntry {
    char *name;
    Kob *value;
    struct KEntry **next;
} KEntry;

typedef struct {
    int level;
    KEntry *header;
} KNable;

typedef struct KStack {
    Kob *kob;
    struct KStack *next;
} KStack;

double krandom();
int trndlevel(KNable *knable);

KEntry *newentry(int level, char *name, Kob *value);
void delentry(KEntry **kentry);
size_t repsizeentry(KEntry *kentry);
void repentry(KEntry *kentry, char *buffer);

KNable *newnable();
void delnable(KNable **knable);
size_t repsizenable(KNable *knable);
void repnable(KNable *knable, KBuffer *kbuffer);

void tset(KNable *knable, char *name, Kob *value);
int tget(KNable *knable, char *name, Kob **value);
int tremove(KNable *knable, char *name);

KStack *newstack();
void delstack(KStack **kstack);
size_t repsizestack(KStack *kstack);
void repstack(KStack *kstack, KBuffer *kbuffer);

unsigned int slength(KStack *kstack);
void spush(KStack **kstack, Kob *kob);
Kob *spop(KStack **kstack);
