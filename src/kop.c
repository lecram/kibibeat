#include <stdlib.h>

#include "ker.h"
#include "kob0.h"
#include "kob1.h"
#include "krt.h"
#include "kop.h"

Error
kinvert(KRuntime *kruntime)
{
    Kob *kob;
    KBnode *kbnode;

    if (slength(kruntime->stack) < 1U)
        return E_ARITY;
    kob = kruntime->stack->kob;
    if (*kob == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob)->name, &kob) == 1)
            kruntime->stack->kob = kob = cpykob(kob);
        else
            return E_NAME;
    }
    if (*kob != T_BLIST)
        return E_TYPE;
    kbnode = ((KBlist *) kob)->first;
    while (kbnode != NULL) {
        kbnode->beat = !kbnode->beat;
        kbnode = kbnode->next;
    }
    return E_OK;
}

Error
kclear(KRuntime *kruntime)
{
    Kob *kob;
    KBnode *kbnode;

    if (slength(kruntime->stack) < 1U)
        return E_ARITY;
    kob = kruntime->stack->kob;
    if (*kob == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob)->name, &kob) == 1)
            kruntime->stack->kob = kob = cpykob(kob);
        else
            return E_NAME;
    }
    if (*kob != T_BLIST)
        return E_TYPE;
    kbnode = ((KBlist *) kob)->first;
    while (kbnode != NULL) {
        kbnode->beat = 0;
        kbnode = kbnode->next;
    }
    return E_OK;
}

Error
kor(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;
    KBnode *kbnode_a, *kbnode_b;

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
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_BLIST)
        return E_TYPE;
    kbnode_a = ((KBlist *) kob_a)->first;
    kbnode_b = ((KBlist *) kob_b)->first;
    while (kbnode_a != NULL  &&  kbnode_b != NULL) {
        kbnode_a->beat = kbnode_a->beat | kbnode_b->beat;
        kbnode_a = kbnode_a->next;
        kbnode_b = kbnode_b->next;
    }
    kruntime->stack = kruntime->stack->next;
    kruntime->stack->kob = kob_a;
    delblist((KBlist **) &kob_b);
    return E_OK;
}

Error
kand(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;
    KBnode *kbnode_a, *kbnode_b;

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    if (*kob_b == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_b)->name, &kob_b) == 1)
            kruntime->stack->kob = kob_b = cpykob(kob_b);
        else
            return E_NAME;
    }
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_BLIST)
        return E_TYPE;
    kbnode_a = ((KBlist *) kob_a)->first;
    kbnode_b = ((KBlist *) kob_b)->first;
    while (kbnode_a != NULL  &&  kbnode_b != NULL) {
        kbnode_a->beat = kbnode_a->beat & kbnode_b->beat;
        kbnode_a = kbnode_a->next;
        kbnode_b = kbnode_b->next;
    }
    kruntime->stack = kruntime->stack->next;
    kruntime->stack->kob = kob_a;
    delblist((KBlist **) &kob_b);
    return E_OK;
}

Error
krepeat(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;
    KBlist *kblist;

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
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_NUMBER)
        return E_TYPE;
    kblist = cpyblist((KBlist *) kob_a);
    for (; ((KNumber *) kob_b)->value > 1; ((KNumber *) kob_b)->value--)
        blist_extend((KBlist *) kob_a, kblist);
    delblist(&kblist);
    kruntime->stack = kruntime->stack->next;
    kruntime->stack->kob = kob_a;
    delnumber((KNumber **) &kob_b);
    return E_OK;
}

Error
kpattern(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;
    KBlist *silence, *result;
    KBnode *kbnode;

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
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_BLIST)
        return E_TYPE;
    silence = newblist();
    kbnode = ((KBlist *) kob_a)->first;
    while (kbnode != NULL) {
        blist_insert(silence, 0, 0);
        kbnode = kbnode->next;
    }
    result = newblist();
    kbnode = ((KBlist *) kob_b)->first;
    while (kbnode != NULL) {
        if (kbnode->beat)
            blist_extend(result, (KBlist *) kob_a);
        else
            blist_extend(result, silence);
        kbnode = kbnode->next;
    }
    delblist(&silence);
    kruntime->stack = kruntime->stack->next;
    kruntime->stack->kob = (Kob *) result;
    delblist((KBlist **) &kob_b);
    delblist((KBlist **) &kob_a);
    return E_OK;
}

Error
krleft(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;
    KBlist *kblist;

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
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_NUMBER)
        return E_TYPE;
    kblist = (KBlist *) kob_a;
    for (; ((KNumber *) kob_b)->value > 0; ((KNumber *) kob_b)->value--)
        blist_insert(kblist, kblist->length, blist_remove(kblist, 0));
    kruntime->stack = kruntime->stack->next;
    kruntime->stack->kob = kob_a;
    delnumber((KNumber **) &kob_b);
    return E_OK;
}

Error
krright(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;
    KBlist *kblist;

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
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_NUMBER)
        return E_TYPE;
    kblist = (KBlist *) kob_a;
    for (; ((KNumber *) kob_b)->value > 0; ((KNumber *) kob_b)->value--)
        blist_insert(kblist, 0, blist_remove(kblist, -1));
    kruntime->stack = kruntime->stack->next;
    kruntime->stack->kob = kob_a;
    delnumber((KNumber **) &kob_b);
    return E_OK;
}

Error
kpleft(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b, *kob_c;
    KBlist *kblist;

    if (slength(kruntime->stack) < 3U)
        return E_ARITY;
    kob_c = kruntime->stack->kob;
    kob_b = kruntime->stack->next->kob;
    kob_a = kruntime->stack->next->next->kob;
    if (*kob_c == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_c)->name, &kob_c) == 1)
            kob_c = cpykob(kob_c);
        else
            return E_NAME;
    }
    if (*kob_b == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_b)->name, &kob_b) == 1)
            kob_b = cpykob(kob_b);
        else
            return E_NAME;
    }
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_NUMBER  ||  *kob_c != T_BEAT)
        return E_TYPE;
    kblist = (KBlist *) kob_a;
    for (; ((KNumber *) kob_b)->value > 0; ((KNumber *) kob_b)->value--)
        blist_insert(kblist, kblist->length, ((KBeat *) kob_c)->value);
    kruntime->stack = kruntime->stack->next->next;
    delnumber((KNumber **) &kob_b);
    delbeat((KBeat **) &kob_c);
    return E_OK;
}

Error
kpright(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b, *kob_c;
    KBlist *kblist;

    if (slength(kruntime->stack) < 3U)
        return E_ARITY;
    kob_c = kruntime->stack->kob;
    kob_b = kruntime->stack->next->kob;
    kob_a = kruntime->stack->next->next->kob;
    if (*kob_c == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_c)->name, &kob_c) == 1)
            kob_c = cpykob(kob_c);
        else
            return E_NAME;
    }
    if (*kob_b == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_b)->name, &kob_b) == 1)
            kob_b = cpykob(kob_b);
        else
            return E_NAME;
    }
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_NUMBER  ||  *kob_c != T_BEAT)
        return E_TYPE;
    kblist = (KBlist *) kob_a;
    for (; ((KNumber *) kob_b)->value > 0; ((KNumber *) kob_b)->value--)
        blist_insert(kblist, 0, ((KBeat *) kob_c)->value);
    kruntime->stack = kruntime->stack->next->next;
    delnumber((KNumber **) &kob_b);
    delbeat((KBeat **) &kob_c);
    return E_OK;
}

Error
ksubdiv(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b, *kob_c;

    if (slength(kruntime->stack) < 3U)
        return E_ARITY;
    kob_c = kruntime->stack->kob;
    kob_b = kruntime->stack->next->kob;
    kob_a = kruntime->stack->next->next->kob;
    if (*kob_c == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_c)->name, &kob_c) == 1)
            kob_c = cpykob(kob_c);
        else
            return E_NAME;
    }
    if (*kob_b == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_b)->name, &kob_b) == 1)
            kob_b = cpykob(kob_b);
        else
            return E_NAME;
    }
    if (*kob_a == T_NAME) {
        if (tget(kruntime->nable, ((KName *) kob_a)->name, &kob_a) == 1)
            kob_a = cpykob(kob_a);
        else
            return E_NAME;
    }
    if (*kob_a != T_BLIST  ||  *kob_b != T_NUMBER  ||  *kob_c != T_NUMBER)
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
