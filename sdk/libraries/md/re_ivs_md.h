/**
 * Reverse Engineered by TekuConcept
 * MD (Motion Detection) Internal Header
 */

#ifndef _RE_IVS_MD_H_
#define _RE_IVS_MD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_common.h"
#include "hi_comm_ive.h"
#include "hi_ive.h"
#include "hi_md.h"
#include "ivs_md.h"
#include "mpi_ive.h"
#include "mpi_sys.h"
#include "securec.h"

/* Trace macros — MD uses module ID 48 (0x30) */
#define HI_TRACE_MD(level, fmt, ...) \
    do { \
        HI_TRACE(level, 48, "[Func]:%s [Line]:%d [Info]:" fmt, \
            __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define HI_TRACE_IVE(level, fmt, ...) \
    do { \
        HI_TRACE(level, HI_ID_IVE, "[Func]:%s [Line]:%d [Info]:" fmt, \
            __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

/* MD error codes using module 48 */
#define HI_ERR_MD_INVALID_CHNID  HI_DEF_ERR(48, EN_ERR_LEVEL_ERROR, EN_ERR_INVALID_CHNID)
#define HI_ERR_MD_EXIST          HI_DEF_ERR(48, EN_ERR_LEVEL_ERROR, EN_ERR_EXIST)
#define HI_ERR_MD_UNEXIST        HI_DEF_ERR(48, EN_ERR_LEVEL_ERROR, EN_ERR_UNEXIST)
#define HI_ERR_MD_NULL_PTR       HI_DEF_ERR(48, EN_ERR_LEVEL_ERROR, EN_ERR_NULL_PTR)
#define HI_ERR_MD_NOT_PERM       HI_DEF_ERR(48, EN_ERR_LEVEL_ERROR, EN_ERR_NOT_PERM)
#define HI_ERR_MD_BUSY           HI_DEF_ERR(48, EN_ERR_LEVEL_ERROR, EN_ERR_BUSY)

/*
 * Per-channel context structure (288 = 0x120 bytes)
 * Layout derived from assembly field access offsets in ivs_md.S
 */
typedef struct hiMD_CHN_CTX_S {
    /* 0x00 */ HI_U32 bCreated;
    /* 0x04 */ MD_ALG_MODE_E enAlgMode;
    /* 0x08 */ IVE_SAD_MODE_E enSadMode;
    /* 0x0C */ IVE_SAD_OUT_CTRL_E enSadOutCtrl;
    /* 0x10 */ HI_U32 u32Width;
    /* 0x14 */ HI_U32 u32Height;
    /* 0x18 */ IVE_CCL_CTRL_S stCclCtrl;           /* 8 bytes */
    /* 0x20 */ IVE_ADD_CTRL_S stAddCtrl;            /* 4 bytes */
    /* 0x24 */ HI_U16 u16SadThr;
    /* 0x26 */ HI_U16 u16Reserved1;
    /* 0x28 */ HI_U32 u32ProcCnt;
    /* 0x2C */ HI_U32 u32TotalTime;
    /* 0x30 */ HI_U32 u32MaxTime;
    /* 0x34 */ HI_U32 u32MinTime;
    /* 0x38 */ HI_U64 u64Reserved2;
    /* 0x40 */ HI_U8  bFirstFrame;
    /* 0x41 */ HI_U8  bDualOutput;
    /* 0x42 */ HI_U8  au8Pad1[6];
    /* 0x48 */ HI_U64 u64SadPhyAddr;               /* SAD output buffer phys */
    /* 0x50 */ HI_U8  au8Pad2[16];
    /* 0x60 */ HI_U64 u64SadVirAddr;                /* SAD output buffer virt (as u64) */
    /* 0x68 */ HI_U8  au8Pad3[16];
    /* 0x78 */ HI_U32 u32BgStride;                  /* aligned BG stride */
    /* 0x7C */ HI_U8  au8Pad4[8];
    /* 0x84 */ HI_U32 u32BgSadWidth;                /* sadWidth before alignment */
    /* 0x88 */ HI_U32 u32BgHeight;                  /* height for BG alloc */
    /* 0x8C */ HI_U32 u32BgReserved;
    /* 0x90 */ HI_U64 u64BgPhyAddr2;                /* BG region 2 phys */
    /* 0x98 */ HI_U8  au8Pad5[16];
    /* 0xA8 */ HI_U64 u64BgVirAddr2;
    /* 0xB0 */ HI_U8  au8Pad6[8];
    /* 0xB8 */ HI_U8  au8Pad7[8];
    /* 0xC0 */ HI_U32 u32BgRefStride;
    /* 0xC4 */ HI_U8  au8Pad8[8];
    /* 0xCC */ HI_U32 u32BgRefWidth;
    /* 0xD0 */ HI_U32 u32BgRefHeight;
    /* 0xD4 */ HI_U32 u32BgAllocated;
    /* 0xD8 */ IVE_IMAGE_S stBgImage;               /* 72 bytes, internal background image */
    /* 0x108 */ HI_U32 u32SadStride;                /* SAD output stride (16-aligned) */
    /* 0x10C */ HI_U8  au8Pad9[8];
    /* 0x114 */ HI_U32 u32SadWidth;                 /* SAD output width */
    /* 0x118 */ HI_U32 u32SadHeight;                /* SAD output height */
    /* 0x11C */ HI_U32 u32Status;                   /* channel status */
} MD_CHN_CTX_S;

/* Internal functions from inner_comm_user.c */
extern HI_S32 IveMalloc(HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr,
    const HI_CHAR *pchName, HI_U32 u32Size);
extern HI_VOID IveFree(HI_U64 u64PhyAddr, HI_VOID *pVirAddr);
extern HI_S32 MdCheckImageUser(IVE_IMAGE_S *pstImage, HI_U32 u32MinWidth,
    HI_U32 u32MaxWidth, HI_U32 u32MinHeight, HI_U32 u32MaxHeight,
    HI_U32 u32Align, HI_U8 u8PlaneCheck);

/* Internal functions from check_param.c */
extern HI_S32 MD_CheckAttr(MD_ATTR_S *pstMdAttr);

/* IVE proc functions from libive (mpi_ive.c) */
extern HI_S32 MPI_IVE_MdProcInit(HI_U64 *pu64PhyAddr, HI_U32 *pu32Size);
extern HI_VOID MPI_IVE_MdProcExit(HI_VOID);
extern HI_S32 MPI_IVE_MdProcBeginWrite(HI_VOID);
extern HI_S32 MPI_IVE_MdProcEndWrite(HI_VOID);

#endif /* _RE_IVS_MD_H_ */
