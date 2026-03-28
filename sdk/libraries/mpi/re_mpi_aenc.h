/**
 * Reverse Engineered by TekuConcept on April 26, 2021
 */

#ifndef RE_MPI_AENC_H
#define RE_MPI_AENC_H

#include "mpi_audio.h"
#include "hi_comm_aenc.h"
#include "hi_debug.h"
#include <sys/ioctl.h>

#define AENC_MAX_CHN_NUM  32
#define AENC_MAX_ENCODER_NUM 20

#define HI_TRACE_AENC(level, fmt, ...)                                                                         \
    do {                                                                                                       \
        HI_TRACE(level, HI_ID_AENC, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

typedef struct hiAENC_ENCODER_CTX_S {
    PAYLOAD_TYPE_E  enType;                                                  /* 0x00 (offset +28 in registry) */
    HI_U32          u32MaxFrmLen;                                            /* 0x04 */
    HI_S32          (*pfnOpenEncoder)(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder); /* 0x08 */
    HI_S32          (*pfnEncodeFrm)(HI_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
                        HI_U8 *pu8Outbuf, HI_U32 *pu32OutLen);              /* 0x0C */
    HI_S32          (*pfnCloseEncoder)(HI_VOID *pEncoder);                   /* 0x10 */
} AENC_ENCODER_CTX_S;

/* Encoder registry: count(4) + mutex(24) + 20 entries × 40 bytes */
/* Each entry: 12 bytes padding + 28 bytes of AENC_ENCODER_S data starting at offset +28 */
typedef struct hiAENC_ENCODER_REGISTRY_S { /* sizeof=0x33C (828) */
    HI_U32 u32Count;                      /* 0x00 */
    pthread_mutex_t mutex;                 /* 0x04 */
    struct {
        HI_S32 s32Handle;                 /* offset +28 from entry start: payload type or -1 if empty */
        HI_U32 u32MaxFrmLen;              /* +32 */
        HI_CHAR aszName[17];              /* +36 */
        HI_U8  _pad[3];
        HI_S32 (*pfnOpenEncoder)(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder);  /* +56 */
        HI_S32 (*pfnEncodeFrm)(HI_VOID *pEncoder, const AUDIO_FRAME_S *pstData,
                    HI_U8 *pu8Outbuf, HI_U32 *pu32OutLen);                     /* +60 (0x3C) */
        HI_S32 (*pfnCloseEncoder)(HI_VOID *pEncoder);                          /* +64 (0x40) */
    } entries[AENC_MAX_ENCODER_NUM];       /* 20 × 40 = 800 bytes */
} AENC_ENCODER_REGISTRY_S;

/* Stream buffer header prepended to each encoded packet in the circular buffer */
typedef struct hiAENC_STREAM_HEADER_S {
    HI_U32 u32Len;        /* encoded data length */
    HI_U64 u64TimeStamp;  /* timestamp */
    HI_U32 u32Seq;        /* sequence number */
} AENC_STREAM_HEADER_S;

/* Circular buffer state for one stream direction */
typedef struct hiAENC_CIRBUF_S { /* sizeof=0x20 (32 bytes) */
    HI_U64 u64PhyAddr;    /* 0x00 */
    HI_U32 *pu8VirtAddr;  /* 0x08 (mapped virtual address) */
    HI_U32 u32PackLen;    /* 0x0C (single packet max length, aligned to 64) */
    HI_U32 u32Size;       /* 0x10 (total buffer size) */
    HI_U32 u32TotalLen;   /* 0x14 (total circular buffer capacity) */
    HI_U32 u32Read;       /* 0x18 (read offset) */
    HI_U32 u32Write;      /* 0x1C (write offset) */
} AENC_CIRBUF_S;

/* Per-channel context: 240 bytes (0xF0) */
typedef struct hiAENC_CHN_CTX_S { /* sizeof=0xF0 (240) */
    HI_U32 u32StreamReady;       /* 0x00: stream buffer initialized flag */
    HI_U32 field_04;             /* 0x04 */
    HI_U64 u64StrmPhyAddr;      /* 0x08: stream buffer physical address */
    HI_U32 *pu8StrmVirtAddr;    /* 0x10: stream buffer virtual address */
    HI_U32 u32StrmBufLen;       /* 0x14: total stream buffer length */
    HI_U32 u32StrmLen;          /* 0x18: stream buffer actual used length */
    HI_U32 u32StrmPackLen;      /* 0x1C: packet length (u32MaxFrmLen aligned to 64) */
    HI_S32 s32EncCount;         /* 0x20: encode count */
    HI_S32 s32DecCount;         /* 0x24: decode/release count */
    HI_U32 field_28;            /* 0x28 */
    HI_S32 s32FrameCount;       /* 0x2C: frame count, init to -1 */
    HI_S32 s32EncoderIdx;       /* 0x30: index into encoder registry, -1 if unbound */
    HI_U32 u32DbgField34;      /* 0x34: from ioctl CreateChn response */
    HI_U32 field_38;            /* 0x38 */
    HI_U32 field_3C;            /* 0x3C */
    HI_U32 u32Created;          /* 0x40: 1 if channel created */
    HI_VOID *pEncoder;          /* 0x44: encoder handle from pfnOpenEncoder */
    HI_U32 field_48;            /* 0x48 */
    pthread_mutex_t mutex;      /* 0x4C: per-channel mutex (24 bytes) */
    HI_U32 field_64;            /* 0x64 */
    PAYLOAD_TYPE_E enType1;     /* 0x68: codec type copy 1, init to 8 */
    HI_U32 field_6C;            /* 0x6C */
    HI_U32 field_70;            /* 0x70 */
    HI_U32 u32StrmBufReady;    /* 0x74: stream read buffer initialized */
    HI_U32 u32EncErrCnt;       /* 0x78: encode error count */
    HI_U32 field_7C;            /* 0x7C */
    HI_U32 u32CheckFrameErr;   /* 0x80: check frame error count */
    HI_U32 field_84;            /* 0x84 */
    HI_U32 field_88;            /* 0x88 */
    HI_U32 field_8C;            /* 0x8C */
    PAYLOAD_TYPE_E enType2;     /* 0x90: codec type copy 2, init to 8 */
    PAYLOAD_TYPE_E enType3;     /* 0x94: codec type copy 3, init to 3 */
    HI_S32 s32Handle1;         /* 0x98: handle/index, init to -1 */
    HI_S32 s32Handle2;         /* 0x9C: handle/index, init to -1 */
    HI_S32 s32Handle3;         /* 0xA0: handle/index, init to -1 */
    HI_U32 u32PtNumPerFrm;    /* 0xA4: points per frame from channel attr */
    AENC_CIRBUF_S stWriteBuf;  /* 0xA8: write circular buffer (32 bytes) */
    AENC_CIRBUF_S stReadBuf;   /* 0xC8: read circular buffer (32 bytes) */
    HI_U8 *pu8EncBuf[2];      /* 0xE8, 0xEC: encode working buffers (malloc'd 16KB each) */
} AENC_CHN_CTX_S;

/* Debug info structure passed to kernel via ioctl */
typedef struct hiAENC_DBG_INFO_S { /* sizeof=0x20 (32) */
    HI_U8 data[32];
} AENC_DBG_INFO_S;

/* CreateChn ioctl structure */
typedef struct hiAENC_CREATE_INFO_S { /* sizeof=0x14 (20) */
    PAYLOAD_TYPE_E enType;      /* 0x00 */
    HI_U32 u32BufSize;         /* 0x04 */
    HI_U32 u32PtNumPerFrm;    /* 0x08 */
    HI_VOID *pValue;           /* 0x0C */
    HI_U32 field_10;           /* 0x10: output from kernel */
} AENC_CREATE_INFO_S;

/* ioctl type 'A' = 0x41 */
#define IOC_TYPE_AENC 'A' /* 0x41 */
#define IOC_AENC_SET_ATTR         _IOR( IOC_TYPE_AENC, 0x00, AENC_CHN_ATTR_S       ) /* 0x40104100u */
#define IOC_AENC_DESTROY_CHN      _IO(  IOC_TYPE_AENC, 0x01                         ) /* 0x00004101u */
#define IOC_AENC_GET_FRAME        _IOW( IOC_TYPE_AENC, 0x02, HI_U8[128]             ) /* 0x80804102u */
#define IOC_AENC_RELEASE_FRAME    _IOR( IOC_TYPE_AENC, 0x03, HI_U8[128]             ) /* 0x40804103u */
#define IOC_AENC_SET_DBG_INFO     _IOR( IOC_TYPE_AENC, 0x04, AENC_DBG_INFO_S        ) /* 0x40204104u */
#define IOC_AENC_GET_STREAM       _IOWR(IOC_TYPE_AENC, 0x05, HI_U8[48]              ) /* 0xC0304105u */
#define IOC_AENC_RELEASE_STREAM   _IOR( IOC_TYPE_AENC, 0x06, HI_U8[40]              ) /* 0x40284106u */
#define IOC_AENC_SET_STRM_BUF     _IOR( IOC_TYPE_AENC, 0x07, HI_U8[24]              ) /* 0x40184107u */
#define IOC_AENC_CLR_STRM_BUF     _IO(  IOC_TYPE_AENC, 0x08                         ) /* 0x00004108u */
#define IOC_AENC_INIT_CHN         _IOR( IOC_TYPE_AENC, 0x09, AENC_CHN               ) /* 0x40044109u */
#define IOC_AENC_SEND_FRAME       _IOR( IOC_TYPE_AENC, 0x0A, HI_U8[56]              ) /* 0x4038410Au */
#define IOC_AENC_CREATE_CHN       _IOWR(IOC_TYPE_AENC, 0x0B, AENC_CREATE_INFO_S     ) /* 0xC014410Bu */
#define IOC_AENC_SET_MUTE         _IOR( IOC_TYPE_AENC, 0x0C, HI_BOOL                ) /* 0x4004410Cu */
#define IOC_AENC_GET_MUTE         _IOW( IOC_TYPE_AENC, 0x0D, HI_BOOL                ) /* 0x8004410Du */

#endif
