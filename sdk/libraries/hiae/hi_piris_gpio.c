/**
 * Reverse Engineered by TekuConcept on April 28, 2021
 * P-iris GPIO stepper motor control via /dev/piris
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "re_hi_ae_adp.h"
#include "hi_ae_comm.h"
#include "securec.h"

#define RE_DBG_LVL HI_DBG_ERR

#define PIRIS_SET_ORGIN    0x40047002
#define PIRIS_SET_CLOSE    0x40047003
#define PIRIS_SET_FD       0x40047004
#define PIRIS_GET_STATUS   0x80047005

typedef struct hiPIRIS_DATA_S {
    unsigned char ZeroIsMax;
    unsigned int  TotalStep;
    int           CurPos;
} PIRIS_DATA_S;

static HI_S32 g_Pirisfd[AE_CTX_SIZE] = { -1, -1, -1, -1 };
static HI_U8  g_au8PIRISPrintIndex[AE_CTX_SIZE] = { 1, 1, 1, 1 };


static HI_S32 ae_piris_init(VI_PIPE ViPipe)
{
    HI_S32 s32Ret;

    g_Pirisfd[ViPipe] = open("/dev/piris", O_RDONLY, 0x100);
    if (g_Pirisfd[ViPipe] < 0) {
        if (g_au8PIRISPrintIndex[ViPipe] == 1) {
            HI_TRACE_ISP(RE_DBG_LVL,
                "********************* Open piris device error! *********************\n");
            g_au8PIRISPrintIndex[ViPipe] = 0;
        }
        return -1;
    }

    s32Ret = ioctl(g_Pirisfd[ViPipe], PIRIS_SET_FD, &ViPipe);
    if (s32Ret != 0) {
        close(g_Pirisfd[ViPipe]);
        g_Pirisfd[ViPipe] = -1;
        return -1;
    }

    return 0;
}


static HI_S32 ae_piris_exit(VI_PIPE ViPipe)
{
    if (g_Pirisfd[ViPipe] < 0)
        return -1;

    close(g_Pirisfd[ViPipe]);
    g_Pirisfd[ViPipe] = -1;
    return 0;
}


static HI_S32 ae_piris_status_get(VI_PIPE ViPipe)
{
    HI_S32 s32Status = 0;
    HI_S32 s32Ret;

    s32Ret = ioctl(g_Pirisfd[ViPipe], PIRIS_GET_STATUS, &s32Status);
    if (s32Ret == 0)
        return s32Status;

    close(g_Pirisfd[ViPipe]);
    g_Pirisfd[ViPipe] = -1;
    return -1;
}


static HI_S32 ae_piris_origin_set(VI_PIPE ViPipe, HI_BOOL bZeroIsMax,
    HI_S32 s32TotalStep)
{
    HI_S32 s32Ret;
    PIRIS_DATA_S stPirisData;

    stPirisData.ZeroIsMax = (unsigned char)bZeroIsMax;
    stPirisData.TotalStep = (unsigned int)s32TotalStep;

    if (bZeroIsMax == 1)
        stPirisData.CurPos = -(s32TotalStep + 10);
    else
        stPirisData.CurPos = s32TotalStep + 10;

    s32Ret = ioctl(g_Pirisfd[ViPipe], PIRIS_SET_ORGIN, &stPirisData);
    if (s32Ret != 0) {
        close(g_Pirisfd[ViPipe]);
        g_Pirisfd[ViPipe] = -1;
        return -1;
    }

    return 0;
}


static HI_S32 ae_piris_close_set(VI_PIPE ViPipe, HI_BOOL bZeroIsMax,
    HI_S32 s32TotalStep)
{
    HI_S32 s32Ret;
    PIRIS_DATA_S stPirisData;

    stPirisData.ZeroIsMax = (unsigned char)bZeroIsMax;
    stPirisData.TotalStep = (unsigned int)s32TotalStep;

    if (bZeroIsMax == 1)
        stPirisData.CurPos = s32TotalStep - 1;
    else
        stPirisData.CurPos = 0;

    s32Ret = ioctl(g_Pirisfd[ViPipe], PIRIS_SET_CLOSE, &stPirisData);
    if (s32Ret != 0) {
        close(g_Pirisfd[ViPipe]);
        g_Pirisfd[ViPipe] = -1;
        return -1;
    }

    return 0;
}


HI_S32 AePiris_register_callback(VI_PIPE ViPipe)
{
    HI_S32 s32Ret;
    ALG_LIB_S stAeLib;
    HI_VOID *apfnPirisCb[5];

    stAeLib.s32Id = (HI_U8)(ViPipe < 0 ? 0 : ViPipe);
    strncpy_s(stAeLib.acLibName, sizeof(stAeLib.acLibName),
        HI_AE_LIB_NAME, sizeof(stAeLib.acLibName));

    apfnPirisCb[0] = (HI_VOID *)ae_piris_init;
    apfnPirisCb[1] = (HI_VOID *)ae_piris_exit;
    apfnPirisCb[2] = (HI_VOID *)ae_piris_origin_set;
    apfnPirisCb[3] = (HI_VOID *)ae_piris_close_set;
    apfnPirisCb[4] = (HI_VOID *)ae_piris_status_get;

    s32Ret = HI_MPI_AE_IrisRegisterCallBack(ViPipe, &stAeLib, apfnPirisCb);
    if (s32Ret != 0) {
        HI_TRACE_ISP(RE_DBG_LVL,
            "piris register callback function to ae lib failed!\n");
    }

    return s32Ret;
}
