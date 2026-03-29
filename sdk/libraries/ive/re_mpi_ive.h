/**
 * Reverse Engineered by TekuConcept
 * IVE (Image Vector Engine) internal header
 */

#ifndef RE_MPI_IVE_H
#define RE_MPI_IVE_H

#include "hi_ive.h"
#include "mpi_ive.h"
#include "hi_debug.h"
#include "hi_comm_ive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "securec.h"

#define HI_TRACE_IVE(level, fmt, ...)                                                                          \
    do {                                                                                                       \
        HI_TRACE(level, HI_ID_IVE, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define HI_TRACE_MD(level, fmt, ...)                                                                          \
    do {                                                                                                       \
        HI_TRACE(level, HI_ID_MD, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

/* Global state */
extern HI_S32 s_s32IveFd;
extern pthread_mutex_t s_IveMutex;

/* ioctl type byte */
#define IOC_TYPE_IVE 'F'

/* ioctl command definitions - extracted from MOV/MOVT pairs in mpi_ive.S */
#define IOC_IVE_DMA                 0xc0684600  /* cmd 0x00, size 104 */
#define IOC_IVE_FILTER              0xc0b84601  /* cmd 0x01, size 184 */
#define IOC_IVE_CSC                 0xc0a04602  /* cmd 0x02, size 160 */
#define IOC_IVE_FILTER_AND_CSC      0xc0c04603  /* cmd 0x03, size 192 */
#define IOC_IVE_SOBEL               0xc1084604  /* cmd 0x04, size 264 */
#define IOC_IVE_MAG_AND_ANG         0xc1084605  /* cmd 0x05, size 264 */
#define IOC_IVE_DILATE              0xc0b84606  /* cmd 0x06, size 184 */
#define IOC_IVE_ERODE               0xc0b84607  /* cmd 0x07, size 184 */
#define IOC_IVE_THRESH              0xc0a84608  /* cmd 0x08, size 168 */
#define IOC_IVE_AND                 0xc0e84609  /* cmd 0x09, size 232 */
#define IOC_IVE_SUB                 0xc0e8460a  /* cmd 0x0a, size 232 */
#define IOC_IVE_OR                  0xc0e8460b  /* cmd 0x0b, size 232 */
#define IOC_IVE_INTEG               0xc0a0460c  /* cmd 0x0c, size 160 */
#define IOC_IVE_HIST                0xc070460d  /* cmd 0x0d, size 112 */
#define IOC_IVE_THRESH_S16          0xc0a8460e  /* cmd 0x0e, size 168 */
#define IOC_IVE_THRESH_U16          0xc0a8460f  /* cmd 0x0f, size 168 */
#define IOC_IVE_16BIT_TO_8BIT       0xc0a84610  /* cmd 0x10, size 168 */
#define IOC_IVE_ORD_STAT_FILTER     0xc0a04611  /* cmd 0x11, size 160 */
#define IOC_IVE_MAP                 0xc0b84613  /* cmd 0x13, size 184 */
#define IOC_IVE_ADD                 0xc0e84614  /* cmd 0x14, size 232 */
#define IOC_IVE_XOR                 0xc0e84615  /* cmd 0x15, size 232 */
#define IOC_IVE_NCC                 0xc0b84616  /* cmd 0x16, size 184 */
#define IOC_IVE_CCL                 0xc0784617  /* cmd 0x17, size 120 */
#define IOC_IVE_GMM                 0xc1184618  /* cmd 0x18, size 280 */
#define IOC_IVE_CANNY_HYS_EDGE     0xc0f04619  /* cmd 0x19, size 240 */
#define IOC_IVE_LBP                 0xc0a8461a  /* cmd 0x1a, size 168 */
#define IOC_IVE_NORM_GRAD           0xc150461b  /* cmd 0x1b, size 336 */
#define IOC_IVE_LK_OPTICAL_FLOW_PYR 0xc2c0461c /* cmd 0x1c, size 704 */
#define IOC_IVE_GRAD_FG             0xc138461d  /* cmd 0x1d, size 312 */
#define IOC_IVE_MATCH_BG_MODEL      0xc178461e  /* cmd 0x1e, size 376 */
#define IOC_IVE_UPDATE_BG_MODEL     0xc1d8461f  /* cmd 0x1f, size 472 */
#define IOC_IVE_ANN_MLP_PREDICT     0xc0b04621  /* cmd 0x21, size 176 */
#define IOC_IVE_SVM_PREDICT         0xc0c04622  /* cmd 0x22, size 192 */
#define IOC_IVE_SAD                 0xc1384629  /* cmd 0x29, size 312 */
#define IOC_IVE_EQUALIZE_HIST       0xc0b8462a  /* cmd 0x2a, size 184 */
#define IOC_IVE_ST_CANDI_CORNER     0xc0c0462b  /* cmd 0x2b, size 192 */
#define IOC_IVE_QUERY               0xc00c462c  /* cmd 0x2c, size 12 */
#define IOC_IVE_GMM2                0xc1a8462d  /* cmd 0x2d, size 424 */
#define IOC_IVE_RESIZE              0xc008462e  /* cmd 0x2e, size 8 */
#define IOC_IVE_CNN_PREDICT         0xd338462f  /* cmd 0x2f, size 4920 */
#define IOC_IVE_KCF_PROCESS         0xc0084633  /* cmd 0x33, size 8 */
#define IOC_IVE_HOG                 0xd0684634  /* cmd 0x34, size 4200 */
#define IOC_IVE_PERSP_TRANS         0xdc604635  /* cmd 0x35, size 7264 */
#define IOC_IVE_MD_PROC_INIT        0x80104664  /* cmd 0x64, size 16 */
#define IOC_IVE_MD_PROC_BEGIN       0x00004665  /* cmd 0x65 */
#define IOC_IVE_MD_PROC_END         0x00004666  /* cmd 0x66 */
#define IOC_IVE_MD_PROC_EXIT        0x00004667  /* cmd 0x67 */

/* IVE_IMAGE_S is 72 bytes: 3*u64(24) + 3*u64(24) + 3*u32(12) + u32(4) + u32(4) + enum(4) = 72 */
/* IVE_DATA_S is 32 bytes: u64(8) + u64(8) + u32(4) + u32(4) + u32(4) + u32(4) = 32 */
/* IVE_MEM_INFO_S is 20 bytes: u64(8) + u64(8) + u32(4) = 20, padded to 24 */

/* KCF TmpBuf size for IVE_CreateObjList */
#define IVE_KCF_TMP_BUF_SIZE  22656  /* 0x5880 */
/* Bytes per KCF obj node in internal tracking */
#define IVE_KCF_OBJ_STRIDE    47616  /* 0xBA00 */
#define IVE_KCF_HOG_SIZE      8192   /* 0x2000 */
#define IVE_KCF_DST_SIZE      16     /* 0x10 */

/* Poison values written into removed list nodes */
#define IVE_LIST_POISON1  0x00100100  /* MOVW #256; MOVT #16 */
#define IVE_LIST_POISON2  0x00200200  /* MOVW #512; MOVT #32 */

/* inner_comm_user.c forward declarations */
HI_S32 IveCheckStrideUser(HI_U32 u32Stride, HI_U32 u32Width, HI_U32 u32Align);
HI_S32 MdCheckStrideUser(HI_U32 u32Stride, HI_U32 u32Width, HI_U32 u32Align);
HI_S32 IveCheckWAndHUser(HI_U32 u32Width, HI_U32 u32Height);
HI_S32 MdCheckWAndHUser(HI_U32 u32Width, HI_U32 u32Height);
HI_S32 IveCheckImageUser(IVE_IMAGE_S *pstImage, HI_U32 u32MinWidth, HI_U32 u32MaxWidth,
    HI_U32 u32MinHeight, HI_U32 u32MaxHeight, HI_U32 u32Align, HI_U8 u8PlaneCheck);
HI_S32 MdCheckImageUser(IVE_IMAGE_S *pstImage, HI_U32 u32MinWidth, HI_U32 u32MaxWidth,
    HI_U32 u32MinHeight, HI_U32 u32MaxHeight, HI_U32 u32Align, HI_U8 u8PlaneCheck);
HI_S32 IveCheckResRelationUser(IVE_IMAGE_S *pstSrc, IVE_IMAGE_S *pstDst);

FILE *IveOpenFile(const HI_CHAR *pchFileName, const HI_CHAR *pchMode);
HI_VOID IveCloseFile(FILE *fp);
HI_S32 IveMalloc(HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr, const HI_CHAR *pchName, HI_U32 u32Size);
HI_VOID IveFree(HI_U64 u64PhyAddr, HI_VOID *pVirAddr);
HI_S32 IveMalloc_Cached(HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr, HI_U32 u32Size);
HI_S32 IveFlushCache(HI_U64 u64PhyAddr, HI_VOID *pVirAddr, HI_U32 u32Size);

/* ive_queue.c forward declarations */
HI_S32 IVE_CreateObjList(IVE_MEM_INFO_S *pstMem, IVE_KCF_OBJ_LIST_S *pstObjList, HI_U32 u32MaxObjNum);
HI_VOID IVE_DestroyObjList(IVE_KCF_OBJ_LIST_S *pstObjList);
IVE_KCF_OBJ_NODE_S *IVE_ObjListGetFree(IVE_KCF_OBJ_LIST_S *pstObjList);
HI_VOID IVE_ObjListPutFree(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode);
HI_U32 IVE_ObjListGetFreeNum(IVE_KCF_OBJ_LIST_S *pstObjList);
HI_U32 IVE_ObjListGetTrainNum(IVE_KCF_OBJ_LIST_S *pstObjList);
HI_U32 IVE_ObjListGetTrackNum(IVE_KCF_OBJ_LIST_S *pstObjList);
HI_VOID IVE_ObjListPutTrack(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode);
HI_VOID IVE_ObjListPutTrain(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode);
IVE_KCF_OBJ_NODE_S *IVE_ObjListGetTrain(IVE_KCF_OBJ_LIST_S *pstObjList);
IVE_KCF_OBJ_NODE_S *IVE_ObjListQueryTrain(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode);
IVE_KCF_OBJ_NODE_S *IVE_ObjListGetTrack(IVE_KCF_OBJ_LIST_S *pstObjList);
IVE_KCF_OBJ_NODE_S *IVE_ObjListQueryTrack(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode);
HI_S32 IVE_ObjFreeTrackNodeByNode(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_OBJ_NODE_S *pstNode);

/* check_param_user.c forward declarations */
HI_S32 IveCheckDMAParamUser(IVE_DATA_S *pstSrc, IVE_DATA_S *pstDst, IVE_DMA_CTRL_S *pstCtrl);
HI_S32 IveCheckFilterParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_FILTER_CTRL_S *pstCtrl);
HI_S32 IveCheckCSCParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_CSC_CTRL_S *pstCtrl);
HI_S32 IveCheckFilterAndCSCParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_FILTER_AND_CSC_CTRL_S *pstCtrl);
HI_S32 IveCheckSobelParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV, IVE_SOBEL_CTRL_S *pstCtrl);
HI_S32 IveCheckMagAndAngParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstMag, IVE_DST_IMAGE_S *pstDstAng, IVE_MAG_AND_ANG_CTRL_S *pstCtrl);
HI_S32 IveCheckDilateParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_DILATE_CTRL_S *pstCtrl);
HI_S32 IveCheckErodeParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_ERODE_CTRL_S *pstCtrl);
HI_S32 IveCheckThreshParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_THRESH_CTRL_S *pstCtrl);
HI_S32 IveCheckAndParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst);
HI_S32 IveCheckSubParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst, IVE_SUB_CTRL_S *pstCtrl);
HI_S32 IveCheckOrParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst);
HI_S32 IveCheckIntegParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_INTEG_CTRL_S *pstCtrl);
HI_S32 IveCheckHistParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_MEM_INFO_S *pstDst);
HI_S32 IveCheckThresh_S16ParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_THRESH_S16_CTRL_S *pstCtrl);
HI_S32 IveCheckThresh_U16ParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_THRESH_U16_CTRL_S *pstCtrl);
HI_S32 IveCheck16BitTo8BitParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_16BIT_TO_8BIT_CTRL_S *pstCtrl);
HI_S32 IveCheckOrdStatFilterParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_ORD_STAT_FILTER_CTRL_S *pstCtrl);
HI_S32 IveCheckEqualizeHistParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_EQUALIZE_HIST_CTRL_S *pstCtrl);
HI_S32 IveCheckAddParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst, IVE_ADD_CTRL_S *pstCtrl);
HI_S32 IveCheckXorParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst);
HI_S32 IveCheckNCCParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_MEM_INFO_S *pstDst);
HI_S32 IveCheckGMMParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstFg, IVE_DST_IMAGE_S *pstBg, IVE_MEM_INFO_S *pstModel, IVE_GMM_CTRL_S *pstCtrl);
HI_S32 IveCheckCannyHysEdgeParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstEdge, IVE_DST_MEM_INFO_S *pstStack, IVE_CANNY_HYS_EDGE_CTRL_S *pstCtrl);
HI_S32 IveCheckCannyEdgeParamUser(IVE_DST_IMAGE_S *pstEdge, IVE_MEM_INFO_S *pstStack);
HI_S32 IveCheckLBPParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_LBP_CTRL_S *pstCtrl);
HI_S32 IveCheckNormGradParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV, IVE_DST_IMAGE_S *pstDstHV, IVE_NORM_GRAD_CTRL_S *pstCtrl);
HI_S32 IveCheckSTCandiCornerParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_ST_CANDI_CORNER_CTRL_S *pstCtrl);
HI_S32 IveCheckGradFgParamUser(IVE_SRC_IMAGE_S *pstBgDiffFg, IVE_SRC_IMAGE_S *pstCurGrad, IVE_SRC_IMAGE_S *pstBgGrad, IVE_DST_IMAGE_S *pstGradFg, IVE_GRAD_FG_CTRL_S *pstCtrl);
HI_S32 IveCheckMatchBgModelParamUser(IVE_SRC_IMAGE_S *pstCurImg, IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstDiffFg, IVE_DST_MEM_INFO_S *pstStatData, IVE_MATCH_BG_MODEL_CTRL_S *pstCtrl);
HI_S32 IveCheckUpdateBgModelParamUser(IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstBgImg, IVE_DST_IMAGE_S *pstChgSta, IVE_DST_MEM_INFO_S *pstStatData, IVE_UPDATE_BG_MODEL_CTRL_S *pstCtrl);
HI_S32 IveCheckSADParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstSad, IVE_DST_IMAGE_S *pstThr, IVE_SAD_CTRL_S *pstCtrl);
HI_S32 IveCheckCCLParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_MEM_INFO_S *pstDst, IVE_CCL_CTRL_S *pstCtrl);
HI_S32 IveCheckGMM2ParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_IMAGE_S *pstFactor, IVE_DST_IMAGE_S *pstFg, IVE_DST_IMAGE_S *pstBg, IVE_DST_IMAGE_S *pstMatchModelInfo, IVE_MEM_INFO_S *pstModel, IVE_GMM2_CTRL_S *pstCtrl);
HI_S32 IveCheckLKOpticalFlowPyrParamUser(IVE_SRC_IMAGE_S astSrcPrevPyr[], IVE_SRC_IMAGE_S astSrcNextPyr[], IVE_SRC_MEM_INFO_S *pstPrevPts, IVE_MEM_INFO_S *pstNextPts, IVE_DST_MEM_INFO_S *pstStatus, IVE_DST_MEM_INFO_S *pstErr, IVE_LK_OPTICAL_FLOW_PYR_CTRL_S *pstCtrl);
HI_S32 IveCheckMapParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_MEM_INFO_S *pstMap, IVE_DST_IMAGE_S *pstDst, IVE_MAP_CTRL_S *pstCtrl);
HI_S32 IveCheckResizeParamUser(IVE_SRC_IMAGE_S astSrc[], IVE_DST_IMAGE_S astDst[], IVE_RESIZE_CTRL_S *pstCtrl);
HI_S32 IveCheckCNNPredictParamUser(IVE_SRC_IMAGE_S astSrc[], IVE_SRC_MEM_INFO_S *pstModel, IVE_DST_BLOB_S astDst[], IVE_CNN_CTRL_S *pstCtrl);
HI_S32 IveCheckANNMLPPredictParamUser(IVE_SRC_DATA_S *pstSrc, IVE_LOOK_UP_TABLE_S *pstActivFuncTab,
    IVE_ANN_MLP_MODEL_S *pstAnnMlpModel, IVE_DST_DATA_S *pstDst, IVE_SRC_MEM_INFO_S *pstModel);
HI_S32 IveCheckSVMPredictParamUser(IVE_SRC_DATA_S *pstSrc, IVE_LOOK_UP_TABLE_S *pstKernelTab,
    IVE_SVM_MODEL_S *pstSvmModel, IVE_DST_DATA_S *pstDstVote, IVE_SRC_MEM_INFO_S *pstModel);
HI_S32 IveCheckPerspTransParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_RECT_U32_S astRoi[],
    IVE_SRC_MEM_INFO_S astPointPair[], IVE_DST_IMAGE_S astDst[], IVE_MEM_INFO_S *pstMem,
    IVE_PERSP_TRANS_CTRL_S *pstPerspTransCtrl);
HI_S32 IveCheckSTCornerParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst,
    IVE_POINT_U16_S *pstCorner, IVE_ST_CANDI_CORNER_CTRL_S *pstCtrl);
HI_S32 IveCheckHogParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_RECT_U32_S astRoi[],
    IVE_DST_BLOB_S astDst[], IVE_HOG_CTRL_S *pstHogCtrl);
HI_S32 IveCheckKcfParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_IMAGE_S *pstBg,
    IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_PRO_CTRL_S *pstKcfProCtrl);
HI_S32 IveCheckKcfGaussPeakParamUser(HI_U3Q5 u3q5Padding, IVE_DST_MEM_INFO_S *pstGaussPeak);
HI_S32 IveCheckKcfCosWinParamUser(IVE_DST_MEM_INFO_S *pstCosWinX, IVE_DST_MEM_INFO_S *pstCosWinY);
HI_S32 IveCheckKcfObjListParamUser(IVE_MEM_INFO_S *pstMem, HI_U32 u32MaxObjNum,
    IVE_KCF_OBJ_LIST_S *pstObjList);
HI_S32 IveCheckKcfGetTrainObjParamUser(HI_U3Q5 u3q5Padding, IVE_ROI_INFO_S astRoiInfo[],
    HI_U32 u32ObjNum, IVE_MEM_INFO_S *pstCosWinX, IVE_MEM_INFO_S *pstCosWinY,
    IVE_MEM_INFO_S *pstGaussPeak, IVE_KCF_OBJ_LIST_S *pstObjList);
HI_VOID IveGetHogFeatureRect(IVE_ROI_INFO_S *pstRoiInfo, HI_U3Q5 u3q5Padding,
    HI_U32 *pu32Height, HI_U32 *pu32Width);
HI_S32 IveCheckKcfGetObjParamUser(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_BBOX_S astBbox[],
    HI_U32 *pu32BboxObjNum, IVE_KCF_BBOX_CTRL_S *pstKcfBboxCtrl);
HI_S32 IveCheckKcfJudgeObjBboxParamUser(IVE_ROI_INFO_S *pstRoiInfo, IVE_KCF_BBOX_S *pstBbox,
    HI_BOOL *pbTrackOk);
HI_S32 IveCheckKcfObjUpdateParamUser(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_BBOX_S astBbox[],
    HI_U32 u32BboxObjNum);

#endif /* RE_MPI_IVE_H */
