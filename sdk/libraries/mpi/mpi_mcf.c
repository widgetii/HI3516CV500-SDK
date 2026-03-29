/**
 * Reverse Engineered from V2.0.2.1 vendor libmpi mpi_mcf.o
 *
 * Multi-Channel Fusion (MCF) MPI module.
 * Standard libmpi pattern: validate group/channel ID, open /dev/mcf, ioctl.
 *
 * HI_ID_MCF = 61 (0x3D), ioctl type = 0x0C
 * MCF supports 1 group (id must be 0) and 1 channel (id must be 0).
 */

#include "hi_common.h"
#include "hi_errno.h"
#include "hi_type.h"

#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* Error codes: HI_DEF_ERR(HI_ID_MCF, EN_ERR_LEVEL_ERROR, ...) */
#define ERR_MCF_INVALID_DEVID   0xA03D8001  /* EN_ERR_INVALID_DEVID  */
#define ERR_MCF_INVALID_CHNID   0xA03D8002  /* EN_ERR_INVALID_CHNID  */
#define ERR_MCF_NULL_PTR        0xA03D8006  /* EN_ERR_NULL_PTR       */
#define ERR_MCF_NOTREADY        0xA03D8010  /* EN_ERR_SYS_NOTREADY   */
#define ERR_MCF_BUSY            0xA03D8012  /* EN_ERR_BUSY           */

/* ioctl commands — type 0x0C */
#define IOC_MCF_BIND            0x40040C00  /* _IOW(0x0C, 0x00, 4)    — bind grp/chn */
#define IOC_MCF_CREATE_GRP      0x40780C02  /* _IOW(0x0C, 0x02, 120)  — create group */
#define IOC_MCF_DESTROY_GRP     0x00000C03  /* _IO(0x0C, 0x03)        — destroy group */
#define IOC_MCF_START_GRP       0x00000C04  /* _IO(0x0C, 0x04)        — start group */
#define IOC_MCF_STOP_GRP        0x00000C05  /* _IO(0x0C, 0x05)        — stop group */
#define IOC_MCF_SET_GRP_ATTR    0x40780C07  /* _IOW(0x0C, 0x07, 120)  — set group attr */
#define IOC_MCF_GET_GRP_ATTR    0x80780C08  /* _IOR(0x0C, 0x08, 120)  — get group attr */
#define IOC_MCF_ENABLE_CHN      0x00000C09  /* _IO(0x0C, 0x09)        — enable channel */
#define IOC_MCF_DISABLE_CHN     0x00000C0A  /* _IO(0x0C, 0x0A)        — disable channel */
#define IOC_MCF_SET_ALG_PARAM   0x41020C0B  /* _IOW(0x0C, 0x0B, 258)  — set alg param */
#define IOC_MCF_GET_ALG_PARAM   0x81020C0C  /* _IOR(0x0C, 0x0C, 258)  — get alg param */

static pthread_mutex_t g_mcf_mutex = PTHREAD_MUTEX_INITIALIZER;
static HI_S32 g_mcf_chn_fd = -1;
static HI_S32 g_mcf_grp_fd = -1;


static HI_S32
mpi_mcf_check_grp_id(MCF_GRP McfGrp)
{
    if (McfGrp != 0) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:McfGrp(%d) is invalid\r\n",
            __FUNCTION__, __LINE__, McfGrp);
        return ERR_MCF_INVALID_DEVID;
    }
    return 0;
}


static HI_S32
mpi_mcf_check_null_ptr(const HI_VOID *ptr)
{
    if (ptr == NULL) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:NULL pointer \r\n",
            __FUNCTION__, __LINE__);
        return ERR_MCF_NULL_PTR;
    }
    return 0;
}


static HI_S32
mpi_mcf_check_grp_open(MCF_GRP McfGrp)
{
    HI_S32 fd;
    HI_U32 data;

    pthread_mutex_lock(&g_mcf_mutex);

    if (g_mcf_grp_fd >= 0)
        goto end;

    fd = open("/dev/mcf", 0);
    g_mcf_grp_fd = fd;

    if (fd < 0) {
        pthread_mutex_unlock(&g_mcf_mutex);
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:open mcf(%d) err, ret:%d \r\n",
            __FUNCTION__, __LINE__, McfGrp, g_mcf_grp_fd);
        return ERR_MCF_NOTREADY;
    }

    data = (McfGrp << 16) & 0xFF0000;
    if (ioctl(fd, IOC_MCF_BIND, &data)) {
        close(fd);
        g_mcf_grp_fd = -1;
        pthread_mutex_unlock(&g_mcf_mutex);
        return ERR_MCF_NOTREADY;
    }

end:
    pthread_mutex_unlock(&g_mcf_mutex);
    return 0;
}


static HI_S32
mpi_mcf_check_chn_open(MCF_GRP McfGrp, MCF_CHN McfChn)
{
    HI_S32 fd;
    HI_U32 data;
    HI_S32 idx;

    idx = McfGrp + McfChn;

    pthread_mutex_lock(&g_mcf_mutex);

    if (g_mcf_chn_fd >= 0)
        goto end;

    fd = open("/dev/mcf", 0);
    g_mcf_chn_fd = fd;

    if (fd < 0) {
        pthread_mutex_unlock(&g_mcf_mutex);
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:open mcf(%d,%d) err, ret:%d \r\n",
            __FUNCTION__, __LINE__, McfGrp, McfChn, g_mcf_chn_fd);
        return ERR_MCF_NOTREADY;
    }

    data = ((McfGrp << 16) & 0xFF0000) | ((HI_U8)McfChn);
    if (ioctl(fd, IOC_MCF_BIND, &data)) {
        close(fd);
        g_mcf_chn_fd = -1;
        pthread_mutex_unlock(&g_mcf_mutex);
        return ERR_MCF_NOTREADY;
    }

end:
    pthread_mutex_unlock(&g_mcf_mutex);
    return 0;
}


HI_S32
hi_mpi_mcf_create_grp(MCF_GRP McfGrp, const HI_VOID *pstGrpAttr)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_null_ptr(pstGrpAttr);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_CREATE_GRP, pstGrpAttr);
}


HI_S32
hi_mpi_mcf_destroy_grp(MCF_GRP McfGrp)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_DESTROY_GRP);
}


HI_S32
hi_mpi_mcf_start_grp(MCF_GRP McfGrp)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_START_GRP);
}


HI_S32
hi_mpi_mcf_stop_grp(MCF_GRP McfGrp)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_STOP_GRP);
}


HI_S32
hi_mpi_mcf_set_grp_attr(MCF_GRP McfGrp, const HI_VOID *pstGrpAttr)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_null_ptr(pstGrpAttr);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_SET_GRP_ATTR, pstGrpAttr);
}


HI_S32
hi_mpi_mcf_get_grp_attr(MCF_GRP McfGrp, HI_VOID *pstGrpAttr)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_null_ptr(pstGrpAttr);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_GET_GRP_ATTR, pstGrpAttr);
}


HI_S32
hi_mpi_mcf_set_alg_param(MCF_GRP McfGrp, const HI_VOID *pstAlgParam)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_null_ptr(pstAlgParam);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_SET_ALG_PARAM, pstAlgParam);
}


HI_S32
hi_mpi_mcf_get_alg_param(MCF_GRP McfGrp, HI_VOID *pstAlgParam)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_null_ptr(pstAlgParam);
    if (s32Ret != 0)
        return s32Ret;

    s32Ret = mpi_mcf_check_grp_open(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_grp_fd, IOC_MCF_GET_ALG_PARAM, pstAlgParam);
}


HI_S32
hi_mpi_mcf_enable_chn(MCF_GRP McfGrp, MCF_CHN McfChn)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    if (McfChn != 0) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:McfChn(%d) is invalid\r\n",
            __FUNCTION__, __LINE__, McfChn);
        return ERR_MCF_INVALID_CHNID;
    }

    s32Ret = mpi_mcf_check_chn_open(McfGrp, McfChn);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_chn_fd, IOC_MCF_ENABLE_CHN);
}


HI_S32
hi_mpi_mcf_disable_chn(MCF_GRP McfGrp, MCF_CHN McfChn)
{
    HI_S32 s32Ret;

    s32Ret = mpi_mcf_check_grp_id(McfGrp);
    if (s32Ret != 0)
        return s32Ret;

    if (McfChn != 0) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:McfChn(%d) is invalid\r\n",
            __FUNCTION__, __LINE__, McfChn);
        return ERR_MCF_INVALID_CHNID;
    }

    s32Ret = mpi_mcf_check_chn_open(McfGrp, McfChn);
    if (s32Ret != 0)
        return s32Ret;

    return ioctl(g_mcf_chn_fd, IOC_MCF_DISABLE_CHN);
}


HI_S32
hi_mpi_mcf_close_fd(HI_VOID)
{
    HI_S32 s32Ret = 0;

    pthread_mutex_lock(&g_mcf_mutex);

    /* Close channel fd */
    if (g_mcf_chn_fd >= 0) {
        if (close(g_mcf_chn_fd) != 0) {
            perror("close mcf chn fd");

            /* Still try to close grp fd */
            if (g_mcf_grp_fd >= 0) {
                if (close(g_mcf_grp_fd) != 0) {
                    perror("close mcf grp fd");
                    pthread_mutex_unlock(&g_mcf_mutex);
                    return ERR_MCF_BUSY;
                }
                g_mcf_grp_fd = -1;
            }

            pthread_mutex_unlock(&g_mcf_mutex);
            return ERR_MCF_BUSY;
        }
        g_mcf_chn_fd = -1;
    }

    /* Close group fd */
    if (g_mcf_grp_fd >= 0) {
        if (close(g_mcf_grp_fd) != 0) {
            perror("close mcf grp fd");
            pthread_mutex_unlock(&g_mcf_mutex);
            return ERR_MCF_BUSY;
        }
        g_mcf_grp_fd = -1;
        pthread_mutex_unlock(&g_mcf_mutex);
        return 0;
    }

    /* grp fd was already closed */
    pthread_mutex_unlock(&g_mcf_mutex);
    return 0;
}