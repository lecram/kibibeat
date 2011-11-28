typedef struct {
    KStack *stack;
    KNable *nable;
    KTrack *track;
    int comment;
} KRuntime;

KRuntime *newruntime();
void delruntime(KRuntime **kruntime);

Error kprocess(KRuntime *kruntime, char *token);
