/**
 * Reverse Engineered by TekuConcept on September 19, 2020
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "re_mpi_vdec.h"
#include "securec.h"

static pthread_mutex_t s_VdecMutex;
HI_S32 g_s32VdecModParamfd = -1;
HI_S32 g_s32Vdecfd[HI_VDEC_MAX_CHN_NUM] = {
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
};

HI_S32
VDEC_CheckOpen(VDEC_CHN VdChn)
{
    HI_S32 fd;
    char devpath[128];

    pthread_mutex_lock(&s_VdecMutex);

    if (g_s32Vdecfd[VdChn] >= 0) {
        pthread_mutex_unlock(&s_VdecMutex);
        return 0;
    }

    memset_s(devpath, 128, 0, 128);
    snprintf_s(devpath, 128, 9, "/dev/vdec");

    fd = open(devpath, O_RDWR, 0);
    if (fd < 0) {
        g_s32Vdecfd[VdChn] = -1;
        pthread_mutex_unlock(&s_VdecMutex);
        perror("open vdec err");
        return HI_ERR_VDEC_NOMEM;
    }

    g_s32Vdecfd[VdChn] = fd;

    if (ioctl(fd, 0x40044409, &VdChn)) {
        perror("can't bind channel");
        close(fd);
        g_s32Vdecfd[VdChn] = -1;
        pthread_mutex_unlock(&s_VdecMutex);
        return HI_ERR_VDEC_NOMEM;
    }

    pthread_mutex_unlock(&s_VdecMutex);
    return 0;
}

HI_S32
HI_MPI_VDEC_CreateChn(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstAttr) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x40284400u, pstAttr);
}

HI_S32
HI_MPI_VDEC_DestroyChn(VDEC_CHN VdChn)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x4401);
}

HI_S32
HI_MPI_VDEC_GetChnAttr(VDEC_CHN VdChn, VDEC_CHN_ATTR_S *pstAttr)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstAttr) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x80284403u, pstAttr);
}

HI_S32
HI_MPI_VDEC_SetChnAttr(VDEC_CHN VdChn, const VDEC_CHN_ATTR_S *pstAttr)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstAttr) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x40284402u, pstAttr);
}

HI_S32
HI_MPI_VDEC_SendStream(
    VDEC_CHN VdChn,
    const VDEC_STREAM_S *pstStream,
    HI_S32 s32MilliSec)
{
    struct {
        HI_S32 s32MilliSec;
        HI_S32 reserved;
        VDEC_STREAM_S stStream;
    } data;

    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstStream) return HI_ERR_VDEC_NULL_PTR;

    data.s32MilliSec = s32MilliSec;
    memcpy_s(
        &data.stStream,
        sizeof(VDEC_STREAM_S),
        pstStream,
        sizeof(VDEC_STREAM_S));
    return ioctl(g_s32Vdecfd[VdChn], 0x40284405u, &data);
}

HI_S32
HI_MPI_VDEC_GetUserData(
    VDEC_CHN VdChn,
    VDEC_USERDATA_S *pstUserData,
    HI_S32 s32MilliSec)
{
    HI_S32 result;
    struct {
        HI_S32 s32MilliSec;
        VDEC_USERDATA_S *pstUserData;
    } data;

    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    result = VDEC_CheckOpen(VdChn);
    if (result) return result;

    if (!pstUserData) return HI_ERR_VDEC_NULL_PTR;

    data.s32MilliSec = s32MilliSec;
    data.pstUserData = pstUserData;

    result = ioctl(g_s32Vdecfd[VdChn], 0xc008440eu, &data);
    if (result) return result;

    pstUserData->pu8Addr = HI_NULL;

    if (pstUserData->bValid != HI_TRUE) return 0;

    pstUserData->pu8Addr = (HI_U8 *)HI_MPI_SYS_Mmap(
        pstUserData->u64PhyAddr,
        pstUserData->u32Len);

    if (pstUserData->pu8Addr) return 0;

    perror("GetUserData: VirAddr sys mmap err. ");

    if (ioctl(g_s32Vdecfd[VdChn], 0x4018440fu, pstUserData) == 0)
        return -1;

    perror("GetUserData: userdata release err. ");
    return -1;
}

HI_S32
HI_MPI_VDEC_ReleaseUserData(
    VDEC_CHN VdChn,
    const VDEC_USERDATA_S *pstUserData)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstUserData) return HI_ERR_VDEC_NULL_PTR;

    if (pstUserData->bValid == HI_TRUE) {
        if (HI_MPI_SYS_Munmap(pstUserData->pu8Addr, pstUserData->u32Len)) {
            perror("ReleaseUserData VirAddr sys Munmap err!");
            if (ioctl(g_s32Vdecfd[VdChn], 0x4018440fu, pstUserData))
                perror("ReleaseUserData VirAddr sys Munmap err!");
            return HI_ERR_VDEC_BADADDR;
        }
    }

    return ioctl(g_s32Vdecfd[VdChn], 0x4018440fu, pstUserData);
}

HI_S32
HI_MPI_VDEC_GetFrame(
    VDEC_CHN VdChn,
    VIDEO_FRAME_INFO_S *pstFrameInfo,
    HI_S32 s32MilliSec)
{
    struct {
        HI_S32 s32MilliSec;
        VIDEO_FRAME_INFO_S *pstFrameInfo;
    } data;

    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstFrameInfo) return HI_ERR_VDEC_NULL_PTR;

    data.s32MilliSec = s32MilliSec;
    data.pstFrameInfo = pstFrameInfo;

    return ioctl(g_s32Vdecfd[VdChn], 0xc0084410u, &data);
}

HI_S32
HI_MPI_VDEC_ReleaseFrame(
    VDEC_CHN VdChn,
    const VIDEO_FRAME_INFO_S *pstFrameInfo)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstFrameInfo) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x41504411u, pstFrameInfo);
}

HI_S32
HI_MPI_VDEC_StartRecvStream(VDEC_CHN VdChn)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x4406);
}

HI_S32
HI_MPI_VDEC_StopRecvStream(VDEC_CHN VdChn)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x4407);
}

HI_S32
HI_MPI_VDEC_QueryStatus(VDEC_CHN VdChn, VDEC_CHN_STATUS_S *pstStatus)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstStatus) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x80404404u, pstStatus);
}

HI_S32
HI_MPI_VDEC_GetFd(VDEC_CHN VdChn)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return g_s32Vdecfd[VdChn];
}

HI_S32
HI_MPI_VDEC_CloseFd(VDEC_CHN VdChn)
{
    HI_S32 result;

    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    pthread_mutex_lock(&s_VdecMutex);

    if (g_s32Vdecfd[VdChn] < 0) {
        pthread_mutex_unlock(&s_VdecMutex);
        return 0;
    }

    result = close(g_s32Vdecfd[VdChn]);
    if (result) {
        perror("Close VDEC Channel Fd Fail");
        pthread_mutex_unlock(&s_VdecMutex);
        return result;
    }

    g_s32Vdecfd[VdChn] = -1;
    pthread_mutex_unlock(&s_VdecMutex);
    return 0;
}

HI_S32
HI_MPI_VDEC_ResetChn(VDEC_CHN VdChn)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x4408);
}

HI_S32
HI_MPI_VDEC_SetChnParam(VDEC_CHN VdChn, const VDEC_CHN_PARAM_S *pstParam)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstParam) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x401c440au, pstParam);
}

HI_S32
HI_MPI_VDEC_GetChnParam(VDEC_CHN VdChn, VDEC_CHN_PARAM_S *pstParam)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstParam) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x801c440bu, pstParam);
}

HI_S32
HI_MPI_VDEC_SetProtocolParam(
    VDEC_CHN VdChn,
    const VDEC_PRTCL_PARAM_S *pstParam)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstParam) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x4014440cu, pstParam);
}

HI_S32
HI_MPI_VDEC_GetProtocolParam(VDEC_CHN VdChn, VDEC_PRTCL_PARAM_S *pstParam)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstParam) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x8014440du, pstParam);
}

HI_S32
HI_MPI_VDEC_SetUserPic(
    VDEC_CHN VdChn,
    const VIDEO_FRAME_INFO_S *pstUsrPic)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstUsrPic) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x41504412u, pstUsrPic);
}

HI_S32
HI_MPI_VDEC_EnableUserPic(VDEC_CHN VdChn, HI_BOOL bInstant)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x40044413u, &bInstant);
}

HI_S32
HI_MPI_VDEC_DisableUserPic(VDEC_CHN VdChn)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x4414);
}

HI_S32
HI_MPI_VDEC_SetRotation(VDEC_CHN VdChn, ROTATION_E enRotation)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x40044415u, &enRotation);
}

HI_S32
HI_MPI_VDEC_GetRotation(VDEC_CHN VdChn, ROTATION_E *penRotation)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!penRotation) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x80044416u, penRotation);
}

HI_S32
HI_MPI_VDEC_SetDisplayMode(VDEC_CHN VdChn, VIDEO_DISPLAY_MODE_E enDisplayMode)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x40044417u, &enDisplayMode);
}

HI_S32
HI_MPI_VDEC_GetDisplayMode(
    VDEC_CHN VdChn,
    VIDEO_DISPLAY_MODE_E *penDisplayMode)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!penDisplayMode) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x80044418u, penDisplayMode);
}

HI_S32
HI_MPI_VDEC_AttachVbPool(VDEC_CHN VdChn, const VDEC_CHN_POOL_S *pstPool)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstPool) return HI_ERR_VDEC_NULL_PTR;

    return ioctl(g_s32Vdecfd[VdChn], 0x40084419u, pstPool);
}

HI_S32
HI_MPI_VDEC_DetachVbPool(VDEC_CHN VdChn)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (VDEC_CheckOpen(VdChn)) return HI_ERR_VDEC_INVALID_CHNID;

    return ioctl(g_s32Vdecfd[VdChn], 0x441a);
}

static HI_S32
vdec_mod_open_fd(void)
{
    char devpath[128];

    if (g_s32VdecModParamfd >= 0)
        return 0;

    memset_s(devpath, 128, 0, 128);
    snprintf_s(devpath, 128, 9, "/dev/vdec");

    g_s32VdecModParamfd = open(devpath, O_RDWR, 0);
    if (g_s32VdecModParamfd >= 0)
        return 0;

    g_s32VdecModParamfd = -1;
    return -1;
}

HI_S32
HI_MPI_VDEC_SetModParam(const VDEC_MOD_PARAM_S *pstModParam)
{
    if (!pstModParam) return HI_ERR_VDEC_NULL_PTR;

    pthread_mutex_lock(&s_VdecMutex);

    if (vdec_mod_open_fd()) {
        pthread_mutex_unlock(&s_VdecMutex);
        perror("open vdec err");
        return HI_ERR_VDEC_NOMEM;
    }

    pthread_mutex_unlock(&s_VdecMutex);
    return ioctl(g_s32VdecModParamfd, 0x4024441bu, pstModParam);
}

HI_S32
HI_MPI_VDEC_GetModParam(VDEC_MOD_PARAM_S *pstModParam)
{
    if (!pstModParam) return HI_ERR_VDEC_NULL_PTR;

    pthread_mutex_lock(&s_VdecMutex);

    if (vdec_mod_open_fd()) {
        pthread_mutex_unlock(&s_VdecMutex);
        perror("open vdec err");
        return HI_ERR_VDEC_NOMEM;
    }

    pthread_mutex_unlock(&s_VdecMutex);
    return ioctl(g_s32VdecModParamfd, 0x8024441cu, pstModParam);
}

HI_S32
HI_MPI_VDEC_SetUserDataAttr(
    VDEC_CHN VdChn,
    const VDEC_USER_DATA_ATTR_S *pstUserDataAttr)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstUserDataAttr) return HI_ERR_VDEC_NULL_PTR;

    return HI_ERR_VDEC_NOT_SUPPORT;
}

HI_S32
HI_MPI_VDEC_GetUserDataAttr(
    VDEC_CHN VdChn,
    VDEC_USER_DATA_ATTR_S *pstUserDataAttr)
{
    if (VdChn > 15) return HI_ERR_VDEC_INVALID_CHNID;

    if (!pstUserDataAttr) return HI_ERR_VDEC_NULL_PTR;

    return HI_ERR_VDEC_NOT_SUPPORT;
}
