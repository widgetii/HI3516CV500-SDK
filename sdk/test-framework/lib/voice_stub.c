/* Weak stubs — overridden by real implementations from .a libraries */
#define WEAK __attribute__((weak))

/* Stubs for VoiceEngine symbols that libmpi's audio_voice_adp.c references */
WEAK int HI_VOICE_DecReset(void *h) { return 0; }
WEAK int HI_VOICE_DecodeFrame(void *h, void *in, void *out, void *cfg) { return 0; }
WEAK int HI_VOICE_EncReset(void *h) { return 0; }
WEAK int HI_VOICE_EncodeFrame(void *h, void *in, void *out, void *cfg) { return 0; }
WEAK int HI_VOICE_EngineOpen(void *cfg, void **h) { return 0; }
WEAK int HI_VOICE_EngineClose(void *h) { return 0; }
WEAK int HI_VOICE_EngineProcess(void *h, void *in, void *out) { return 0; }
WEAK int HI_VOICE_GetConfig(void *h, void *cfg) { return 0; }
WEAK int HI_VOICE_SetConfig(void *h, void *cfg) { return 0; }

/* VQE comparison stubs (referenced by mpi_ao.c / mpi_ai.c) */
WEAK int mpi_vqe_compare_hpf_cfg(void *a, void *b) { return 0; }
WEAK int mpi_vqe_compare_anr_cfg(void *a, void *b) { return 0; }
WEAK int mpi_vqe_compare_agc_cfg(void *a, void *b) { return 0; }
WEAK int mpi_vqe_compare_eq_cfg(void *a, void *b) { return 0; }
WEAK int ao_check_agc(int d, int c, void *p) { return 0; }
WEAK int ao_check_hpf(int d, int c, void *p) { return 0; }
WEAK int ao_check_anr(int d, int c, void *p) { return 0; }
WEAK int ao_check_eq(int d, int c, void *p) { return 0; }
WEAK int ao_check_frame_info(void *p) { return 0; }
WEAK int ai_compare_agc_attr(void *a, void *b) { return 0; }
WEAK int HI_UPVQE_Create(void **h, void *a) { return -1; }
WEAK int HI_UPVQE_Destroy(void *h) { return 0; }
WEAK void *HI_MPI_SYS_Mmap(unsigned long long phys, unsigned int size) { return 0; }
WEAK int HI_MPI_SYS_Munmap(void *p, unsigned int size) { return 0; }
WEAK unsigned long long HI_MPI_SYS_GetTimeStamp(void) { return 0; }
WEAK int ao_check_vqe(int d, int c, void *p) { return 0; }
WEAK int ao_parse_sound_mode(void *a, void *b) { return 0; }
WEAK int HI_UPVQE_GetConfig(void *h, void *c) { return 0; }
WEAK int HI_UPVQE_GetVolume(void *h, void *v) { return 0; }
WEAK int HI_UPVQE_ReadFrame(void *h, void *f, void *a) { return 0; }
WEAK int HI_UPVQE_SetVolume(void *h, int v) { return 0; }
WEAK int HI_UPVQE_WriteFrame(void *h, void *f, void *a) { return 0; }

/* V2.0.2.1 lowercase VQE names */
WEAK int hi_upvqe_create(void **h, void *a) { return -1; }
WEAK int hi_upvqe_destroy(void *h) { return 0; }
WEAK int hi_upvqe_set_volume(void *h, int v) { return 0; }
WEAK int hi_upvqe_get_volume(void *h, void *v) { return 0; }
WEAK int hi_dnvqe_create(void **h, void *a) { return -1; }
WEAK int hi_dnvqe_destroy(void *h) { return 0; }
WEAK int hi_dnvqe_get_config(void *h, void *c) { return 0; }
WEAK int hi_dnvqe_process_frame(void *h, void *i, void *o) { return 0; }
WEAK int hi_dnvqe_write_frame(void *h, void *i, void *o) { return 0; }
WEAK int hi_dnvqe_read_frame(void *h, void *o, int n, int b) { return 0; }
WEAK int hi_dnvqe_get_version(void *v) { return 0; }
WEAK int hi_upvqe_get_config(void *h, void *c) { return 0; }
WEAK int hi_upvqe_write_frame(void *h, void *i, void *o) { return 0; }
WEAK int hi_upvqe_read_frame(void *h, void *o, int n, int b) { return 0; }

/* Internal MPI symbols referenced across .o boundaries */
WEAK int MPI_VI_CheckPipeId(int p) { return 0; }
WEAK int MPI_VI_CheckChnId(int c) { return 0; }
WEAK int MPI_VI_CheckPhyChnId(int c) { return 0; }
WEAK int MPI_VI_CheckExtChnId(int c) { return 0; }
WEAK int MPI_VI_CheckNullPtr(void) { return 0; }
WEAK int MPI_VI_CheckChnOpen(int p, int c) { return 0; }
WEAK int MPI_VI_CheckPipeOpen(int p) { return 0; }
WEAK int GDC_Spread_CFG(void *a, void *b, unsigned int c, void *d, void *e) { return 0; }
WEAK int LDC_COUNT_BITS(unsigned int v) { int c = 0; while (v) { c += v & 1; v >>= 1; } return c; }
