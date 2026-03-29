CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
PROJECT(libraries C ASM)

OPTION(BUILD_SHARED_LIBS "Build shared user-space libraries" ON)
OPTION(BUILD_STATIC_LIBS "Build static user-space libraries" ON)

set(CMAKE_ASM_CREATE_SHARED_LIBRARY ${CMAKE_C_CREATE_SHARED_LIBRARY})

#
# mpi
#

SET(MPI_FILES
    "mpi/af_buf.c"
    "mpi/as_buf.c"
    "mpi/audio_comm.c"
    "mpi/audio_voice_adp.c"
    "mpi/hiisp_gdc_fw_pointquery.c"
    "mpi/hiisp_gdc_fw_user.c"
    "mpi/mpi_adec.c"
    "mpi/mpi_aenc.c"
    "mpi/mpi_ai.c"
    "mpi/mpi_ao.c"
    "mpi/mpi_audio.c"
    "mpi/mpi_bind.c"
    "mpi/mpi_gdc.c"
    "mpi/mpi_region.c"
    "mpi/mpi_sys.c"
    "mpi/mpi_vb.c"
    "mpi/mpi_vdec.c"
    "mpi/mpi_venc.c"
    "mpi/mpi_vgs.c"
    "mpi/mpi_vi.c"
    "mpi/mpi_vo.c"
    "mpi/mpi_vpss.c"
)
ADD_LIBRARY(mpi_obj OBJECT ${MPI_FILES})
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(mpi_shared SHARED $<TARGET_OBJECTS:mpi_obj>)
    SET_TARGET_PROPERTIES(mpi_shared PROPERTIES OUTPUT_NAME mpi)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(mpi_static STATIC $<TARGET_OBJECTS:mpi_obj>)
    SET_TARGET_PROPERTIES(mpi_static PROPERTIES OUTPUT_NAME mpi)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(mpi_obj PUBLIC
    "../common"
    "mpi"
)



#
# ive
#

SET(IVE_FILES
    "ive/ive_queue.c"
    "ive/inner_comm_user.c"
    "ive/check_param_user.c"
    "ive/mpi_ive.c"
)
ADD_LIBRARY(ive_obj OBJECT ${IVE_FILES})
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(ive_shared SHARED $<TARGET_OBJECTS:ive_obj>)
    SET_TARGET_PROPERTIES(ive_shared PROPERTIES OUTPUT_NAME ive)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(ive_static STATIC $<TARGET_OBJECTS:ive_obj>)
    SET_TARGET_PROPERTIES(ive_static PROPERTIES OUTPUT_NAME ive)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(ive_obj PUBLIC
    "../common"
    "ive"
)


#
# md (motion detection)
#

SET(MD_FILES
    "md/inner_comm_user.c"
    "md/check_param.c"
    "md/ivs_md.c"
)
ADD_LIBRARY(md_obj OBJECT ${MD_FILES})
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(md_shared SHARED $<TARGET_OBJECTS:md_obj>)
    SET_TARGET_PROPERTIES(md_shared PROPERTIES OUTPUT_NAME md)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(md_static STATIC $<TARGET_OBJECTS:md_obj>)
    SET_TARGET_PROPERTIES(md_static PROPERTIES OUTPUT_NAME md)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(md_obj PUBLIC
    "../common"
    "md"
)


#
# securec
#

SET(SECUREC_FILES
    "securec/fscanf_s.c"
    "securec/fwscanf_s.c"
    "securec/gets_s.c"
    "securec/memcpy_s.c"
    "securec/memmove_s.c"
    "securec/memset_s.c"
    "securec/scanf_s.c"
    "securec/securecutil.c"
    "securec/secureinput_a.c"
    "securec/secureinput_w.c"
    "securec/secureprintoutput_a.c"
    "securec/secureprintoutput_w.c"
    "securec/snprintf_s.c"
    "securec/sprintf_s.c"
    "securec/sscanf_s.c"
    "securec/strcat_s.c"
    "securec/strcpy_s.c"
    "securec/strncat_s.c"
    "securec/strncpy_s.c"
    "securec/strtok_s.c"
    "securec/swprintf_s.c"
    "securec/swscanf_s.c"
    "securec/vfscanf_s.c"
    "securec/vfwscanf_s.c"
    "securec/vscanf_s.c"
    "securec/vsnprintf_s.c"
    "securec/vsprintf_s.c"
    "securec/vsscanf_s.c"
    "securec/vswprintf_s.c"
    "securec/vswscanf_s.c"
    "securec/vwscanf_s.c"
    "securec/wcscat_s.c"
    "securec/wcscpy_s.c"
    "securec/wcsncat_s.c"
    "securec/wcsncpy_s.c"
    "securec/wcstok_s.c"
    "securec/wmemcpy_s.c"
    "securec/wmemmove_s.c"
    "securec/wscanf_s.c"
)
ADD_LIBRARY(securec_obj OBJECT ${SECUREC_FILES})
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(securec_shared SHARED $<TARGET_OBJECTS:securec_obj>)
    SET_TARGET_PROPERTIES(securec_shared PROPERTIES OUTPUT_NAME securec)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(securec_static STATIC $<TARGET_OBJECTS:securec_obj>)
    SET_TARGET_PROPERTIES(securec_static PROPERTIES OUTPUT_NAME securec)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(securec_obj PUBLIC
    "securec"
)


#
# hi_cipher
#

SET(HI_CIPHER_FILES
    "hi_cipher/mpi_cipher.c"
    "hi_cipher/sys_cipher.c"
    "hi_cipher/unf_cipher.c"
    "hi_cipher/user_osal_lib.c"
)
ADD_LIBRARY(hi_cipher_obj OBJECT ${HI_CIPHER_FILES})
TARGET_COMPILE_OPTIONS(hi_cipher_obj PRIVATE -fcommon)
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(hi_cipher_shared SHARED $<TARGET_OBJECTS:hi_cipher_obj>)
    SET_TARGET_PROPERTIES(hi_cipher_shared PROPERTIES OUTPUT_NAME hi_cipher)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(hi_cipher_static STATIC $<TARGET_OBJECTS:hi_cipher_obj>)
    SET_TARGET_PROPERTIES(hi_cipher_static PROPERTIES OUTPUT_NAME hi_cipher)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(hi_cipher_obj PUBLIC
    "../common"
    "hi_cipher"
)


#
# aacdec (requires libfdk-aac at link time)
# Disabled: vendor source has conflicting type definitions between
# fdk-aac-mod.h (reference header) and FDK_audio.h (pulled via aacdec_lib.h).
# Multiple pre-existing bugs (duplicate variable, missing args, assignment-in-if).
#

# SET(AACDEC_FILES
#     "aacdec/aacdec.c"
# )
# ADD_LIBRARY(aacdec_obj OBJECT ${AACDEC_FILES})
# IF (${BUILD_SHARED_LIBS})
#     ADD_LIBRARY(aacdec_shared SHARED $<TARGET_OBJECTS:aacdec_obj>)
#     SET_TARGET_PROPERTIES(aacdec_shared PROPERTIES OUTPUT_NAME aacdec)
# ENDIF ()
# IF (${BUILD_STATIC_LIBS})
#     ADD_LIBRARY(aacdec_static STATIC $<TARGET_OBJECTS:aacdec_obj>)
#     SET_TARGET_PROPERTIES(aacdec_static PROPERTIES OUTPUT_NAME aacdec)
# ENDIF ()
# TARGET_INCLUDE_DIRECTORIES(aacdec_obj PUBLIC
#     "../common"
#     "aacdec"
# )


#
# aacenc (requires libfdk-aac at link time)
#

SET(AACENC_FILES
    "aacenc/aacenc.c"
)
ADD_LIBRARY(aacenc_obj OBJECT ${AACENC_FILES})
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(aacenc_shared SHARED $<TARGET_OBJECTS:aacenc_obj>)
    SET_TARGET_PROPERTIES(aacenc_shared PROPERTIES OUTPUT_NAME aacenc)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(aacenc_static STATIC $<TARGET_OBJECTS:aacenc_obj>)
    SET_TARGET_PROPERTIES(aacenc_static PROPERTIES OUTPUT_NAME aacenc)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(aacenc_obj PUBLIC
    "../common"
    "aacenc"
)


#
# dnvqe (downlink noise/voice quality enhancement)
#

SET(DNVQE_FILES
    "dnvqe/dnvqe_work.c"
    "dnvqe/hi_dnvqe_wrap.c"
    "dnvqe/hi_audio_dl_work.c"
    "dnvqe/hi_audio_module_wrap.c"
    "dnvqe/hi_resample_work.c"
)
ADD_LIBRARY(dnvqe_obj OBJECT ${DNVQE_FILES})
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(dnvqe_shared SHARED $<TARGET_OBJECTS:dnvqe_obj>)
    SET_TARGET_PROPERTIES(dnvqe_shared PROPERTIES OUTPUT_NAME dnvqe)
    TARGET_LINK_LIBRARIES(dnvqe_shared dl pthread)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(dnvqe_static STATIC $<TARGET_OBJECTS:dnvqe_obj>)
    SET_TARGET_PROPERTIES(dnvqe_static PROPERTIES OUTPUT_NAME dnvqe)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(dnvqe_obj PUBLIC
    "../common"
    "dnvqe"
    "securec"
)


#
# hiae (auto-exposure algorithm library)
#

SET(HIAE_FILES
    "hiae/hi_ae_adp.c"
    "hiae/hi_ae_increment.c"
    "hiae/hi_ae_route.c"
    "hiae/hi_ae_route_ex.c"
    "hiae/hi_auto_exposure.c"
    "hiae/hi_auto_iris.c"
    "hiae/hi_iris_pwm.c"
    "hiae/hi_piris_gpio.c"
    "hiae/mpi_isp_ae.c"
)
ADD_LIBRARY(hiae_obj OBJECT ${HIAE_FILES})
IF (${BUILD_SHARED_LIBS})
    ADD_LIBRARY(hiae_shared SHARED $<TARGET_OBJECTS:hiae_obj>)
    SET_TARGET_PROPERTIES(hiae_shared PROPERTIES OUTPUT_NAME hiae)
    TARGET_LINK_LIBRARIES(hiae_shared pthread m)
ENDIF ()
IF (${BUILD_STATIC_LIBS})
    ADD_LIBRARY(hiae_static STATIC $<TARGET_OBJECTS:hiae_obj>)
    SET_TARGET_PROPERTIES(hiae_static PROPERTIES OUTPUT_NAME hiae)
ENDIF ()
TARGET_INCLUDE_DIRECTORIES(hiae_obj PUBLIC
    "../common"
    "../common/adapt"
    "../drivers/hi3516cv500_isp"
    "../drivers/hi3516cv500_isp/adapt"
    "hiae"
    "mpi"
    "securec"
)


#
# sns_gc2053 (incomplete - assembly deleted, C rewrites not finished)
#

# SET(SNS_GC2053_FILES
#     "sns_gc2053/sns_gc2053.c"
#     "sns_gc2053/gc2053_cmos.c"
#     "sns_gc2053/gc2053_sensor.c"
# )
# ADD_LIBRARY(sns_gc2053_obj OBJECT ${SNS_GC2053_FILES})
# IF (${BUILD_SHARED_LIBS})
#     ADD_LIBRARY(sns_gc2053_shared SHARED $<TARGET_OBJECTS:sns_gc2053_obj>)
#     SET_TARGET_PROPERTIES(sns_gc2053_shared PROPERTIES OUTPUT_NAME sns_gc2053)
# ENDIF ()
# IF (${BUILD_STATIC_LIBS})
#     ADD_LIBRARY(sns_gc2053_static STATIC $<TARGET_OBJECTS:sns_gc2053_obj>)
#     SET_TARGET_PROPERTIES(sns_gc2053_static PROPERTIES OUTPUT_NAME sns_gc2053)
# ENDIF ()
# TARGET_INCLUDE_DIRECTORIES(sns_gc2053_obj PUBLIC "../common")
