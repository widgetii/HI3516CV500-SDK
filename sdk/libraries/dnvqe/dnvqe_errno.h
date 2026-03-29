#ifndef HI_DNVQE_ERRNO_H
#define HI_DNVQE_ERRNO_H

/* DNVQE error codes (module 0xA136) */
#define ERR_DNVQE_NULL_PTR            0xA1360002
#define ERR_DNVQE_NOMEM               0xA1360003
#define ERR_DNVQE_PROCESS_FAIL        0xA1360004
#define ERR_DNVQE_ILLEGAL_PARAM       0xA1360005
#define ERR_DNVQE_CACHE_FULL          0xA1360006
#define ERR_DNVQE_CACHE_EMPTY         0xA1360007
#define ERR_DNVQE_CACHE_BUSY          0xA1360008
#define ERR_DNVQE_MODULE_INIT         0xA1360009

/* Resampler error codes (module 0xA334) */
#define ERR_RESAMPLER_ILLEGAL_PARAM   0xA3340002
#define ERR_RESAMPLER_NOMEM           0xA3340003
#define ERR_RESAMPLER_NULL_PTR        0xA3340004

#endif
