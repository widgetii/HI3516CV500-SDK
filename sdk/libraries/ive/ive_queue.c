/**
 * Reverse Engineered by TekuConcept
 * IVE KCF Object List Management
 */

#include "re_mpi_ive.h"

/* IVE_CreateObjList: Allocate and initialize KCF object list with linked lists.
 * R0 = pstMem (IVE_MEM_INFO_S*), R1 = pstObjList (IVE_KCF_OBJ_LIST_S*), R2 = u32MaxObjNum
 *
 * The function sets up per-node memory regions from pstMem for:
 *   - HogFeature buffer: IVE_KCF_OBJ_STRIDE (0xBA00) bytes per obj
 *   - Alpha buffer: IVE_KCF_HOG_SIZE (0x2000) bytes per obj
 *   - Dst buffer: IVE_KCF_DST_SIZE (0x10) bytes per obj
 * All nodes are placed on the free list initially.
 */
HI_S32 IVE_CreateObjList(IVE_MEM_INFO_S *pstMem, IVE_KCF_OBJ_LIST_S *pstObjList, HI_U32 u32MaxObjNum)
{
    IVE_KCF_OBJ_NODE_S *pstNodeBuf;
    HI_U8 *pu8TmpBuf;
    HI_U32 u32NodeBufSize;
    HI_U32 i;
    HI_U64 u64PhyBase, u64VirBase;
    HI_U64 u64HogPhy, u64HogVir;
    HI_U64 u64AlphaPhy, u64AlphaVir;
    HI_U64 u64DstPhy, u64DstVir;
    IVE_LIST_HEAD_S *pstFreeHead;

    /* Init the three list heads to empty (point to themselves) */
    pstObjList->stFreeObjList.pstNext = &pstObjList->stFreeObjList;
    pstObjList->stFreeObjList.pstPrev = &pstObjList->stFreeObjList;
    pstObjList->stTrainObjList.pstNext = &pstObjList->stTrainObjList;
    pstObjList->stTrainObjList.pstPrev = &pstObjList->stTrainObjList;
    pstObjList->stTrackObjList.pstNext = &pstObjList->stTrackObjList;
    pstObjList->stTrackObjList.pstPrev = &pstObjList->stTrackObjList;

    /* Allocate node buffer: u32MaxObjNum * sizeof(IVE_KCF_OBJ_NODE_S) = u32MaxObjNum * 184 */
    u32NodeBufSize = u32MaxObjNum * sizeof(IVE_KCF_OBJ_NODE_S);
    pstNodeBuf = (IVE_KCF_OBJ_NODE_S *)malloc(u32NodeBufSize);
    pstObjList->pstObjNodeBuf = pstNodeBuf;
    if (pstNodeBuf == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "malloc pstObjNodeBuf failed!\n");
        return HI_ERR_IVE_NOMEM;
    }
    (void)memset_s(pstNodeBuf, u32NodeBufSize, 0, u32NodeBufSize);

    /* Allocate temp buffer */
    pu8TmpBuf = (HI_U8 *)malloc(IVE_KCF_TMP_BUF_SIZE);
    pstObjList->pu8TmpBuf = pu8TmpBuf;
    if (pu8TmpBuf == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "malloc pu8TmpBuf failed!\n");
        free(pstObjList->pstObjNodeBuf);
        pstObjList->pstObjNodeBuf = NULL;
        return HI_ERR_IVE_NOMEM;
    }
    (void)memset_s(pu8TmpBuf, IVE_KCF_TMP_BUF_SIZE, 0, IVE_KCF_TMP_BUF_SIZE);

    /* Set up per-node memory regions from pstMem physical/virtual addresses */
    u64PhyBase = pstMem->u64PhyAddr;
    u64VirBase = pstMem->u64VirAddr;

    /* HogFeature starts at base + u32MaxObjNum * IVE_KCF_OBJ_STRIDE */
    u64HogPhy = u64PhyBase + (HI_U64)u32MaxObjNum * IVE_KCF_OBJ_STRIDE;
    u64HogVir = u64VirBase + (HI_U64)u32MaxObjNum * IVE_KCF_OBJ_STRIDE;

    /* Alpha starts at HogFeature + u32MaxObjNum * IVE_KCF_HOG_SIZE */
    u64AlphaPhy = u64HogPhy + (HI_U64)u32MaxObjNum * IVE_KCF_HOG_SIZE;
    u64AlphaVir = u64HogVir + (HI_U64)u32MaxObjNum * IVE_KCF_HOG_SIZE;

    /* Dst starts at Alpha + u32MaxObjNum * IVE_KCF_DST_SIZE */
    u64DstPhy = u64AlphaPhy + (HI_U64)u32MaxObjNum * IVE_KCF_DST_SIZE;
    u64DstVir = u64AlphaVir + (HI_U64)u32MaxObjNum * IVE_KCF_DST_SIZE;

    if (u32MaxObjNum == 0)
        goto done;

    pstFreeHead = &pstObjList->stFreeObjList;

    for (i = 0; i < u32MaxObjNum; i++) {
        IVE_KCF_OBJ_NODE_S *pstNode = &pstNodeBuf[i];
        IVE_KCF_OBJ_S *pstObj = &pstNode->stKcfObj;
        IVE_LIST_HEAD_S *pstTail;

        /* HogFeature */
        pstObj->stHogFeature.u64PhyAddr = u64PhyBase + (HI_U64)i * IVE_KCF_OBJ_STRIDE;
        pstObj->stHogFeature.u64VirAddr = u64VirBase + (HI_U64)i * IVE_KCF_OBJ_STRIDE;
        pstObj->stHogFeature.u32Size = IVE_KCF_OBJ_STRIDE;

        /* Alpha */
        pstObj->stAlpha.u64PhyAddr = u64HogPhy + (HI_U64)i * IVE_KCF_HOG_SIZE;
        pstObj->stAlpha.u64VirAddr = u64HogVir + (HI_U64)i * IVE_KCF_HOG_SIZE;
        pstObj->stAlpha.u32Size = IVE_KCF_HOG_SIZE;

        /* Dst */
        pstObj->stDst.u64PhyAddr = u64AlphaPhy + (HI_U64)i * IVE_KCF_DST_SIZE;
        pstObj->stDst.u64VirAddr = u64AlphaVir + (HI_U64)i * IVE_KCF_DST_SIZE;
        pstObj->stDst.u32Size = IVE_KCF_DST_SIZE;

        /* Insert node at tail of free list */
        pstTail = pstFreeHead->pstPrev;
        pstNode->stList.pstPrev = pstTail;
        pstNode->stList.pstNext = pstFreeHead;
        pstTail->pstNext = &pstNode->stList;
        pstFreeHead->pstPrev = &pstNode->stList;
    }

done:
    pstObjList->u32TrainObjNum = 0;
    pstObjList->u32FreeObjNum = u32MaxObjNum;
    pstObjList->u32MaxObjNum = u32MaxObjNum;
    pstObjList->u32TrackObjNum = 0;
    return HI_SUCCESS;
}

HI_VOID IVE_DestroyObjList(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    HI_U8 *pu8TmpBuf;
    IVE_KCF_OBJ_NODE_S *pstNodeBuf;

    pstObjList->u32FreeObjNum = 0;

    /* Re-init all lists to empty */
    pstObjList->stFreeObjList.pstNext = &pstObjList->stFreeObjList;
    pstObjList->stFreeObjList.pstPrev = &pstObjList->stFreeObjList;
    pstObjList->stTrainObjList.pstNext = &pstObjList->stTrainObjList;
    pstObjList->stTrainObjList.pstPrev = &pstObjList->stTrainObjList;
    pstObjList->stTrackObjList.pstNext = &pstObjList->stTrackObjList;
    pstObjList->stTrackObjList.pstPrev = &pstObjList->stTrackObjList;

    pstObjList->u32MaxObjNum = 0;
    pstObjList->u32TrainObjNum = 0;
    pstObjList->u32TrackObjNum = 0;

    /* Free temp buffer */
    pu8TmpBuf = pstObjList->pu8TmpBuf;
    if (pu8TmpBuf != NULL) {
        free(pu8TmpBuf);
        pstObjList->pu8TmpBuf = NULL;
    }

    /* Free node buffer */
    pstNodeBuf = pstObjList->pstObjNodeBuf;
    if (pstNodeBuf != NULL) {
        free(pstNodeBuf);
        pstObjList->pstObjNodeBuf = NULL;
    }
}

/* Remove first node from free list, return it (or NULL if empty) */
IVE_KCF_OBJ_NODE_S *IVE_ObjListGetFree(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stFreeObjList;
    IVE_LIST_HEAD_S *pstFirst = pstHead->pstNext;
    IVE_LIST_HEAD_S *pstNext, *pstPrev;

    if (pstFirst == pstHead)
        return NULL;

    /* Unlink from free list */
    pstNext = pstFirst->pstNext;
    pstPrev = pstFirst->pstPrev;
    pstPrev->pstNext = pstNext;
    pstNext->pstPrev = pstPrev;

    /* Poison the removed node's pointers */
    pstFirst->pstNext = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON1;
    pstFirst->pstPrev = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON2;

    pstObjList->u32FreeObjNum--;

    return (IVE_KCF_OBJ_NODE_S *)pstFirst;
}

/* Insert node at tail of free list */
HI_VOID IVE_ObjListPutFree(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stFreeObjList;
    IVE_LIST_HEAD_S *pstTail = pstHead->pstPrev;

    pstObjList->stFreeObjList.pstPrev = &pstNode->stList;
    pstNode->stList.pstPrev = pstTail;
    pstNode->stList.pstNext = pstHead;
    pstTail->pstNext = &pstNode->stList;

    pstObjList->u32FreeObjNum++;
}

HI_U32 IVE_ObjListGetFreeNum(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    return pstObjList->u32FreeObjNum;
}

HI_U32 IVE_ObjListGetTrainNum(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    return pstObjList->u32TrainObjNum;
}

HI_U32 IVE_ObjListGetTrackNum(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    return pstObjList->u32TrackObjNum;
}

/* Insert node at tail of track list */
HI_VOID IVE_ObjListPutTrack(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stTrackObjList;
    IVE_LIST_HEAD_S *pstTail = pstHead->pstPrev;

    pstObjList->stTrackObjList.pstPrev = &pstNode->stList;
    pstNode->stList.pstPrev = pstTail;
    pstNode->stList.pstNext = pstHead;
    pstTail->pstNext = &pstNode->stList;

    pstObjList->u32TrackObjNum++;
}

/* Insert node at tail of train list */
HI_VOID IVE_ObjListPutTrain(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stTrainObjList;
    IVE_LIST_HEAD_S *pstTail = pstHead->pstPrev;

    pstObjList->stTrainObjList.pstPrev = &pstNode->stList;
    pstNode->stList.pstPrev = pstTail;
    pstNode->stList.pstNext = pstHead;
    pstTail->pstNext = &pstNode->stList;

    pstObjList->u32TrainObjNum++;
}

/* Remove first node from train list, return it (or NULL if empty) */
IVE_KCF_OBJ_NODE_S *IVE_ObjListGetTrain(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stTrainObjList;
    IVE_LIST_HEAD_S *pstFirst = pstHead->pstNext;
    IVE_LIST_HEAD_S *pstNext, *pstPrev;

    if (pstFirst == pstHead)
        return NULL;

    pstNext = pstFirst->pstNext;
    pstPrev = pstFirst->pstPrev;
    pstPrev->pstNext = pstNext;
    pstNext->pstPrev = pstPrev;

    pstFirst->pstNext = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON1;
    pstFirst->pstPrev = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON2;

    pstObjList->u32TrainObjNum--;

    return (IVE_KCF_OBJ_NODE_S *)pstFirst;
}

/* Query if pstNode is in the train list. Returns pstNode if found, NULL otherwise.
 * If pstNode is NULL or list is empty, returns NULL (with special return value 0 vs 1). */
IVE_KCF_OBJ_NODE_S *IVE_ObjListQueryTrain(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stTrainObjList;
    IVE_LIST_HEAD_S *pstCur = pstHead->pstNext;

    if (pstNode == NULL || pstCur == pstHead)
        return NULL;

    /* Walk the list looking for pstNode */
    while (pstCur != pstHead) {
        if (pstCur == &pstNode->stList)
            return pstNode;
        pstCur = pstCur->pstNext;
    }

    return NULL;
}

/* Remove first node from track list, return it (or NULL if empty) */
IVE_KCF_OBJ_NODE_S *IVE_ObjListGetTrack(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stTrackObjList;
    IVE_LIST_HEAD_S *pstFirst = pstHead->pstNext;
    IVE_LIST_HEAD_S *pstNext, *pstPrev;

    if (pstFirst == pstHead)
        return NULL;

    pstNext = pstFirst->pstNext;
    pstPrev = pstFirst->pstPrev;
    pstPrev->pstNext = pstNext;
    pstNext->pstPrev = pstPrev;

    pstFirst->pstNext = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON1;
    pstFirst->pstPrev = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON2;

    pstObjList->u32TrackObjNum--;

    return (IVE_KCF_OBJ_NODE_S *)pstFirst;
}

/* Query if pstNode is in the track list. Returns pstNode if found, NULL otherwise. */
IVE_KCF_OBJ_NODE_S *IVE_ObjListQueryTrack(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stTrackObjList;
    IVE_LIST_HEAD_S *pstCur = pstHead->pstNext;

    if (pstNode == NULL || pstCur == pstHead)
        return NULL;

    while (pstCur != pstHead) {
        if (pstCur == &pstNode->stList)
            return pstNode;
        pstCur = pstCur->pstNext;
    }

    return NULL;
}

/* Find and remove pstNode from the track list by traversal.
 * Returns 0 on success, -1 if not found or invalid args. */
HI_S32 IVE_ObjFreeTrackNodeByNode(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode)
{
    IVE_LIST_HEAD_S *pstHead = &pstObjList->stTrackObjList;
    IVE_LIST_HEAD_S *pstCur;
    IVE_LIST_HEAD_S *pstNext, *pstPrev;

    if (pstNode == NULL || pstHead->pstNext == pstHead)
        return -1;

    /* Walk the track list looking for pstNode */
    pstCur = pstHead->pstNext;
    while (pstCur != pstHead) {
        if (pstCur == &pstNode->stList) {
            /* Found it - unlink */
            pstNext = pstCur->pstNext;
            pstPrev = pstCur->pstPrev;
            pstPrev->pstNext = pstNext;
            pstNext->pstPrev = pstPrev;

            pstCur->pstNext = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON1;
            pstCur->pstPrev = (IVE_LIST_HEAD_S *)(uintptr_t)IVE_LIST_POISON2;

            pstObjList->u32TrackObjNum--;
            return 0;
        }
        pstCur = pstCur->pstNext;
    }

    return 0;
}
