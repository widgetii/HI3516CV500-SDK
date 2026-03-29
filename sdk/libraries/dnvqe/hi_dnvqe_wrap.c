/**
 * Reverse Engineered by TekuConcept on October 18, 2020
 */

#include "re_dnvqe_wrap.h"
#include "re_dnvqe_work.h"
#include "re_dnvqe_resampler_work.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "dnvqe_errno.h"

#define RESAMP_PROC_BUF_SZ 0x6000

HI_CHAR aHibvtAudioVers[65] = "HiBVT_AUDIO_VERSION_V3.0.7.34 Build Time:[Dec 18 2018, 21:31:50]";

HI_S32
DestoryDNVQE_Cache_Node_S(DNVQE_Cache_Node_S *pNode)
{
    if ( pNode ) {
        if ( pNode->data ) free(pNode->data);

        free(pNode);
    }

    return HI_SUCCESS;
}


HI_S32
DestoryDNVQE_Cache_S(
    HI_U32 u32ChainSize,
    DNVQE_Cache_S *pstCache)
{
    DNVQE_Cache_Node_S *pCurrNode;
    DNVQE_Cache_Node_S *pNextNode;
    HI_U32 i;

    if ( pstCache ) {
        pCurrNode = pstCache->field_4;

        for (i = 0; i < u32ChainSize; i++) {
            pNextNode = pCurrNode->next;
            DestoryDNVQE_Cache_Node_S(pCurrNode);
            pCurrNode = pNextNode;
        }

        free(pstCache->data);
        free(pstCache);
    }

    return HI_SUCCESS;
}


HI_S32
ChipIdMemMap(HI_U32 address)
{
    HI_S32 fd;
    HI_S32 *mem;
    HI_S32 result;
    HI_U32 offset;

    fd = open("/dev/mem", __O_LARGEFILE | O_TRUNC | O_RDWR);

    if ( fd < 0 ) {
        printf("Func: %s, line: %d, open fd error!\n",
            __FUNCTION__, __LINE__);
        return 0;
    }

    offset = address & 0xFFFFF000;
    mem = mmap(0, 0x1000u, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);

    if ( mem == (HI_S32 *)-1 ) {
        result = 0;
        printf("Func: %s, line: %d, mmap error!\n",
            __FUNCTION__, __LINE__);
    }
    else {
        result = mem[(address - offset) >> 2];
        munmap(mem, 0x1000u);
    }

    close(fd);
    return result;
}


DNVQE_Cache_Node_S*
CreateDNVQE_Cache_Node_S(HI_S32 u32Size)
{
    DNVQE_Cache_Node_S *stCacheNode;
    size_t size;

    if ( u32Size < 0 ) return HI_NULL;

    stCacheNode = malloc(sizeof(DNVQE_Cache_Node_S));

    if ( stCacheNode == HI_NULL ) goto error1;

    size = 2 * u32Size + 1;
    memset_s(stCacheNode, sizeof(DNVQE_Cache_Node_S),
        0, sizeof(DNVQE_Cache_Node_S));
    stCacheNode->data = malloc(size);

    if ( stCacheNode->data == HI_NULL ) goto error2;

    memset_s(stCacheNode->data, size, 0, size);
    stCacheNode->next = HI_NULL;

    return stCacheNode;

  error2:
    free(stCacheNode);
  error1:
    fprintf(stderr,
        "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
        __FUNCTION__, __LINE__, "Malloc Fail!");
    return HI_NULL;
}


DNVQE_Cache_S*
CreateDNVQE_Cache_S(
    HI_U32 u32CacheSize,
    HI_U32 u32ChainSize)
{
    DNVQE_Cache_S *pCache;
    DNVQE_Cache_Node_S *pNodeRoot;
    DNVQE_Cache_Node_S *pNextNode;
    DNVQE_Cache_Node_S *pCurrNode;
    HI_S32 i, j;
    size_t size;

    if ( u32CacheSize < 0 ) return 0;

    pCache = (DNVQE_Cache_S *)malloc(sizeof(DNVQE_Cache_S));
    if ( pCache == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "Malloc Fail!\n");
        return HI_NULL;
    }
    memset_s(pCache, sizeof(DNVQE_Cache_S), 0, sizeof(DNVQE_Cache_S));

    size = 2 * u32CacheSize + 1;
    pCache->data = (HI_CHAR *)malloc(size);
    if ( pCache->data == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "Malloc Fail!\n");
        goto error1;
    }
    memset_s(pCache->data, size, 0, size);

    pNodeRoot = CreateDNVQE_Cache_Node_S(u32CacheSize);
    if ( pNodeRoot == HI_NULL ) goto error2;

    pNextNode = HI_NULL;
    pCurrNode = pNodeRoot;

    for (i = 0; i < (HI_S32)(u32ChainSize - 1); i++) {
        pNextNode = CreateDNVQE_Cache_Node_S(u32CacheSize);

        if ( pNextNode == HI_NULL ) goto error3;

        pCurrNode->next = pNextNode;
        pCurrNode       = pNextNode;
    }

    if ( pNextNode ) pNextNode->next = pNodeRoot;

    pCache->field_4  = pNodeRoot;
    pCache->field_8  = pNodeRoot;
    pCache->field_10 = 0;
    return pCache;

  error3:
    pCurrNode = pNodeRoot;
    for (j = 0; j <= i; j++) {
        pNextNode = pCurrNode->next;
        DestoryDNVQE_Cache_Node_S(pCurrNode);
        pCurrNode = pNextNode;
    }
  error2:
    free(pCache->data);
  error1:
    free(pCache);
    return HI_NULL;
}


HI_S32
HI_DNVQE_GetVersion(HI_CHAR *aVersion)
{
    HI_S32 i;

    if ( aVersion == HI_NULL ) return ERR_DNVQE_NULL_PTR;

    for ( i = 0; ; i++ ) {
        aVersion[i] = aHibvtAudioVers[i];
        if ( aHibvtAudioVers[i] == '\0' ) break;
    }

    return HI_SUCCESS;
}


HI_VOID
HI_DNVQE_Destroy(DNVQE_CTX *pDnVqeCtx)
{
    if ( pDnVqeCtx != HI_NULL ) {
        DNVQE_Destroy(pDnVqeCtx->pWorkCtx);
        RES_ReSampler_Destory(pDnVqeCtx->pstReadCache);
        RES_ReSampler_Destory(pDnVqeCtx->pstReSampler);

        if ( pDnVqeCtx->pResampleProcBuf != HI_NULL ) {
            free(pDnVqeCtx->pResampleProcBuf);
            pDnVqeCtx->pResampleProcBuf = HI_NULL;
        }

        DestoryDNVQE_Cache_S(pDnVqeCtx->u32ChainSize, pDnVqeCtx->pSinCache);
        DestoryDNVQE_Cache_S(pDnVqeCtx->u32ChainSize, pDnVqeCtx->pSoutCache);
        pthread_mutex_destroy(&pDnVqeCtx->mutex);
        free(pDnVqeCtx);
    }
}


HI_S32
HI_DNVQE_Create(
    DNVQE_CTX **ppDnVqeCtx,
    DNVQE_ATTR *pAttr)
{
    HI_S32 result;
    DNVQE_CTX *ctx;
    int u32CacheSize = 0;

    if ( pAttr == HI_NULL ) return ERR_DNVQE_NULL_PTR;

    if (ChipIdMemMap(SYS_CTRL | VENDOR_ID) == HISI_VID &&
        ChipIdMemMap(SYS_CTRL | SC_SYSRES) == SYSRES_OK)
        return ERR_DNVQE_NULL_PTR;

    ctx = (DNVQE_CTX *)malloc(sizeof(DNVQE_CTX));
    if ( !ctx ) return ERR_DNVQE_NOMEM;

    memset_s(ctx, sizeof(DNVQE_CTX), 0, sizeof(DNVQE_CTX));

    if (RES_ReSampler_Create(&ctx->pstReadCache, pAttr->field_14, pAttr->field_18,     DNVQE_RESAMPLER_TYPE_READ_CACHE) ||
        RES_ReSampler_Create(&ctx->pstReSampler, pAttr->field_18, pAttr->enSamplerate, DNVQE_RESAMPLER_TYPE_RESAMPLER))
        goto error;

    if (ctx->pstReSampler || ctx->pstReadCache) {
        ctx->pResampleProcBuf = (HI_CHAR *)malloc(RESAMP_PROC_BUF_SZ);
        if ( ctx->pResampleProcBuf == HI_NULL ) {
            fprintf(stderr,
                "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
                __FUNCTION__, __LINE__, "Malloc DnQVE Resample ProcessBuf Fail!");
            goto error;
        }
        memset_s(ctx->pResampleProcBuf, RESAMP_PROC_BUF_SZ, 0, RESAMP_PROC_BUF_SZ);
    }

    result = DNVQE_Create(ctx, &ctx->config.u32EqEnable, &u32CacheSize, pAttr);
    if ( result ) goto error;

    pthread_mutex_init(&ctx->mutex, 0);

    ctx->u32CacheSize = u32CacheSize;
    ctx->u32ChainSize = 0x3000 / ctx->u32CacheSize;

    ctx->pSinCache = CreateDNVQE_Cache_S(u32CacheSize, ctx->u32ChainSize);
    if ( ctx->pSinCache == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "CreateDNVQE_Cache_S SinCache Fail!");
        goto error;
    }

    ctx->pSoutCache = CreateDNVQE_Cache_S(u32CacheSize, ctx->u32ChainSize);
    if ( ctx->pSoutCache == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "CreateDNVQE_Cache_S SoutCache Fail!");
        goto error;
    }

    *ppDnVqeCtx = ctx;
    return HI_SUCCESS;

  error:
    HI_DNVQE_Destroy(ctx);
    return HI_FAILURE;
}


HI_S32
HI_DNVQE_GetConfig(
    DNVQE_CTX *info,
    DNVQE_ATTR *config)
{
    if ( info == HI_NULL || config == HI_NULL )
        return ERR_DNVQE_NULL_PTR;

    memcpy(config, &info->config, sizeof(DNVQE_ATTR));
    return HI_SUCCESS;
}


HI_S32
DNVQE_CacheWrite(
    DNVQE_Cache_S *pstCache,
    HI_S32 u32NodeSize,
    HI_CHAR *pData,
    HI_S32 u32Samples)
{
    HI_S32 remaining;
    HI_S32 fillCount;

    if ( u32Samples + pstCache->field_10 < u32NodeSize ) {
        /* Fits in the current intermediate buffer */
        memcpy_s(pstCache->data + pstCache->field_10 * 2,
            u32Samples * 2, pData, u32Samples * 2);
        pstCache->field_10 += u32Samples;
        return HI_SUCCESS;
    }

    remaining = u32Samples;

    /* Flush intermediate buffer to current write node */
    if ( pstCache->field_10 > 0 ) {
        memcpy_s(pstCache->field_4->data,
            pstCache->field_10 * 2,
            pstCache->data,
            pstCache->field_10 * 2);

        fillCount = u32NodeSize - pstCache->field_10;
        memcpy_s(pstCache->field_4->data + pstCache->field_10 * 2,
            fillCount * 2, pData, fillCount * 2);

        remaining -= fillCount;
        pData += fillCount * 2;
        pstCache->field_10 = 0;
        pstCache->field_4 = pstCache->field_4->next;
        pstCache->field_0--;
    }

    /* Fill complete nodes */
    while ( remaining >= u32NodeSize ) {
        memcpy_s(pstCache->field_4->data,
            u32NodeSize * 2, pData, u32NodeSize * 2);
        pData += u32NodeSize * 2;
        remaining -= u32NodeSize;
        pstCache->field_4 = pstCache->field_4->next;
        pstCache->field_0--;
    }

    /* Store leftover in intermediate buffer */
    if ( remaining > 0 ) {
        memcpy_s(pstCache->data, remaining * 2, pData, remaining * 2);
        pstCache->field_10 = remaining;
    }

    return HI_SUCCESS;
}


HI_S32
DNVQE_CacheRead(
    DNVQE_Cache_S *pstCache,
    HI_S32 u32NodeSize,
    HI_CHAR *pOutBuf,
    HI_S32 u32Samples)
{
    HI_S32 remaining = u32Samples;
    HI_S32 partialSize;

    /* Simple case: all data in intermediate buffer */
    if ( u32Samples <= pstCache->field_10 ) {
        partialSize = u32Samples * 2;
        memcpy_s(pOutBuf, partialSize, pstCache->data, partialSize);
        pstCache->field_10 -= u32Samples;

        if ( pstCache->field_10 > 0 ) {
            memmove_s(pstCache->data, pstCache->field_10 * 2,
                pstCache->data + partialSize, pstCache->field_10 * 2);
        }

        return HI_SUCCESS;
    }

    /* Copy whatever is in the intermediate buffer first */
    if ( pstCache->field_10 > 0 ) {
        memcpy_s(pOutBuf, pstCache->field_10 * 2,
            pstCache->data, pstCache->field_10 * 2);
        pOutBuf += pstCache->field_10 * 2;
        remaining -= pstCache->field_10;
    }

    pstCache->field_10 = 0;

    /* Read complete nodes */
    while ( remaining >= u32NodeSize ) {
        memcpy_s(pOutBuf, u32NodeSize * 2,
            pstCache->field_8->data, u32NodeSize * 2);
        pOutBuf += u32NodeSize * 2;
        remaining -= u32NodeSize;
        pstCache->field_8 = pstCache->field_8->next;
        pstCache->field_0++;
    }

    /* Partial read from the next node */
    if ( remaining > 0 ) {
        HI_S32 readBytes = remaining * 2;
        HI_S32 leftover = u32NodeSize - remaining;

        memcpy_s(pOutBuf, readBytes,
            pstCache->field_8->data, readBytes);

        pstCache->field_10 = leftover;
        memcpy_s(pstCache->data, leftover * 2,
            pstCache->field_8->data + readBytes, leftover * 2);

        pstCache->field_8 = pstCache->field_8->next;
        pstCache->field_0++;
    }

    return HI_SUCCESS;
}


HI_S32
DNVQE_CacheProcessWriteInBuf(
    DNVQE_CTX *pDnVqeCtx,
    HI_S16 *ps16SinBuf,
    HI_S32 u32Samples)
{
    DNVQE_Cache_S *pSinCache = pDnVqeCtx->pSinCache;
    HI_CHAR *pBuf;
    HI_S32 available;

    if ( pDnVqeCtx->pstReadCache == HI_NULL ) {
        pBuf = (HI_CHAR *)ps16SinBuf;
    }
    else {
        pBuf = pDnVqeCtx->pResampleProcBuf;
        RES_ReSampler_ProcessFrame(
            pDnVqeCtx->pstReadCache,
            (HI_S16 *)pBuf,
            ps16SinBuf,
            u32Samples,
            &u32Samples,
            DNVQE_RESAMPLER_TYPE_READ_CACHE);
    }

    available = pSinCache->field_0 * pDnVqeCtx->u32CacheSize
              + pDnVqeCtx->u32CacheSize
              - pSinCache->field_10;

    if ( available < u32Samples ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "HI_ERR_DNVQE_WRITECACHE_FULL");
        return ERR_DNVQE_CACHE_FULL;
    }

    DNVQE_CacheWrite(pSinCache, pDnVqeCtx->u32CacheSize, pBuf, u32Samples);
    return HI_SUCCESS;
}


HI_S32
HI_DNVQE_ReadFrame(
    DNVQE_CTX *pDnVqeCtx,
    HI_S16 *ps16SouBuf,
    HI_S32 u32Samples,
    HI_S32 bBlock)
{
    DNVQE_Cache_S *pSoutCache;
    HI_S32 available;
    HI_S32 readCount;
    HI_CHAR *pBuf;
    HI_S32 result;

    if ( pDnVqeCtx == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "DNVQE invalid hDnVqe");
        return ERR_DNVQE_NULL_PTR;
    }

    if ( ps16SouBuf == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "DNVQE invalid ps16SouBuf");
        return ERR_DNVQE_NULL_PTR;
    }

    pSoutCache = pDnVqeCtx->pSoutCache;
    readCount = u32Samples;

    pthread_mutex_lock(&pDnVqeCtx->mutex);

    if ( pDnVqeCtx->pstReSampler == HI_NULL ) {
        pBuf = (HI_CHAR *)ps16SouBuf;
    }
    else {
        pBuf = pDnVqeCtx->pResampleProcBuf;
        readCount = RES_ReSampler_GetInputNum(
            pDnVqeCtx->pstReSampler, u32Samples,
            DNVQE_RESAMPLER_TYPE_RESAMPLER);
        if ( readCount >= 0x3000 )
            readCount = 0x3000;
    }

    available = (pDnVqeCtx->u32ChainSize - pSoutCache->field_0)
              * pDnVqeCtx->u32CacheSize
              + pSoutCache->field_10;

    if ( readCount > available ) {
        if ( bBlock == 0 ) {
            result = ERR_DNVQE_CACHE_EMPTY;
            goto unlock;
        }

        DNVQE_CacheRead(pSoutCache, pDnVqeCtx->u32CacheSize,
            pBuf, available);

        if ( pDnVqeCtx->pstReSampler != HI_NULL ) {
            result = RES_ReSampler_ProcessFrame(
                pDnVqeCtx->pstReSampler,
                ps16SouBuf,
                (HI_S16 *)pBuf,
                available,
                &u32Samples,
                DNVQE_RESAMPLER_TYPE_BUTT);
            if ( result != 0 )
                result = HI_FAILURE;
            else
                result = u32Samples;
        }
        else {
            result = ERR_DNVQE_CACHE_EMPTY;
        }

        goto unlock;
    }

    DNVQE_CacheRead(pSoutCache, pDnVqeCtx->u32CacheSize,
        pBuf, readCount);

    if ( pDnVqeCtx->pstReSampler != HI_NULL ) {
        result = RES_ReSampler_ProcessFrame(
            pDnVqeCtx->pstReSampler,
            ps16SouBuf,
            (HI_S16 *)pBuf,
            readCount,
            &u32Samples,
            DNVQE_RESAMPLER_TYPE_RESAMPLER);
    }
    else {
        result = 0;
    }

    if ( bBlock == 0 )
        goto unlock;

    if ( result != 0 )
        result = HI_FAILURE;
    else
        result = u32Samples;

  unlock:
    pthread_mutex_unlock(&pDnVqeCtx->mutex);
    return result;
}


HI_S32
DNVQE_CacheProcessFrame(DNVQE_CTX *pDnVqeCtx)
{
    DNVQE_Cache_S *pSinCache  = pDnVqeCtx->pSinCache;
    DNVQE_Cache_S *pSoutCache = pDnVqeCtx->pSoutCache;
    DNVQE_Cache_Node_S *pReadNode  = pSinCache->field_8;
    DNVQE_Cache_Node_S *pWriteNode = pSoutCache->field_4;
    HI_S32 result;

    while ( pSoutCache->field_0 > 0 ) {
        if ( pReadNode == pSinCache->field_4 )
            break;

        result = HI_DNVQE_ProcessFrame(
            pDnVqeCtx,
            (HI_S16 *)pReadNode->data,
            (HI_S16 *)pWriteNode->data);

        if ( result != HI_SUCCESS ) {
            pSinCache->field_8  = pReadNode;
            pSoutCache->field_4 = pWriteNode;
            return result;
        }

        pReadNode  = pReadNode->next;
        pWriteNode = pWriteNode->next;
        pSinCache->field_0++;
        pSoutCache->field_0--;
    }

    pSinCache->field_8  = pReadNode;
    pSoutCache->field_4 = pWriteNode;

    if ( pSoutCache->field_0 != 0 )
        return HI_SUCCESS;
    else
        return ERR_DNVQE_CACHE_BUSY;
}


HI_S32
HI_DNVQE_ProcessFrame(
    DNVQE_CTX *pDnVqeCtx,
    HI_S16 *ps16SinBuf,
    HI_S16 *ps16SouBuf)
{
    if ( pDnVqeCtx == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "DNVQE invalid hDnVqe");
        return ERR_DNVQE_NULL_PTR;
    }

    if ( ps16SinBuf == HI_NULL || ps16SouBuf == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "DNVQE invalid ps16SinBuf/ps16SouBuf\n");
        return ERR_DNVQE_NULL_PTR;
    }

    return DNVQE_ProcessFrame(pDnVqeCtx->pWorkCtx, ps16SinBuf, ps16SouBuf);
}


HI_S32
HI_DNVQE_WriteFrame(
    DNVQE_CTX *pDnVqeCtx,
    HI_S16 *ps16SinBuf,
    HI_S16 *ps16SouBuf)
{
    if ( pDnVqeCtx == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "DNVQE invalid hDnVqe");
        return ERR_DNVQE_NULL_PTR;
    }

    if ( ps16SinBuf == HI_NULL ) {
        fprintf(stderr,
            "\n\n\x1B[40m\x1B[31m\x1B[1m**Err In %s-%d:  %s**\x1B[0m\n\n",
            __FUNCTION__, __LINE__, "DNVQE invalid ps16SinBuf");
        return ERR_DNVQE_NULL_PTR;
    }

    pthread_mutex_lock(&pDnVqeCtx->mutex);

    DNVQE_CacheProcessFrame(pDnVqeCtx);
    if ( !DNVQE_CacheProcessWriteInBuf(pDnVqeCtx, ps16SinBuf, ps16SouBuf) )
        DNVQE_CacheProcessFrame(pDnVqeCtx);

    pthread_mutex_unlock(&pDnVqeCtx->mutex);
    return HI_SUCCESS;
}
