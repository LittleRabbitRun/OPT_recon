# add the install targets
CONFIGURE_FILE(${OPT_recon_SOURCE_DIR}/CCD/apply_Coefficients_mp/opt_applyccdcoeffs.py ${OPT_recon_SOURCE_DIR}/CCD/apply_Coefficients_mp/opt_applyccdcoeffs COPYONLY)

INSTALL(FILES apply_Coefficients_mp/opt_applyccdcoeffs.py DESTINATION bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ)
INSTALL(FILES apply_Coefficients_mp/opt_applyccdcoeffs    DESTINATION bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE OWNER_READ GROUP_READ WORLD_READ)

INSTALL(FILES apply_Coefficients_mp/coeffa.mnc DESTINATION share/OPT_recon)

INSTALL(FILES apply_Coefficients_mp/coeffb.mnc DESTINATION share/OPT_recon)

INSTALL(FILES apply_Coefficients_mp/coeffc.mnc DESTINATION share/OPT_recon)





