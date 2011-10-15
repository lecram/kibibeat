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
    while (*kob == T_NAME)
        if (tget(kruntime->nable, ((KName *) kob)->name, &kob) == 0)
            return E_NAME;
    if (*kob != T_BLIST)
        return E_TYPE;
    kbnode = ((KBlist *) kob)->first;
    while (kbnode != NULL) {
        kbnode->beat = 1 - kbnode->beat;
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
    while (*kob == T_NAME)
        if (tget(kruntime->nable, ((KName *) kob)->name, &kob) == 0)
            return E_NAME;
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

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    return E_OK;
}

Error
kand(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    return E_OK;
}

Error
krepeat(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    return E_OK;
}

Error
kpattern(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    return E_OK;
}

Error
krleft(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    return E_OK;
}

Error
krright(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b;

    if (slength(kruntime->stack) < 2U)
        return E_ARITY;
    kob_b = kruntime->stack->kob;
    kob_a = kruntime->stack->next->kob;
    return E_OK;
}

Error
kpleft(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b, *kob_c;

    if (slength(kruntime->stack) < 3U)
        return E_ARITY;
    kob_c = kruntime->stack->kob;
    kob_b = kruntime->stack->next->kob;
    kob_a = kruntime->stack->next->next->kob;
    return E_OK;
}

Error
kpright(KRuntime *kruntime)
{
    Kob *kob_a, *kob_b, *kob_c;

    if (slength(kruntime->stack) < 3U)
        return E_ARITY;
    kob_c = kruntime->stack->kob;
    kob_b = kruntime->stack->next->kob;
    kob_a = kruntime->stack->next->next->kob;
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
    while (*kob_b == T_NAME)
        if (tget(kruntime->nable, ((KName *) kob_b)->name, &kob_b) == 0)
            return E_NAME;
    if (*kob_a != T_NAME)
        return E_TYPE;
    tset(kruntime->nable, ((KName *) kob_a)->name, cpykob(kob_b));
    kruntime->stack->next = kruntime->stack->next->next;
    delname((KName **) &kob_a);
    return E_OK;
}
