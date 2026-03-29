/**
 * Reverse Engineered by TekuConcept on April 28, 2021
 * Extended AE route table — 5-field nodes (IntTime, Again, Dgain,
 * IspDgain, IrisFNOLin) with 48-byte nodes, same pool architecture
 * as hi_ae_route.c.
 */

#include <stdio.h>
#include <string.h>

#include "re_hi_ae_adp.h"
#include "hi_comm_isp.h"
#include "hi_ae_comm.h"

/* ========================================================================== */
/* Internal route_ex node: 48 bytes (0x30)                                    */
/* ========================================================================== */

typedef struct hiAE_ROUTEEX_LIST_S {
    struct hiAE_ROUTEEX_LIST_S *pNext;
    struct hiAE_ROUTEEX_LIST_S *pPrev;
} AE_ROUTEEX_LIST_S;

typedef struct hiAE_ROUTEEX_NODE_S {
    HI_U32 u32Values[5];          /* 0x00: inttime, again, dgain, ispdgain, irisfnoLin */
    HI_U32 u32Reserved;           /* 0x14 */
    HI_U64 u64Multi;              /* 0x18: product of exposure values */
    HI_U32 u32Stgy;               /* 0x20 */
    HI_U32 u32Stgy2;              /* 0x24 */
    AE_ROUTEEX_LIST_S stList;     /* 0x28 */
} AE_ROUTEEX_NODE_S;

#define AE_ROUTEEX_NODE_FROM_LIST(ptr) \
    ((AE_ROUTEEX_NODE_S *)((char *)(ptr) - \
        (unsigned long)(&((AE_ROUTEEX_NODE_S *)0)->stList)))

typedef struct hiAE_ROUTEEX_MGR_S {
    AE_ROUTEEX_NODE_S astNodes[ISP_AE_ROUTE_MAX_NODES]; /* 0x000 */
    HI_U32            u32BusyCnt;     /* 0x300 */
    HI_U32            u32FreeCnt;     /* 0x304 */
    AE_ROUTEEX_LIST_S stFreeHead;    /* 0x308 */
    AE_ROUTEEX_LIST_S stBusyHead;    /* 0x310 */
} AE_ROUTEEX_MGR_S;

extern ISP_AE_CTX_S g_astAeCtx[AE_CTX_SIZE];

/* Boundary helpers from hi_ae_route.c */
extern HI_U32 AeBoundariesCheck(HI_U32 u32Value, HI_U32 u32Min, HI_U32 u32Max);
extern HI_U64 AeRatioCalculate(HI_U64 u64Denom, HI_U32 u32Factor, HI_U64 u64Num);

/* ========================================================================== */
/* Linked list operations                                                     */
/* ========================================================================== */

void AeRouteExPutBusy(AE_ROUTEEX_MGR_S *pMgr, AE_ROUTEEX_NODE_S *pNode)
{
    AE_ROUTEEX_LIST_S *pPrev = pMgr->stBusyHead.pPrev;
    pMgr->u32BusyCnt++;
    pNode->stList.pNext = &pMgr->stBusyHead;
    pNode->stList.pPrev = pPrev;
    pPrev->pNext = &pNode->stList;
    pMgr->stBusyHead.pPrev = &pNode->stList;
}

void AeRouteExPutBusyByPos(AE_ROUTEEX_MGR_S *pMgr, AE_ROUTEEX_NODE_S *pNode,
    AE_ROUTEEX_NODE_S *pPos)
{
    AE_ROUTEEX_LIST_S *pAfter = pPos->stList.pNext;
    pMgr->u32BusyCnt++;
    pAfter->pPrev = &pNode->stList;
    pNode->stList.pPrev = &pPos->stList;
    pNode->stList.pNext = pAfter;
    pPos->stList.pNext = &pNode->stList;
}

AE_ROUTEEX_NODE_S *AeRouteExGetFree(AE_ROUTEEX_MGR_S *pMgr)
{
    AE_ROUTEEX_LIST_S *pFirst = pMgr->stFreeHead.pNext;
    if (pFirst == &pMgr->stFreeHead) {
        fprintf(stderr, "[Func]:%s [Line]:%d [Info]:free list empty\n",
            __FUNCTION__, __LINE__);
        return HI_NULL;
    }
    pMgr->u32FreeCnt--;
    pFirst->pPrev->pNext = pFirst->pNext;
    pFirst->pNext->pPrev = pFirst->pPrev;
    return AE_ROUTEEX_NODE_FROM_LIST(pFirst);
}

void AeRouteExPutFree(AE_ROUTEEX_MGR_S *pMgr, AE_ROUTEEX_NODE_S *pNode)
{
    AE_ROUTEEX_LIST_S *pPrev = pMgr->stFreeHead.pPrev;
    pMgr->u32FreeCnt++;
    pNode->stList.pNext = &pMgr->stFreeHead;
    pNode->stList.pPrev = pPrev;
    pPrev->pNext = &pNode->stList;
    pMgr->stFreeHead.pPrev = &pNode->stList;
}

/* ========================================================================== */
/* Node operations                                                            */
/* ========================================================================== */

HI_S32 AeRouteExNodeMultiCheck(AE_ROUTEEX_NODE_S *pNode,
    AE_ROUTEEX_NODE_S *pNext, HI_U32 u32Cnt,
    HI_U32 *pu32Max, HI_U64 u64NewMulti,
    HI_U32 *pu32Min, HI_U64 u64OldMulti)
{
    HI_U32 *pu32Limit;
    HI_U32 i;

    pu32Limit = (u64OldMulti <= u64NewMulti) ? pu32Min : pu32Max;

    if (pNode->u32Stgy != 3 && pNext->u32Stgy2 != 3)
        goto store;
    if (u32Cnt == 0)
        goto store;

    for (i = 0; i < u32Cnt; i++) {
        if (pNode->u32Values[i] != pu32Limit[i])
            goto recalc;
    }

store:
    pNode->u64Multi = u64OldMulti;
    return 0;

recalc:
    {
        HI_U64 u64Denom = (u64OldMulti == 0) ? 1 : u64OldMulti;
        HI_U64 u64Ratio = AeRatioCalculate(u64Denom, pNode->u32Values[i], u64NewMulti);
        HI_U32 u32Clamped = AeBoundariesCheck((HI_U32)u64Ratio, pu32Min[i], pu32Max[i]);
        pNode->u32Values[i] = u32Clamped;

        if ((HI_U32)u64Ratio != u32Clamped) {
            HI_U64 u64Product = 1;
            HI_U32 j;
            for (j = 0; j < u32Cnt; j++)
                u64Product *= pNode->u32Values[j];
            pNode->u64Multi = u64Product;
        } else {
            pNode->u64Multi = u64NewMulti;
        }
    }
    return 0;
}

HI_S32 AeRouteExNodeRecurit(AE_ROUTEEX_NODE_S *pNode,
    AE_ROUTEEX_NODE_S *pNext, AE_ROUTEEX_MGR_S *pMgr, HI_U32 u32Cnt)
{
    HI_U32 u32LessCnt = 0;
    HI_U32 u32SplitField;
    HI_U32 i;
    AE_ROUTEEX_NODE_S *pNew;

    if (pNode->u64Multi == pNext->u64Multi || u32Cnt == 0)
        return 0;

    for (i = 0; i < u32Cnt; i++) {
        if (pNode->u32Values[i] < pNext->u32Values[i])
            u32LessCnt++;
    }
    if (u32LessCnt <= 1)
        return 0;

    for (u32SplitField = 0; u32SplitField < u32Cnt; u32SplitField++) {
        if (pNode->u32Values[u32SplitField] != pNext->u32Values[u32SplitField])
            if (pNode->u32Stgy == u32SplitField)
                break;
    }
    if (u32SplitField >= u32Cnt)
        return 0;

    pNew = AeRouteExGetFree(pMgr);
    if (pNew == HI_NULL)
        return -1;

    {
        HI_U64 u64Product = 1;
        for (i = 0; i < u32Cnt; i++) {
            pNew->u32Values[i] = (i == u32SplitField) ?
                pNext->u32Values[u32SplitField] : pNode->u32Values[i];
            u64Product *= pNew->u32Values[i];
        }
        pNew->u64Multi = u64Product;
        pNew->u32Stgy2 = pNode->u32Stgy;
        pNew->u32Stgy = 0;
        AeRouteExPutBusyByPos(pMgr, pNew, pNode);
    }
    return 0;
}

HI_S32 AeRouteExNodeStgy(AE_ROUTEEX_NODE_S *pNode,
    AE_ROUTEEX_NODE_S *pNext, HI_U32 u32Cnt)
{
    HI_U32 i;

    if (pNode->u64Multi == pNext->u64Multi) {
        pNode->u32Stgy = 3;
        pNext->u32Stgy2 = 3;
        return 0;
    }
    if (pNode->u64Multi >= pNext->u64Multi || u32Cnt == 0)
        return 0;

    for (i = 0; i < u32Cnt; i++) {
        if (pNode->u32Values[i] != pNext->u32Values[i]) {
            pNode->u32Stgy = i;
            pNext->u32Stgy2 = i;
            return 0;
        }
    }
    return 0;
}

/* ========================================================================== */
/* Route_ex init / update / query / print / limit                             */
/* ========================================================================== */

HI_S32 AeRouteExInit(AE_ROUTEEX_MGR_S *pMgr)
{
    HI_U32 i;

    pMgr->u32BusyCnt = 0;
    pMgr->u32FreeCnt = ISP_AE_ROUTE_MAX_NODES;
    pMgr->stBusyHead.pNext = &pMgr->stBusyHead;
    pMgr->stBusyHead.pPrev = &pMgr->stBusyHead;
    pMgr->stFreeHead.pNext = &pMgr->stFreeHead;
    pMgr->stFreeHead.pPrev = &pMgr->stFreeHead;

    for (i = 0; i < ISP_AE_ROUTE_MAX_NODES; i++) {
        pMgr->astNodes[i].u32Stgy = 4;
        pMgr->astNodes[i].u32Stgy2 = 4;
        AeRouteExPutFree(pMgr, &pMgr->astNodes[i]);
    }
    return 0;
}

HI_S32 AeRouteExUpdate(HI_U32 s32Handle, HI_U32 u32Cnt)
{
    HI_U8 *pCtx = (HI_U8 *)&g_astAeCtx[s32Handle];
    /* Extended route manager at offset 0x988 within AE context */
    AE_ROUTEEX_MGR_S *pMgr = (AE_ROUTEEX_MGR_S *)(pCtx + 0x988);
    /* Extended route attr at offset 0x868 */
    ISP_AE_ROUTE_EX_S *pRoute = (ISP_AE_ROUTE_EX_S *)(pCtx + 0x868);
    HI_U32 u32TotalNum;
    HI_U32 i;
    AE_ROUTEEX_LIST_S *pCurr, *pNext;

    AeRouteExInit(pMgr);

    u32TotalNum = pRoute->u32TotalNum;

    for (i = 0; i < u32TotalNum; i++) {
        AE_ROUTEEX_NODE_S *pNode = AeRouteExGetFree(pMgr);
        ISP_AE_ROUTE_EX_NODE_S *pSrc = &pRoute->astRouteExNode[i];
        HI_U64 u64Product = 1;
        HI_U32 j;

        if (pNode == HI_NULL) break;

        pNode->u32Values[0] = pSrc->u32IntTime;
        pNode->u32Values[1] = pSrc->u32Again;
        pNode->u32Values[2] = pSrc->u32Dgain;
        pNode->u32Values[3] = pSrc->u32IspDgain;

        /* Check bAERouteExValid flag at offset 0xE64 */
        if (*(HI_U8 *)(pCtx + 0xE64))
            pNode->u32Values[4] = pSrc->u32IrisFNOLin;
        else
            pNode->u32Values[4] = (pSrc->enIrisFNO == 0) ? 1 : (1u << pSrc->enIrisFNO);

        for (j = 0; j < u32Cnt; j++)
            u64Product *= pNode->u32Values[j];
        pNode->u64Multi = u64Product;

        AeRouteExPutBusy(pMgr, pNode);
    }

    /* Assign strategies */
    pCurr = pMgr->stBusyHead.pNext;
    pNext = pCurr->pNext;
    while (pCurr != &pMgr->stBusyHead && pNext != &pMgr->stBusyHead) {
        AeRouteExNodeStgy(AE_ROUTEEX_NODE_FROM_LIST(pCurr),
            AE_ROUTEEX_NODE_FROM_LIST(pNext), u32Cnt);
        pCurr = pNext;
        pNext = pNext->pNext;
    }

    return 0;
}

HI_S32 AeRouteExGetJoint(AE_ROUTEEX_MGR_S *pMgr, HI_U64 u64Target,
    AE_ROUTEEX_NODE_S **ppLow, AE_ROUTEEX_NODE_S **ppHigh)
{
    AE_ROUTEEX_LIST_S *pCurr, *pNext;

    *ppLow = HI_NULL;
    *ppHigh = HI_NULL;

    pCurr = pMgr->stBusyHead.pNext;
    pNext = pCurr->pNext;

    while (pCurr != &pMgr->stBusyHead && pNext != &pMgr->stBusyHead) {
        AE_ROUTEEX_NODE_S *pC = AE_ROUTEEX_NODE_FROM_LIST(pCurr);
        AE_ROUTEEX_NODE_S *pN = AE_ROUTEEX_NODE_FROM_LIST(pNext);

        if (pC->u64Multi <= u64Target && u64Target <= pN->u64Multi) {
            *ppLow = pC;
            *ppHigh = pN;
            return 0;
        }
        pCurr = pNext;
        pNext = pNext->pNext;
    }
    return 0;
}

AE_ROUTEEX_NODE_S *AeRouteExGetUpNode(AE_ROUTEEX_MGR_S *pMgr,
    AE_ROUTEEX_NODE_S *pNode)
{
    AE_ROUTEEX_LIST_S *p = pNode->stList.pNext;
    return (p == &pMgr->stBusyHead) ? HI_NULL : AE_ROUTEEX_NODE_FROM_LIST(p);
}

AE_ROUTEEX_NODE_S *AeRouteExGetDwNode(AE_ROUTEEX_MGR_S *pMgr,
    AE_ROUTEEX_NODE_S *pNode)
{
    AE_ROUTEEX_LIST_S *p = pNode->stList.pPrev;
    return (p == &pMgr->stBusyHead) ? HI_NULL : AE_ROUTEEX_NODE_FROM_LIST(p);
}

AE_ROUTEEX_NODE_S *AeRouteExGetFirstNode(AE_ROUTEEX_MGR_S *pMgr)
{
    AE_ROUTEEX_LIST_S *p = pMgr->stBusyHead.pNext;
    return (p == &pMgr->stBusyHead) ? HI_NULL : AE_ROUTEEX_NODE_FROM_LIST(p);
}

void AeRouteExPrint(AE_ROUTEEX_MGR_S *pMgr)
{
    AE_ROUTEEX_NODE_S *pNode;
    HI_U32 i;

    pNode = AeRouteExGetFirstNode(pMgr);
    if (pNode == HI_NULL || pMgr->u32BusyCnt == 0)
        return;

    for (i = 0; i < pMgr->u32BusyCnt; i++) {
        if (pNode == HI_NULL) return;
        printf("%8d%8u%8u%8u%8u%8u%8d%8d%12llu\n",
            i, pNode->u32Values[0], pNode->u32Values[1],
            pNode->u32Values[2], pNode->u32Values[3], pNode->u32Values[4],
            pNode->u32Stgy, pNode->u32Stgy2, pNode->u64Multi);
        pNode = AeRouteExGetUpNode(pMgr, pNode);
    }
}

void AeRouteExDelRdcy(AE_ROUTEEX_MGR_S *pMgr, HI_U32 u32Cnt)
{
    AE_ROUTEEX_LIST_S *pCurr, *pNext;

    pCurr = pMgr->stBusyHead.pNext;
    pNext = pCurr->pNext;

    while (pCurr != &pMgr->stBusyHead && pNext != &pMgr->stBusyHead) {
        AE_ROUTEEX_NODE_S *pC = AE_ROUTEEX_NODE_FROM_LIST(pCurr);
        AE_ROUTEEX_NODE_S *pN = AE_ROUTEEX_NODE_FROM_LIST(pNext);
        HI_BOOL bRedundant = HI_FALSE;
        HI_U32 i;

        if (pC->u32Stgy == 3) {
            bRedundant = HI_TRUE;
            for (i = 0; i < u32Cnt; i++) {
                if (pC->u32Values[i] != pN->u32Values[i]) {
                    bRedundant = HI_FALSE;
                    break;
                }
            }
        }

        if (!bRedundant) {
            pCurr = pNext;
            pNext = pNext->pNext;
            continue;
        }

        pC->u32Stgy = pN->u32Stgy;
        {
            AE_ROUTEEX_LIST_S *pAfter = pNext->pNext;
            pNext->pPrev->pNext = pAfter;
            pAfter->pPrev = pNext->pPrev;
        }
        AeRouteExPutFree(pMgr, pN);
        pMgr->u32BusyCnt--;

        pNext = pCurr->pNext;
    }
}

HI_S32 AeRouteExLimit(AE_ROUTEEX_MGR_S *pMgr, HI_U32 u32Cnt,
    HI_U32 *pu32Min, HI_U32 *pu32Max)
{
    AE_ROUTEEX_LIST_S *pCurr, *pNext;
    HI_U32 i;

    /* Phase 1: Clamp all node values */
    pCurr = pMgr->stBusyHead.pNext;
    while (pCurr != &pMgr->stBusyHead) {
        AE_ROUTEEX_NODE_S *pNode = AE_ROUTEEX_NODE_FROM_LIST(pCurr);
        HI_U64 u64Product = 1;

        for (i = 0; i < u32Cnt; i++) {
            pNode->u32Values[i] = AeBoundariesCheck(
                pNode->u32Values[i], pu32Min[i], pu32Max[i]);
            u64Product *= pNode->u32Values[i];
        }

        if (pNode->u64Multi != u64Product) {
            pNext = pCurr->pNext;
            AE_ROUTEEX_NODE_S *pOther = (pNext != &pMgr->stBusyHead) ?
                AE_ROUTEEX_NODE_FROM_LIST(pNext) : pNode;
            AeRouteExNodeMultiCheck(pNode, pOther, u32Cnt,
                pu32Min, u64Product, pu32Max, pNode->u64Multi);
        }

        pCurr = pCurr->pNext;
    }

    /* Phase 2: Split nodes */
    pCurr = pMgr->stBusyHead.pNext;
    pNext = pCurr->pNext;
    while (pCurr != &pMgr->stBusyHead && pNext != &pMgr->stBusyHead) {
        AeRouteExNodeRecurit(AE_ROUTEEX_NODE_FROM_LIST(pCurr),
            AE_ROUTEEX_NODE_FROM_LIST(pNext), pMgr, u32Cnt);
        pCurr = pNext;
        pNext = pNext->pNext;
    }

    /* Phase 3: Reassign strategies */
    pCurr = pMgr->stBusyHead.pNext;
    pNext = pCurr->pNext;
    while (pCurr != &pMgr->stBusyHead && pNext != &pMgr->stBusyHead) {
        AeRouteExNodeStgy(AE_ROUTEEX_NODE_FROM_LIST(pCurr),
            AE_ROUTEEX_NODE_FROM_LIST(pNext), u32Cnt);
        pCurr = pNext;
        pNext = pNext->pNext;
    }

    AeRouteExDelRdcy(pMgr, u32Cnt);
    return 0;
}
