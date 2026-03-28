/**
 * Reverse Engineered by TekuConcept on September 19, 2020
 */

#ifndef RE_MPI_VDEC_H
#define RE_MPI_VDEC_H

#include "mpi_vdec.h"
#include "mpi_errno.h"

#define HI_VDEC_MAX_CHN_NUM 16

extern HI_VOID* HI_MPI_SYS_Mmap(HI_U64 u64PhyAddr, HI_U32 u32Size);
extern HI_S32 HI_MPI_SYS_Munmap(HI_VOID *pVirAddr, HI_U32 u32Size);

HI_S32 VDEC_CheckOpen(VDEC_CHN VdChn);

#endif
