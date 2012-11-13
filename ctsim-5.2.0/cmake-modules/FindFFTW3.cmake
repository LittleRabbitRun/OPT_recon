# - Try to find FFTW
# Once done this will define
#  FFTW_FOUND - System has FFTW3
#  FFTW_INCLUDE_DIRS - The FFTW3 include directories
#  FFTW_LIBRARIES - The libraries needed to use FFTW3
#  FFTW_DEFINITIONS - Compiler switches required for using FFTW3

find_package(PkgConfig)

pkg_check_modules(PC_FFTW3 QUIET fftw3)

set(FFTW3F_DEFINITIONS ${PC_FFTW3_CFLAGS_OTHER})

find_path(FFTW3_INCLUDE_DIR fftw3.h
          HINTS ${PC_FFTW3_INCLUDEDIR} ${PC_FFTW3_INCLUDE_DIRS}
          NAMES fftw3.h )

find_library(FFTW3_LIBRARY NAMES fftw3 libfftw3
             HINTS ${PC_FFTW3F_LIBDIR} ${PC_FFTW3F_LIBRARY_DIRS} )

# find_library(FFTW3_MALLOC_LIBRARY NAMES fftw_malloc libfftw_malloc
#              HINTS ${PC_FFTW3F_LIBDIR} ${PC_FFTW3F_LIBRARY_DIRS} )

set(FFTW3_LIBRARIES ${FFTW3_LIBRARY} )
set(FFTW3_INCLUDE_DIRS ${FFTW3_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set FFTW3_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FFTW3  DEFAULT_MSG
                                  FFTW3_LIBRARY FFTW3_INCLUDE_DIR)

mark_as_advanced(FFTW3_INCLUDE_DIR FFTW3_LIBRARY )
