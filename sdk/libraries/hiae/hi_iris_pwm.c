/**
 * Reverse Engineered by TekuConcept on April 28, 2021
 * DC-iris PWM control via /dev/pwm
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

#define ISP_DEV_SET_FD      0x40044900
#define ISP_PWM_NUM_GET     0x8004492F
#define PWM_CMD_WRITE       0x01

typedef struct hiPWM_DATA_S {
    unsigned char pwm_num;
    unsigned int  duty;
    unsigned int  period;
    unsigned char enable;
} PWM_DATA_S;

static HI_U32 g_au32PreIrisValue[AE_CTX_SIZE];
static HI_S32 g_Ispfd[AE_CTX_SIZE]       = { -1, -1, -1, -1 };
static HI_S32 g_Pwmfd[AE_CTX_SIZE]       = { -1, -1, -1, -1 };
static HI_U32 g_au32PwmNum[AE_CTX_SIZE]  = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
static HI_U8  g_au8PWMPrintIndex[AE_CTX_SIZE] = { 1, 1, 1, 1 };


HI_S32 ae_dciris_pwm_init(VI_PIPE ViPipe)
{
    HI_S32 s32Ret;
    HI_U32 u32Tmp;

    if (g_au32PwmNum[ViPipe] == 0xFFFFFFFF) {
        g_Ispfd[ViPipe] = open("/dev/isp_dev", O_RDONLY | O_NOCTTY);
        if (g_Ispfd[ViPipe] < 0) {
            HI_TRACE_ISP(RE_DBG_LVL, "Open isp device error!\n");
            return -1;
        }

        u32Tmp = ViPipe;
        s32Ret = ioctl(g_Ispfd[ViPipe], ISP_DEV_SET_FD, &u32Tmp);
        if (s32Ret != 0) {
            close(g_Ispfd[ViPipe]);
            g_Ispfd[ViPipe] = -1;
            return -1;
        }

        s32Ret = ioctl(g_Ispfd[ViPipe], ISP_PWM_NUM_GET, &g_au32PwmNum[ViPipe]);
        if (s32Ret != 0) {
            HI_TRACE_ISP(RE_DBG_LVL, "get pwm number failed!\n");
            close(g_Ispfd[ViPipe]);
            return -1;
        }

        close(g_Ispfd[ViPipe]);
    }

    g_Pwmfd[ViPipe] = open("/dev/pwm", O_RDONLY | O_NOCTTY);
    if (g_Pwmfd[ViPipe] >= 0)
        return 0;

    if (g_au8PWMPrintIndex[ViPipe] == 1) {
        HI_TRACE_ISP(RE_DBG_LVL,
            "********************* Open pwm device error! *********************\n");
        g_au8PWMPrintIndex[ViPipe] = 0;
    }
    return -1;
}


HI_S32 ae_dciris_pwm_exit(VI_PIPE ViPipe)
{
    if (g_Pwmfd[ViPipe] < 0)
        return -1;

    close(g_Pwmfd[ViPipe]);
    g_Pwmfd[ViPipe] = -1;
    return 0;
}


HI_S32 ae_dciris_pwm_update(VI_PIPE ViPipe, HI_S32 s32IrisDuty)
{
    HI_S32 s32Ret;
    HI_U32 u32Duty;
    PWM_DATA_S stPwmData;

    stPwmData.pwm_num = (unsigned char)g_au32PwmNum[ViPipe];
    stPwmData.duty    = (unsigned int)s32IrisDuty;
    stPwmData.period  = 1000;
    stPwmData.enable  = 1;

    if (s32IrisDuty == (HI_S32)g_au32PreIrisValue[ViPipe])
        return 0;

    u32Duty = (HI_U32)s32IrisDuty;
    if (u32Duty < 100) u32Duty = 100;
    if (u32Duty > 1000) u32Duty = 1000;

    s32Ret = ioctl(g_Pwmfd[ViPipe], PWM_CMD_WRITE, &stPwmData);
    if (s32Ret == 0) {
        g_au32PreIrisValue[ViPipe] = u32Duty;
    } else {
        close(g_Pwmfd[ViPipe]);
        g_Pwmfd[ViPipe] = -1;
    }

    return 0;
}


HI_S32 AeDCiris_register_callback(VI_PIPE ViPipe)
{
    HI_S32 s32Ret;
    ALG_LIB_S stAeLib;
    HI_VOID *apfnIrisCb[3];

    stAeLib.s32Id = (HI_U8)(ViPipe < 0 ? 0 : ViPipe);
    strncpy_s(stAeLib.acLibName, sizeof(stAeLib.acLibName),
        HI_AE_LIB_NAME, sizeof(stAeLib.acLibName));

    apfnIrisCb[0] = (HI_VOID *)ae_dciris_pwm_init;
    apfnIrisCb[1] = (HI_VOID *)ae_dciris_pwm_exit;
    apfnIrisCb[2] = (HI_VOID *)ae_dciris_pwm_update;

    s32Ret = HI_MPI_AE_IrisRegisterCallBack(ViPipe, &stAeLib, apfnIrisCb);
    if (s32Ret != 0) {
        HI_TRACE_ISP(RE_DBG_LVL,
            "dciris register callback function to ae lib failed!\n");
    }

    return s32Ret;
}
