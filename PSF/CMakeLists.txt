ADD_SUBDIRECTORY(FDR)

INCLUDE_DIRECTORIES(
${MINC2_INCLUDE_PATH} ${HDF5_INCLUDE_PATH} ${NETCDF_INCLUDE_PATH}
${OPT_recon_SOURCE_DIR}/MINC
${OPT_recon_SOURCE_DIR}/GetOPTData
${OPT_recon_SOURCE_DIR}/PSF/FDR
)
LINK_DIRECTORIES(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

SET(DECONV_SRC
deconvInFdrFilter.c
)
ADD_EXECUTABLE(opt_deconvInFdrFilter ${DECONV_SRC})
TARGET_LINK_LIBRARIES(opt_deconvInFdrFilter fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_deconvInFdrFilter DESTINATION bin)

SET(ROLLOFF_SRC
rollofffilter.c
)
ADD_EXECUTABLE(opt_rollofffilter ${ROLLOFF_SRC})
TARGET_LINK_LIBRARIES(opt_rollofffilter fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_rollofffilter DESTINATION bin)

SET(BANDLIMIT_SRC
bandlimitfilter.c
)
ADD_EXECUTABLE(opt_bandlimitfilter ${BANDLIMIT_SRC})
TARGET_LINK_LIBRARIES(opt_bandlimitfilter fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_bandlimitfilter DESTINATION bin)

SET(FDR_SRC
psfInFdrFilter.c
)
ADD_EXECUTABLE(opt_psfInFdrFilter ${FDR_SRC})
TARGET_LINK_LIBRARIES(opt_psfInFdrFilter fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS opt_psfInFdrFilter DESTINATION bin)

SET(LIMITRECOVERY_SRC
limitrecoveryfilter.c
)
ADD_EXECUTABLE(opt_limitrecoveryfilter ${LIMITRECOVERY_SRC})
TARGET_LINK_LIBRARIES(opt_limitrecoveryfilter fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS opt_limitrecoveryfilter DESTINATION bin)

#SET(NORMALIZE_PSF_SRC
#normalizepsf.c
#)
#ADD_EXECUTABLE(opt_normalizepsf ${NORMALIZE_PSF_SRC})
#TARGET_LINK_LIBRARIES(opt_normalizepsf fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS opt_normalizepsf DESTINATION bin)

SET(PSF_STACK_FFT_SRC
psfstackfft.c
)
ADD_EXECUTABLE(opt_psfstackfft ${PSF_STACK_FFT_SRC})
TARGET_LINK_LIBRARIES(opt_psfstackfft  fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS opt_psfstackfft DESTINATION bin)

SET(MINC_MAGNITUDE_SRC
mincmagnitude.c
)
ADD_EXECUTABLE(mincmagnitude ${MINC_MAGNITUDE_SRC})
TARGET_LINK_LIBRARIES(mincmagnitude  fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS mincmagnitude DESTINATION bin)

SET(INVERSE_FFT_SRC
inverse3dfft.c
)
ADD_EXECUTABLE(opt_inverse3dfft ${INVERSE_FFT_SRC})
TARGET_LINK_LIBRARIES(opt_inverse3dfft  fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS opt_inverse3dfft DESTINATION bin)

SET(COMPLEX_MULT_SRC
complexmultiply.c
)
ADD_EXECUTABLE(opt_complexmultiply ${COMPLEX_MULT_SRC})
TARGET_LINK_LIBRARIES(opt_complexmultiply  fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS opt_complexmultiply DESTINATION bin)

SET(COMPLEX_REAL_MULT_SRC
complexrealmultiply.c
)
ADD_EXECUTABLE(opt_complexrealmultiply ${COMPLEX_REAL_MULT_SRC})
TARGET_LINK_LIBRARIES(opt_complexrealmultiply  fftw3f fftw3 useMINC2 FDR)
#INSTALL (TARGETS opt_complexrealmultiply DESTINATION bin)

SET(FORWARD_FFT_SRC
forward3dfft.c
)
ADD_EXECUTABLE(opt_forward3dfft ${FORWARD_FFT_SRC})
TARGET_LINK_LIBRARIES(opt_forward3dfft  fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_forward3dfft DESTINATION bin)

SET(FFT_SHIFT_C_SRC
fftshift_c.c
)
ADD_EXECUTABLE(opt_fftshift_c ${FFT_SHIFT_C_SRC})
TARGET_LINK_LIBRARIES(opt_fftshift_c  fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_fftshift_c DESTINATION bin)

SET(FFT_SHIFT_F_SRC
fftshift_f.c
)
ADD_EXECUTABLE(opt_fftshift_f ${FFT_SHIFT_F_SRC})
TARGET_LINK_LIBRARIES(opt_fftshift_f fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_fftshift_f DESTINATION bin)

SET(FREQUENCY1_SRC
frequencySpaceFilterRecon_part1.c
)
ADD_EXECUTABLE(opt_frequencySpaceFilterRecon_projections ${FREQUENCY1_SRC})
TARGET_LINK_LIBRARIES(opt_frequencySpaceFilterRecon_projections fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_frequencySpaceFilterRecon_projections DESTINATION bin)

SET(FREQUENCY2_SRC
frequencySpaceFilterRecon_part2.c
)
ADD_EXECUTABLE(opt_frequencySpaceFilterRecon_fdr ${FREQUENCY2_SRC})
TARGET_LINK_LIBRARIES(opt_frequencySpaceFilterRecon_fdr fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_frequencySpaceFilterRecon_fdr DESTINATION bin)

SET(FREQUENCY3_SRC
frequencySpaceFilterRecon_part3.c
)
ADD_EXECUTABLE(opt_frequencySpaceFilterRecon_complete ${FREQUENCY3_SRC})
TARGET_LINK_LIBRARIES(opt_frequencySpaceFilterRecon_complete fftw3f fftw3 useMINC2 FDR)
#INSTALL(TARGETS opt_frequencySpaceFilterRecon_complete DESTINATION bin)


#SET(TEST_SRC
#test.c
#)
#ADD_EXECUTABLE(testcomplex ${TEST_SRC})
#TARGET_LINK_LIBRARIES(testcomplex fftw3f fftw3 useMINC2 FDR)

CONFIGURE_FILE(${OPT_recon_SOURCE_DIR}/PSF/frequencySpaceFilterRecon.py ${OPT_recon_SOURCE_DIR}/pyScripts/opt_frequencySpaceFilterRecon COPYONLY)
CONFIGURE_FILE(${OPT_recon_SOURCE_DIR}/PSF/frequencySpaceFilterRecon.py ${OPT_recon_SOURCE_DIR}/pyScripts/opt_frequencySpaceFilterRecon.py COPYONLY)

INSTALL(FILES ${OPT_recon_SOURCE_DIR}/pyScripts/opt_frequencySpaceFilterRecon DESTINATION bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ)
INSTALL(FILES ${OPT_recon_SOURCE_DIR}/pyScripts/opt_frequencySpaceFilterRecon.py DESTINATION bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ)

# add all zoom level psf to this installation
#INSTALL(FILES ${OPT_recon_SOURCE_DIR}/PSF/psf12307.mnc DESTINATION share/OPT_recon)

