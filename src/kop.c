#include <stdlib.h>

#include "ker.h"
#include "kob0.h"
#include "kob1.h"
#include "krt.h"
#include "kop.h"

Error
kgetargs(KRuntime *kruntime, int arity, Kob *args[])
{
    if (slength(kruntime->stack) < (unsigned int) arity)
        return E_ARITY;
    for (arity--; arity >= 0; arity--) {
        Kob *kob;

        kob = spop(&kruntime->stack);
        if (*kob == T_NAME) {
            if (tget(kruntime->nable, ((KName *) kob)->name, &kob) == 1)
                kob = cpykob(kob);
            else
                return E_NAME;
        }
        args[arity] = kob;
    }
    return E_OK;
}

Error
kinvert(KRuntime *kruntime)
{
    Kob *args[1];
    Error error;
    KBnode *kbnode;

    error = kgetargs(kruntime, 1, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST)
        return E_TYPE;
    kbnode = ((KBlist *) args[0])->first;
    while (kbnode != NULL) {
        kbnode->beat = !kbnode->beat;
        kbnode = kbnode->next;
    }
    spush(&kruntime->stack, args[0]);
    return E_OK;
}

Error
kclear(KRuntime *kruntime)
{
    Kob *args[1];
    Error error;
    KBnode *kbnode;

    error = kgetargs(kruntime, 1, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST)
        return E_TYPE;
    kbnode = ((KBlist *) args[0])->first;
    while (kbnode != NULL) {
        kbnode->beat = 0;
        kbnode = kbnode->next;
    }
    spush(&kruntime->stack, args[0]);
    return E_OK;
}

Error
kor(KRuntime *kruntime)
{
    Kob *args[2];
    Error error;
    KBnode *kbnode_a, *kbnode_b;

    error = kgetargs(kruntime, 1, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||  *args[1] != T_BLIST)
        return E_TYPE;
    kbnode_a = ((KBlist *) args[0])->first;
    kbnode_b = ((KBlist *) args[1])->first;
    while (kbnode_a != NULL  &&  kbnode_b != NULL) {
        kbnode_a->beat = kbnode_a->beat | kbnode_b->beat;
        kbnode_a = kbnode_a->next;
        kbnode_b = kbnode_b->next;
    }
    spush(&kruntime->stack, args[0]);
    delblist((KBlist **) &args[1]);
    return E_OK;
}

Error
kand(KRuntime *kruntime)
{
    Kob *args[2];
    Error error;
    KBnode *kbnode_a, *kbnode_b;

    error = kgetargs(kruntime, 1, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||  *args[1] != T_BLIST)
        return E_TYPE;
    kbnode_a = ((KBlist *) args[0])->first;
    kbnode_b = ((KBlist *) args[1])->first;
    while (kbnode_a != NULL  &&  kbnode_b != NULL) {
        kbnode_a->beat = kbnode_a->beat & kbnode_b->beat;
        kbnode_a = kbnode_a->next;
        kbnode_b = kbnode_b->next;
    }
    spush(&kruntime->stack, args[0]);
    delblist((KBlist **) &args[1]);
    return E_OK;
}

Error
krepeat(KRuntime *kruntime)
{
    Kob *args[2];
    Error error;
    KBlist *kblist;

    error = kgetargs(kruntime, 2, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||  *args[1] != T_NUMBER)
        return E_TYPE;
    kblist = cpyblist((KBlist *) args[0]);
    for (; ((KNumber *) args[1])->value > 1; ((KNumber *) args[1])->value--)
        blist_extend(kblist, (KBlist *) args[0]);
    spush(&kruntime->stack, (Kob *) kblist);
    delblist((KBlist **) &args[0]);
    delnumber((KNumber **) &args[1]);
    return E_OK;
}

Error
kpattern(KRuntime *kruntime)
{
    Kob *args[2];
    Error error;
    KBlist *silence, *result;
    KBnode *kbnode;

    error = kgetargs(kruntime, 2, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||  *args[1] != T_BLIST)
        return E_TYPE;
    silence = newblist();
    kbnode = ((KBlist *) args[0])->first;
    while (kbnode != NULL) {
        blist_insert(silence, 0, 0);
        kbnode = kbnode->next;
    }
    result = newblist();
    kbnode = ((KBlist *) args[1])->first;
    while (kbnode != NULL) {
        if (kbnode->beat)
            blist_extend(result, (KBlist *) args[0]);
        else
            blist_extend(result, silence);
        kbnode = kbnode->next;
    }
    spush(&kruntime->stack, (Kob *) result);
    delblist(&silence);
    delblist((KBlist **) &args[0]);
    delblist((KBlist **) &args[1]);
    return E_OK;
}

Error
krleft(KRuntime *kruntime)
{
    Kob *args[2];
    Error error;
    KBlist *kblist;

    error = kgetargs(kruntime, 2, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||  *args[1] != T_NUMBER)
        return E_TYPE;
    kblist = (KBlist *) args[0];
    for (; ((KNumber *) args[1])->value > 0; ((KNumber *) args[1])->value--)
        blist_insert(kblist, kblist->length, blist_remove(kblist, 0));
    spush(&kruntime->stack, args[0]);
    delnumber((KNumber **) &args[1]);
    return E_OK;
}

Error
krright(KRuntime *kruntime)
{
    Kob *args[2];
    Error error;
    KBlist *kblist;

    error = kgetargs(kruntime, 2, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||  *args[1] != T_NUMBER)
        return E_TYPE;
    kblist = (KBlist *) args[0];
    for (; ((KNumber *) args[1])->value > 0; ((KNumber *) args[1])->value--)
        blist_insert(kblist, 0, blist_remove(kblist, -1));
    spush(&kruntime->stack, args[0]);
    delnumber((KNumber **) &args[1]);
    return E_OK;
}

Error
kpleft(KRuntime *kruntime)
{
    Kob *args[3];
    Error error;
    KBlist *kblist;

    error = kgetargs(kruntime, 3, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||  *args[1] != T_NUMBER  ||  *args[2] != T_BEAT)
        return E_TYPE;
    kblist = (KBlist *) args[0];
    for (; ((KNumber *) args[1])->value > 0; ((KNumber *) args[1])->value--)
        blist_insert(kblist, kblist->length, ((KBeat *) args[2])->value);
    spush(&kruntime->stack, args[0]);
    delnumber((KNumber **) &args[1]);
    delbeat((KBeat **) &args[2]);
    return E_OK;
}

Error
kpright(KRuntime *kruntime)
{
    Kob *args[3];
    Error error;
    KBlist *kblist;

    error = kgetargs(kruntime, 3, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||
        *args[1] != T_NUMBER  ||
        *args[2] != T_BEAT)
        return E_TYPE;
    kblist = (KBlist *) args[0];
    for (; ((KNumber *) args[1])->value > 0; ((KNumber *) args[1])->value--)
        blist_insert(kblist, 0, ((KBeat *) args[2])->value);
    spush(&kruntime->stack, args[0]);
    delnumber((KNumber **) &args[1]);
    delbeat((KBeat **) &args[2]);
    return E_OK;
}

Error
ksubdiv(KRuntime *kruntime)
{
    Kob *args[3];
    Error error;

    error = kgetargs(kruntime, 3, args);
    if (error != E_OK) return error;
    if (*args[0] != T_BLIST  ||
        *args[1] != T_NUMBER  ||
        *args[2] != T_NUMBER)
        return E_TYPE;
    return E_OK;
}

Error
kset(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    if (*kob_b == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_b)->name, &kob_b) == 1)
            kob_b = cpykob(kob_b);
        else
            return E_NAME;
    }
    if (*kob_a != T_NAME)
        return E_TYPE;
    tset(kruntime->nable, ((KName *) kob_a)->name, kob_b);
    kruntime->stack = kruntime->stack->next->next;
    delname((KName **) &kob_a);
    return E_OK;
}
