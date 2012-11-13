/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         useMINC2.c
**      Purpose:      helper functions for minc2 library
**      Programmer:   L.B
**      Date Started: Aug 2011
**
**  This is part of the OPT_RECON program
**  
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License (version 2) as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
******************************************************************************/
#include "useMINC2.h"
int get_minc_data_type(mihandle_t filein, unsigned short *bitstored)
{
int result;
mitype_t volume_data_type;
result = miget_data_type(filein,&volume_data_type);
if (result == MI_ERROR) 
{
 fprintf(stderr, "Error getting datatype %d.\n", result);
 return 1;
}
 // set the data type for the file  
switch (volume_data_type)
{
    case MI_TYPE_BYTE:
    case MI_TYPE_UBYTE:
      *bitstored = 8;
      break;
    case MI_TYPE_SHORT:
    case MI_TYPE_USHORT:
      *bitstored = 16;
      break;
    case MI_TYPE_INT:
    case MI_TYPE_UINT:
    case MI_TYPE_FLOAT:
      *bitstored = 32;
      break;
    default:
      fprintf(stderr,"Bad data type ");
      return;
  } //end of switch
  return 0;
}
int get_minc_spatial_separations_from_handle(mihandle_t filein,
                                             double *stepz, double *stepy, double *stepx)
{
 midimhandle_t dimhandles[NDIMS];
 double  separations[NDIMS];
 int result;
 /* Get the dimension handles */
 result = miget_volume_dimensions (filein,MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL,
                                            MI_DIMORDER_FILE, NDIMS, dimhandles);
 if (result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting dimension handles %d.\n", result);
   return 1;
  }
 /* Get the dimension separations */
 result = miget_dimension_separations(dimhandles,MI_ORDER_FILE,NDIMS,separations);
 if (result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting dimension sizes.\n");
   return 1;
  }
 /* Set the separations as requested */
 *stepz = separations[0];
 *stepy = separations[1];
 *stepx = separations[2];

 return 0;
}
int get_minc_spatial_starts_from_handle(mihandle_t filein,
                                        double *startz, double *starty, 
                                        double *startx)
{
 midimhandle_t dimhandles[NDIMS];
 double  starts[NDIMS];
 int result;
 /* Get the dimension handles */
 result = miget_volume_dimensions (filein,MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL,
                                            MI_DIMORDER_FILE, NDIMS, dimhandles);
 if (result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting dimension handles %d.\n", result);
   return 1;
  }
 /* Get the dimension starts */
 result = miget_dimension_starts(dimhandles,MI_ORDER_FILE,NDIMS,starts);
 if (result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting dimension sizes.\n");
   return 1;
  }
 /* Set the separations as requested */
 *startz = starts[0];
 *starty = starts[1];
 *startx = starts[2];

 return 0;
}
int get_minc_spatial_dimensions_from_handle(mihandle_t filein, 
                                            long *nz, long *ny, long *nx)
{
 midimhandle_t dimhandles[NDIMS];
 unsigned int  dimsizes[NDIMS];
 int result;
 /* Get the dimension handles */
 result = miget_volume_dimensions (filein,MI_DIMCLASS_SPATIAL, MI_DIMATTR_ALL,
                                          MI_DIMORDER_FILE, NDIMS, dimhandles);
 /* Get the dimension sizes */
 result = miget_dimension_sizes (dimhandles,NDIMS,dimsizes);

 if (result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting dimension sizes %d.\n", result);
   return 1;
  }

 *nz = dimsizes[0];
 *ny = dimsizes[1];
 *nx = dimsizes[2];

 return 0;
}


int open_minc_file_and_read(char *filename, mihandle_t *handle)
{
 int result;

 /* Get volume handle from minc file */
 result = miopen_volume(filename,MI2_OPEN_READ, handle);
 if (result == MI_ERROR) 
  {
   fprintf(stderr, "Error opening the input file %s.\n", filename);
   return 1;
  } 
  	
 return 0;
	
}

int create_OPT_group_in_minc_header(mihandle_t handle)
{
 int result;

 result = micreate_group(handle, "/", "OPT");
 if (result == MI_ERROR) 
  {
  fprintf(stderr, "Error create_group failed\n");
  return 1;
  }
 return 0;
}

int add_attribute_to_minc_header(mihandle_t handle, const char *name, float value)
{
 int result;
 result = miset_attr_values(handle, MI_TYPE_FLOAT, "/OPT", name, 128, &value);
  if (result == MI_ERROR) 
  {
  fprintf(stderr, "Error set_attr_values failed\n");
  return 1;
  }
 return 0;
}


/* add dimension order flag */
int open_minc_file_and_write(char *filename, mihandle_t *handle, 
                             long n3, long n2, long n1, 
                             double starts[NDIMS], double separations[NDIMS],int flags)
{
  
 midimhandle_t dimhandles[NDIMS];
 unsigned int  dimsizes[NDIMS];
 int result;
    	
 dimsizes[0] = n3;
 dimsizes[1] = n2;
 dimsizes[2] = n1;

 /* Create the three dimensions */
 result = micreate_dimension("xspace",MI_DIMCLASS_SPATIAL,
                                      MI_DIMATTR_REGULARLY_SAMPLED,
				      dimsizes[2],&dimhandles[2]);
 if(result == MI_ERROR) 
 { 
  fprintf(stderr, "Error creating output dimensions\n"); 
  return 1;
 }  
  
 result = micreate_dimension("yspace",MI_DIMCLASS_SPATIAL,
                                      MI_DIMATTR_REGULARLY_SAMPLED,
		                      dimsizes[1],&dimhandles[1]);
 if(result == MI_ERROR) 
 { 
  fprintf(stderr, "Error creating output dimensions\n"); 
  return 1;
 }  
 result = micreate_dimension("zspace",MI_DIMCLASS_SPATIAL,
                                      MI_DIMATTR_REGULARLY_SAMPLED,
			              dimsizes[0],&dimhandles[0]);
 if(result == MI_ERROR) 
 { 
  fprintf(stderr, "Error creating output dimensions\n"); 
  return 1;
 }
  
 result = miset_dimension_starts(dimhandles,NDIMS,starts);
 result = miset_dimension_separations(dimhandles,NDIMS,separations);

 if(result == MI_ERROR) 
 { 
  fprintf(stderr, "Error setting starts and separations\n"); 
  return 1;
 }  
  	
 /* Create the file on the disk and get volume handle*/
 if(flags == COMPLEX)
    result = micreate_volume(filename, NDIMS, dimhandles,
                            MI_TYPE_FCOMPLEX, MI_CLASS_COMPLEX,
                            NULL,handle);
 else if (flags == FLOAT)
     result = micreate_volume(filename, NDIMS, dimhandles, 
                              MI_TYPE_FLOAT, MI_CLASS_REAL,
                              NULL,handle);
 else if (flags == USHORT)
     result = micreate_volume(filename, NDIMS, dimhandles, 
                              MI_TYPE_USHORT, MI_CLASS_REAL,
                              NULL,handle);
 else
     result = micreate_volume(filename, NDIMS, dimhandles, 
                              MI_TYPE_UBYTE, MI_CLASS_REAL,
                              NULL,handle);
 if(result == MI_ERROR) 
 {
  fprintf(stderr, "Error creating output volume\n");
  return 1;
 }
  

 /* Create the image associated with the volume */
 result = micreate_volume_image(*handle);
 if(result == MI_ERROR)
 {
  fprintf(stderr, "Error creating volume image.\n");
  return 1;
 }

 return 0;
	
}

int get_minc_history(mihandle_t handle, char *history)
{
 int result;
 char value[STRINGLENGTH*2];
 result = miget_attr_values(handle, MI_TYPE_STRING, "", "history", STRINGLENGTH*2, value);
 if (result == MI_ERROR) 
  {
  fprintf(stderr, "Error can not get minc history\n");
  return 1;
  }
 
 strcpy(history,value);
 return 0;
}

int get_minc_attribute(mihandle_t handle, const char *name, 
                       mitype_t data_type, void *value)
{
 int result;
 
 result = miget_attr_values(handle, data_type, "/OPT", name, 1, (void *)value);
 if (result == MI_ERROR) 
  {
  fprintf(stderr, "Error can not get minc attribute\n");
  return 1;
  }

 return 0;
}
