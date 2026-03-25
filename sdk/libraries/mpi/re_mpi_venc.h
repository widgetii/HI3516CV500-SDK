/**
 * Reverse Engineered from mpi_venc.S vendor assembly
 */

#ifndef RE_MPI_VENC_H
#define RE_MPI_VENC_H

#include "mpi_venc.h"
#include "re_debug.h"

#include <pthread.h>
#include <sys/ioctl.h>

#define VENC_DEV_NAME "/dev/venc"

/* Channel context structure — 80 (0x50) bytes per channel, 16 channels max */
typedef struct hiVENC_CHN_CTX_S {
    HI_S32          s32Fd;          /* 0x00 */
    pthread_mutex_t mutex;          /* 0x04 (24 bytes on ARM) */
    HI_U32          reserved_1C;    /* 0x1C */
    HI_U64          u64PhyAddr;     /* 0x20 — stream buffer physical base */
    HI_U64          u64VirtAddr;    /* 0x28 — stream buffer kernel virtual base */
    HI_VOID*        pUserAddr;      /* 0x30 — mmap'd userspace pointer */
    HI_U32          reserved_34;    /* 0x34 */
    HI_U64          u64BufLen;      /* 0x38 — stream buffer length */
    HI_U32          reserved_40;    /* 0x40 */
    HI_U32          reserved_44;    /* 0x44 */
    HI_U32          u32CodingType;  /* 0x48 — PAYLOAD_TYPE_E */
    HI_U32          u32PackCnt;     /* 0x4C — max pack count */
} VENC_CHN_CTX_S;

/* Kernel-internal stream buffer info (size = 0x20 = 32 bytes) */
typedef struct hiVENC_STREAM_BUF_INFO_KERN_S {
    HI_U64  u64PhyAddr;     /* 0x00 */
    HI_U64  u64VirtAddr;    /* 0x08 — kernel virtual */
    HI_U32  u32BufLen;      /* 0x10 */
    HI_U32  u32Field14;     /* 0x14 */
    HI_U32  u32Field18;     /* 0x18 */
    HI_U32  u32Field1C;     /* 0x1C */
} VENC_STREAM_BUF_INFO_KERN_S;

/* Ioctl wrapper for InsertUserData (size = 8) */
typedef struct hiVENC_USER_DATA_S {
    HI_U8*  pu8Data;        /* 0x00 */
    HI_U32  u32Len;         /* 0x04 */
} VENC_USER_DATA_S;

/* Ioctl wrapper for RoiBgFrameRate (size = 8) */
typedef struct hiVENC_ROI_BG_FRAME_RATE_IOC_S {
    VENC_ROIBG_FRAME_RATE_S stRoiBgFrmRate;
} VENC_ROI_BG_FRAME_RATE_IOC_S;


/* ioctl type = 'E' (0x45) */
#define VENC_CTL_CREATE_CHN          _IOR( 0x45, 0x00, 0x074) /* 0x40744500 */
#define VENC_CTL_DESTROY_CHN         _IO(  0x45, 0x01       ) /* 0x00004501 */
#define VENC_CTL_SET_RC_PARAM        _IO(  0x45, 0x02       ) /* 0x00004502 */
#define VENC_CTL_GET_RC_PARAM        _IO(  0x45, 0x03       ) /* 0x00004503 */
#define VENC_CTL_START_RECV_FRAME    _IOR( 0x45, 0x04, 0x004) /* 0x40044504 */
#define VENC_CTL_STOP_RECV_FRAME     _IO(  0x45, 0x05       ) /* 0x00004505 */
#define VENC_CTL_GET_STREAM          _IOWR(0x45, 0x06, 0x188) /* 0xC1884506 */
#define VENC_CTL_RELEASE_STREAM      _IOR( 0x45, 0x07, 0x180) /* 0x41804507 */
#define VENC_CTL_QUERY_STATUS        _IOW( 0x45, 0x08, 0x058) /* 0x80584508 */
#define VENC_CTL_GET_STREAM_BUF_INFO _IOW( 0x45, 0x09, 0x020) /* 0x80204509 */
#define VENC_CTL_REQUEST_IDR         _IOR( 0x45, 0x0A, 0x004) /* 0x4004450A */
#define VENC_CTL_INSERT_USERDATA     _IOR( 0x45, 0x0B, 0x008) /* 0x4008450B */
#define VENC_CTL_SET_CHN_ID          _IOR( 0x45, 0x0C, 0x004) /* 0x4004450C */
#define VENC_CTL_GET_ROI_ATTR        _IOWR(0x45, 0x0D, 0x020) /* 0xC020450D */
#define VENC_CTL_SET_ROI_ATTR        _IOR( 0x45, 0x0E, 0x020) /* 0x4020450E */
#define VENC_CTL_GET_ROI_ATTR_EX     _IOWR(0x45, 0x0F, 0x058) /* 0xC058450F */
#define VENC_CTL_SET_ROI_ATTR_EX     _IOR( 0x45, 0x10, 0x058) /* 0x40584510 */
#define VENC_CTL_SET_H264_SLICE_SPLIT _IOR( 0x45, 0x11, 0x008) /* 0x40084511 */
#define VENC_CTL_GET_H264_SLICE_SPLIT _IOW( 0x45, 0x12, 0x008) /* 0x80084512 */
#define VENC_CTL_SET_H264_INTRA_PRED _IOR( 0x45, 0x13, 0x004) /* 0x40044513 */
#define VENC_CTL_GET_H264_INTRA_PRED _IOW( 0x45, 0x14, 0x004) /* 0x80044514 */
#define VENC_CTL_SET_H264_TRANS      _IOR( 0x45, 0x15, 0x090) /* 0x40904515 */
#define VENC_CTL_GET_H264_TRANS      _IOW( 0x45, 0x16, 0x090) /* 0x80904516 */
#define VENC_CTL_SET_H264_ENTROPY    _IOR( 0x45, 0x17, 0x010) /* 0x40104517 */
#define VENC_CTL_GET_H264_ENTROPY    _IOW( 0x45, 0x18, 0x010) /* 0x80104518 */
#define VENC_CTL_SET_H264_DBLK       _IOR( 0x45, 0x19, 0x00C) /* 0x400C4519 */
#define VENC_CTL_GET_H264_DBLK       _IOW( 0x45, 0x1A, 0x00C) /* 0x800C451A */
#define VENC_CTL_SET_H264_VUI        _IOR( 0x45, 0x1B, 0x01C) /* 0x401C451B */
#define VENC_CTL_GET_H264_VUI        _IOW( 0x45, 0x1C, 0x01C) /* 0x801C451C */
#define VENC_CTL_SET_JPEG_PARAM      _IOR( 0x45, 0x1D, 0x0C8) /* 0x40C8451D */
#define VENC_CTL_GET_JPEG_PARAM      _IOW( 0x45, 0x1E, 0x0C8) /* 0x80C8451E */
#define VENC_CTL_SET_MJPEG_PARAM     _IOR( 0x45, 0x1F, 0x0C4) /* 0x40C4451F */
#define VENC_CTL_GET_MJPEG_PARAM     _IOW( 0x45, 0x20, 0x0C4) /* 0x80C44520 */
#define VENC_CTL_SET_JPEG_ENC_MODE   _IOR( 0x45, 0x21, 0x004) /* 0x40044521 */
#define VENC_CTL_GET_JPEG_ENC_MODE   _IOW( 0x45, 0x22, 0x004) /* 0x80044522 */
#define VENC_CTL_SET_REF_PARAM       _IOR( 0x45, 0x23, 0x00C) /* 0x400C4523 */
#define VENC_CTL_GET_REF_PARAM       _IOW( 0x45, 0x24, 0x00C) /* 0x800C4524 */
#define VENC_CTL_ENABLE_IDR          _IOR( 0x45, 0x25, 0x004) /* 0x40044525 */
#define VENC_CTL_RESET_CHN           _IO(  0x45, 0x26       ) /* 0x00004526 */
#define VENC_CTL_SEND_FRAME          _IO(  0x45, 0x27       ) /* 0x00004527 */
#define VENC_CTL_SET_ROI_BG_FR       _IOR( 0x45, 0x28, 0x008) /* 0x40084528 */
#define VENC_CTL_GET_ROI_BG_FR       _IOW( 0x45, 0x29, 0x008) /* 0x80084529 */
#define VENC_CTL_SET_H265_SLICE_SPLIT _IOR( 0x45, 0x2A, 0x008) /* 0x4008452A */
#define VENC_CTL_GET_H265_SLICE_SPLIT _IOW( 0x45, 0x2B, 0x008) /* 0x8008452B */
#define VENC_CTL_SET_H265_PRED_UNIT  _IOR( 0x45, 0x2C, 0x008) /* 0x4008452C */
#define VENC_CTL_GET_H265_PRED_UNIT  _IOW( 0x45, 0x2D, 0x008) /* 0x8008452D */
#define VENC_CTL_SET_H265_TRANS      _IO(  0x45, 0x2E       ) /* 0x0000452E */
#define VENC_CTL_GET_H265_TRANS      _IO(  0x45, 0x2F       ) /* 0x0000452F */
#define VENC_CTL_SET_H265_ENTROPY    _IOR( 0x45, 0x30, 0x004) /* 0x40044530 */
#define VENC_CTL_GET_H265_ENTROPY    _IOW( 0x45, 0x31, 0x004) /* 0x80044531 */
#define VENC_CTL_SET_H265_DBLK       _IOR( 0x45, 0x32, 0x00C) /* 0x400C4532 */
#define VENC_CTL_GET_H265_DBLK       _IOW( 0x45, 0x33, 0x00C) /* 0x800C4533 */
#define VENC_CTL_SET_H265_SAO        _IOR( 0x45, 0x34, 0x008) /* 0x40084534 */
#define VENC_CTL_GET_H265_SAO        _IOW( 0x45, 0x35, 0x008) /* 0x80084535 */
#define VENC_CTL_SET_H265_VUI        _IOR( 0x45, 0x36, 0x020) /* 0x40204536 */
#define VENC_CTL_GET_H265_VUI        _IOW( 0x45, 0x37, 0x020) /* 0x80204537 */
#define VENC_CTL_SET_FRAME_LOST      _IOR( 0x45, 0x38, 0x010) /* 0x40104538 */
#define VENC_CTL_GET_FRAME_LOST      _IOW( 0x45, 0x39, 0x010) /* 0x80104539 */
/* 0x3A, 0x3B — unknown / reserved */
#define VENC_CTL_SET_SUPER_FRAME     _IOR( 0x45, 0x3C, 0x014) /* 0x4014453C */
#define VENC_CTL_GET_SUPER_FRAME     _IOW( 0x45, 0x3D, 0x014) /* 0x8014453D */
#define VENC_CTL_SET_CHN_ATTR        _IOR( 0x45, 0x3E, 0x074) /* 0x4074453E */
#define VENC_CTL_GET_CHN_ATTR        _IOWR(0x45, 0x3F, 0x074) /* 0xC074453F */
#define VENC_CTL_ATTACH_VB_POOL      _IOR( 0x45, 0x40, 0x008) /* 0x40084540 */
#define VENC_CTL_DETACH_VB_POOL      _IO(  0x45, 0x41       ) /* 0x00004541 */
#define VENC_CTL_SET_INTRA_REFRESH   _IOR( 0x45, 0x42, 0x010) /* 0x40104542 */
#define VENC_CTL_GET_INTRA_REFRESH   _IOWR(0x45, 0x43, 0x010) /* 0xC0104543 */
#define VENC_CTL_SET_MOD_PARAM       _IOR( 0x45, 0x44, 0x018) /* 0x40184544 */
#define VENC_CTL_GET_MOD_PARAM       _IOWR(0x45, 0x45, 0x018) /* 0xC0184545 */
#define VENC_CTL_SEND_FRAME_EX       _IO(  0x45, 0x46       ) /* 0x00004546 */
#define VENC_CTL_GET_SSE_REGION      _IOWR(0x45, 0x47, 0x018) /* 0xC0184547 */
#define VENC_CTL_SET_SSE_REGION      _IOR( 0x45, 0x48, 0x018) /* 0x40184548 */
#define VENC_CTL_SET_SCENE_MODE      _IOR( 0x45, 0x49, 0x004) /* 0x40044549 */
#define VENC_CTL_GET_SCENE_MODE      _IOW( 0x45, 0x4A, 0x004) /* 0x8004454A */
#define VENC_CTL_SET_CHN_PARAM       _IOR( 0x45, 0x4B, 0x02C) /* 0x402C454B */
#define VENC_CTL_GET_CHN_PARAM       _IOWR(0x45, 0x4C, 0x02C) /* 0xC02C454C */
#define VENC_CTL_GET_FG_PROTECT      _IOWR(0x45, 0x4E, 0x090) /* 0xC090454E */
#define VENC_CTL_SET_FG_PROTECT      _IOR( 0x45, 0x4D, 0x090) /* 0x4090454D */
#define VENC_CTL_SET_DEBREATH        _IOR( 0x45, 0x4F, 0x00C) /* 0x400C454F */
#define VENC_CTL_GET_DEBREATH        _IOW( 0x45, 0x50, 0x00C) /* 0x800C4550 */
#define VENC_CTL_SET_CU_PREDICTION   _IOR( 0x45, 0x51, 0x024) /* 0x40244551 */
#define VENC_CTL_GET_CU_PREDICTION   _IOWR(0x45, 0x52, 0x024) /* 0xC0244552 */
#define VENC_CTL_SET_SKIP_BIAS       _IOR( 0x45, 0x53, 0x024) /* 0x40244553 */
#define VENC_CTL_GET_SKIP_BIAS       _IOWR(0x45, 0x54, 0x024) /* 0xC0244554 */
#define VENC_CTL_SET_HIERARCHICAL_QP _IOR( 0x45, 0x55, 0x024) /* 0x40244555 */
#define VENC_CTL_GET_HIERARCHICAL_QP _IOWR(0x45, 0x56, 0x024) /* 0xC0244556 */
#define VENC_CTL_SET_RC_ADV_PARAM    _IOR( 0x45, 0x57, 0x004) /* 0x40044557 */
#define VENC_CTL_GET_RC_ADV_PARAM    _IOW( 0x45, 0x58, 0x004) /* 0x80044558 */


#endif
