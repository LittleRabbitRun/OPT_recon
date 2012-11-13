if(USE_MINC2)
# netcdf
  FIND_PATH (NETCDF_INCLUDE_PATH netcdf.h
             /usr/include 
             /usr/local/include)

  INCLUDE_DIRECTORIES(${NETCDF_INCLUDE_PATH})
  
  FIND_LIBRARY(NETCDF_LIBRARY netcdf
               /usr/lib
               /usr/local/lib)
  
  # hdf5
  FIND_PATH (HDF5_INCLUDE_PATH hdf5.h 
             /usr/include
             /usr/local/include)

  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_PATH})
  
  FIND_LIBRARY(HDF5_LIBRARY hdf5 
               /usr/lib 
               /usr/local/lib)
  # minc

  FIND_LIBRARY(MINC2_LIBRARY minc2
               /usr/lib 
               /usr/local/lib)
  FIND_PATH (MINC2_INCLUDE_PATH minc2.h
             /usr/include
             /usr/local/include)

  INCLUDE_DIRECTORIES(${MINC2_INCLUDE_PATH})

if(MINC2_INCLUDE_PATH AND HDF5_INCLUDE_PATH AND NETCDF_INCLUDE_PATH)
  if(MINC2_LIBRARY AND HDF5_LIBRARY AND NETCDF_LIBRARY)
    SET(MINC2_FOUND 1)
  endif(MINC2_LIBRARY AND HDF5_LIBRARY AND NETCDF_LIBRARY)
endif(MINC2_INCLUDE_PATH AND HDF5_INCLUDE_PATH AND NETCDF_INCLUDE_PATH )

endif(USE_MINC2)
