/**
 * Reverse Engineered by TekuConcept on April 26, 2021
 */

#ifndef RE_MPI_ADEC_H
#define RE_MPI_ADEC_H

#include "mpi_audio.h"
#include "hi_comm_adec.h"
#include "hi_debug.h"
#include <semaphore.h>
#include <sys/ioctl.h>

#define ADEC_MAX_CHN_NUM  32
#define ADEC_MAX_DECODER_NUM 20

#define HI_TRACE_ADEC(level, fmt, ...)                                                                         \
    do {                                                                                                       \
        HI_TRACE(level, HI_ID_ADEC, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

/* Decoder registry: count(4) + mutex(24) + 20 entries × 44 bytes */
/* Each entry: offset +28 contains the ADEC_DECODER_S data */
typedef struct hiADEC_DECODER_REGISTRY_S { /* sizeof=0x38C (908) */
    HI_U32 u32Count;                      /* 0x00 */
    pthread_mutex_t mutex;                 /* 0x04 */
    struct {
        HI_S32 s32Handle;                 /* offset +28: payload type or -1 if empty */
        HI_CHAR aszName[17];              /* +32 */
        HI_U8  _pad[3];
        HI_S32 (*pfnOpenDecoder)(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder);  /* +52 */
        HI_S32 (*pfnDecodeFrm)(HI_VOID *pDecoder, HI_U8 **pu8Inbuf,
                    HI_S32 *ps32LeftByte, HI_U16 *pu16Outbuf,
                    HI_U32 *pu32OutLen, HI_U32 *pu32Chns);                     /* +56 (0x38) */
        HI_S32 (*pfnGetFrmInfo)(HI_VOID *pDecoder, HI_VOID *pInfo);            /* +60 (0x3C) */
        HI_S32 (*pfnCloseDecoder)(HI_VOID *pDecoder);                          /* +64 (0x40) */
        HI_S32 (*pfnResetDecoder)(HI_VOID *pDecoder);                          /* +68 (0x44) */
    } entries[ADEC_MAX_DECODER_NUM];       /* 20 × 44 = 880 bytes */
} ADEC_DECODER_REGISTRY_S;

/* AF (Audio Frame) buffer entry - 72 bytes (0x48) per entry */
/* Stored as doubly-linked list nodes within the AF buffer */
typedef struct hiADEC_AF_NODE_S {
    HI_U8  au8Frame[8];              /* 0x00: frame data area (start of AUDIO_FRAME_S-like) */
    HI_U8  *pu8FrameData;            /* 0x08: pointer into frame data buffer */
    HI_U8  *pu8FrameData2;           /* 0x0C: second channel frame data pointer */
    HI_U8  au8FrameMeta[44];         /* 0x10-0x3B: remaining frame metadata */
    struct hiADEC_AF_NODE_S *pNext;   /* 0x3C: forward link */
    struct hiADEC_AF_NODE_S *pPrev;   /* 0x40: backward link */
    HI_U32 field_44;                  /* 0x44 */
} ADEC_AF_NODE_S; /* 72 bytes = 0x48 */

/* AF buffer metadata - located at afBuf + 0x5460 */
typedef struct hiADEC_AF_META_S {
    HI_U8  *pu8FrameDataBuf;   /* +0x460 from R7: frame data buffer */
    HI_U32 u32TotalCount;      /* +0x464: total frame count */
    HI_U32 u32FreeCount;       /* +0x468: free list count */
    HI_U32 u32BusyCount;       /* +0x46C: busy list count */
    ADEC_AF_NODE_S stFreeHead; /* +0x470: free list sentinel (only first 8 bytes used) */
    ADEC_AF_NODE_S stBusyHead; /* +0x478: busy list sentinel */
} ADEC_AF_META_S;

/* TST (TimeStamp Table) buffer entry - 32 bytes per entry */
typedef struct hiADEC_TST_NODE_S {
    HI_U64 u64TimeStamp;              /* 0x00: timestamp */
    HI_U64 u64Offset;                 /* 0x08: offset in stream buffer (low=offset, high=0) */
    HI_U32 u32Len;                    /* 0x10: data length */
    struct hiADEC_TST_NODE_S *pNext;  /* 0x14: forward link */
    struct hiADEC_TST_NODE_S *pPrev;  /* 0x18: backward link */
    HI_U32 field_1C;                  /* 0x1C */
} ADEC_TST_NODE_S; /* 32 bytes = 0x20 */

/* TST buffer metadata - located at tstBuf + 0x2580 */
typedef struct hiADEC_TST_META_S {
    HI_U32 u32TotalCount;     /* +0x580 from R1: total TST count */
    HI_U32 u32FreeCount;      /* +0x584: free list count */
    HI_U32 u32BusyCount;      /* +0x588: busy list count */
    ADEC_TST_NODE_S stFreeHead;/* +0x58C: free list sentinel */
    ADEC_TST_NODE_S stBusyHead;/* +0x594: busy list sentinel */
} ADEC_TST_META_S;

/* Per-channel context: 232 bytes (0xE8) */
typedef struct hiADEC_CHN_CTX_S { /* sizeof=0xE8 (232) */
    HI_VOID *pAfBuf;              /* 0x00: AF buffer (malloc'd 21632 bytes) */
    HI_U32 bCreated;              /* 0x04: 1 if channel created */
    HI_U32 bDestroying;           /* 0x08: 1 if being destroyed */
    HI_S32 s32AdChn;              /* 0x0C: channel number, init -1 */
    HI_VOID *pDecoder;            /* 0x10: decoder handle from pfnOpenDecoder */
    ADEC_CHN_ATTR_S stAttr;       /* 0x14: channel attributes copy (16 bytes) */
    HI_U32 u32DecMode;            /* 0x24: decode mode (0=pack, 1=stream) */
    sem_t semRead;                /* 0x28: read semaphore (16 bytes) */
    sem_t semWrite;               /* 0x38: write semaphore (16 bytes) */
    HI_U32 u32RefCount;           /* 0x48: reference count */
    pthread_mutex_t mutex;        /* 0x4C: per-channel mutex (24 bytes) */
    HI_U32 u32DecodedCount;       /* 0x64: total decoded frame count */
    /* Debug info block - 28 bytes starting at 0x68, sent to kernel via ioctl */
    HI_U32 u32DbgEncCount;        /* 0x68: encode/operation count */
    HI_U32 u32DbgDecSuccCount;    /* 0x6C: decode success count */
    HI_U32 u32DbgGetFrmCount;     /* 0x70: get frame count */
    HI_U32 u32DbgRelFrmCount;     /* 0x74: release frame count */
    HI_U32 u32G726Bps;            /* 0x78: G726 BPS from pValue, or 8 */
    HI_U32 u32AdpcmType;          /* 0x7C: ADPCM type from pValue, or 3 */
    HI_U32 u32DecReadyFlag;       /* 0x80: decode ready flag */
    /* End of debug info block */
    HI_S32 s32DecoderIdx;         /* 0x84: index into decoder registry, -1 if unbound */
    HI_U64 u64TimeStamp;          /* 0x88: current timestamp */
    HI_U32 u32StrmUsedLen;        /* 0x90: stream buffer used length */
    HI_U32 u32StrmReadPos;        /* 0x94: stream buffer read position */
    HI_U8 *pu8StrmBuf;            /* 0x98: stream buffer (malloc'd 16384 for stream mode) */
    HI_U8 *pu8TstBuf;             /* 0x9C: TST buffer (malloc'd 9632 for stream mode) */
    HI_U32 u32SendAoRunning;      /* 0xA0: SendAo thread running flag */
    HI_U32 u32RetryFlag;          /* 0xA4: retry flag for SendAo */
    HI_S32 s32DecThread;          /* 0xA8: DecProc thread handle */
    HI_S32 s32SendAoThread;       /* 0xAC: SendAoProc thread handle */
    HI_U32 field_B0;              /* 0xB0 */
    HI_U32 field_B4;              /* 0xB4 */
    HI_S32 field_B8;              /* 0xB8: init -1 */
    HI_S32 field_BC;              /* 0xBC: init -1 */
    HI_U64 field_C0;              /* 0xC0: init 0 */
    HI_U32 field_C8;              /* 0xC8 */
    HI_S32 field_CC;              /* 0xCC: init -1 */
    HI_U32 field_D0;              /* 0xD0 */
    HI_U32 field_D4;              /* 0xD4 */
    HI_U32 field_D8;              /* 0xD8 */
    HI_U32 field_DC;              /* 0xDC */
    HI_U32 field_E0;              /* 0xE0 */
    HI_U32 u32EndOfStream;        /* 0xE4: end-of-stream flag */
} ADEC_CHN_CTX_S;

/* Thread argument structure */
typedef struct hiADEC_THREAD_ARG_S {
    HI_U32 field_0;
    HI_U32 field_4;
    HI_U32 field_8;
    HI_S32 s32AdChn;     /* 0x0C: channel number */
} ADEC_THREAD_ARG_S;

/* ioctl type 'H' = 0x48 */
#define IOC_TYPE_ADEC 'H' /* 0x48 */
#define IOC_ADEC_INIT_CHN     _IOR( IOC_TYPE_ADEC, 0x00, ADEC_CHN         ) /* 0x40044800u */
#define IOC_ADEC_SET_ATTR     _IOR( IOC_TYPE_ADEC, 0x01, ADEC_CHN_ATTR_S  ) /* 0x40104801u */
#define IOC_ADEC_DESTROY_CHN  _IO(  IOC_TYPE_ADEC, 0x02                    ) /* 0x00004802u */
#define IOC_ADEC_SET_DBG_INFO _IOR( IOC_TYPE_ADEC, 0x03, HI_U8[28]        ) /* 0x401C4803u */

/* AF buffer constants */
#define ADEC_AF_BUF_SIZE     21632  /* 0x5480 */
#define ADEC_AF_META_OFFSET  0x5000
#define ADEC_AF_NODE_SIZE    72     /* 0x48 */
#define ADEC_MAX_BUF_SIZE    300
#define ADEC_FRAME_DATA_SIZE 0x8000 /* 32768 bytes per frame */

/* TST buffer constants */
#define ADEC_TST_BUF_SIZE    9632   /* 0x25A0 */
#define ADEC_TST_META_OFFSET 0x2000
#define ADEC_TST_NODE_SIZE   32     /* 0x20 */
#define ADEC_TST_MAX_COUNT   300

/* Stream buffer size */
#define ADEC_STRM_BUF_SIZE   16384  /* 0x4000 */

#endif
