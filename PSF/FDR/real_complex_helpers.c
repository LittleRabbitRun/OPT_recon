/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         real_complex_helpers.c (adapted from Jwalls code libfdr2.c)
**      Purpose:      functions for manipulating real and complex data
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
#include "real_complex_helpers.h"
int multiply_real_by_real(mihandle_t inputfile1, mihandle_t inputfile2, mihandle_t outputfile, 
                             long n3, long n2, long n1)
{
 long k, slice, i;
 unsigned long start[3], count[3];
  
 float *data1_3d = NULL;
 float *data2_3d = NULL;
 float *data3_3d = NULL;
  
 float x, y;

 int result;
 int num_slices = NUMSLICES; 
   
 data1_3d = (float *)malloc(num_slices*n2*n1*sizeof(float));
 data2_3d = (float *)malloc(num_slices*n2*n1*sizeof(float));
 data3_3d = (float *)malloc(num_slices*n2*n1*sizeof(float));
 fprintf(stdout, "Attempting to perform real by real multiplication!!!\n");
  
 for(k=0;k<n3;k+=num_slices)
  {   
   if(k+num_slices > n3)
   {
   num_slices = n3 - k;
   }
   /* 
    * Retrieve the data from the file 
    */
   start[0] = (unsigned long)k; start[1] = start[2] = 0;
   count[0] = num_slices;
   count[1] = (unsigned long)n2;
   count[2] = (unsigned long)n1;
    
   fprintf(stdout, "processing slice %ld \n", start[0]);
   result = miget_voxel_value_hyperslab(inputfile1,MI_TYPE_FLOAT,start,count, (float *) data1_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error loading 2D data file1.\n");
    return 1;
   }
    
   result = miget_voxel_value_hyperslab(inputfile2,MI_TYPE_FLOAT,start,count, (float *) data2_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error loading 2D data file2.\n");
    return 1;
   }
        
   for(i=0;i<n1*n2*num_slices;i++)
   {
    data3_3d[i] = ( (float)data1_3d[i] * (float)data2_3d[i] );
   }
    
   /* Write that out to the file */
   result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FLOAT,start,count,(float *)data3_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error in setting values.\n");
    return 1;
   }
  }
  free(data1_3d); 
  free(data2_3d); 
  free(data3_3d);
  
 return 0;
}



int multiply_complex_by_real(mihandle_t inputfile1, mihandle_t inputfile2, mihandle_t outputfile, 
                             long n3, long n2, long n1)
{
 long k, slice, i;
 unsigned long start[3], count[3];
  
 fdr_complex *data1_3d = NULL;
 float *data2_3d = NULL;
 fdr_complex *data3_3d = NULL;
  
 float x, y;

 int result;
 int num_slices = NUMSLICES; 
   
 data1_3d = (fdr_complex *)fftwf_malloc(num_slices*n2*n1*sizeof(fdr_complex));
 data2_3d = (float *)fftwf_malloc(num_slices*n2*n1*sizeof(float));
 data3_3d = (fdr_complex *)fftwf_malloc(num_slices*n2*n1*sizeof(fdr_complex));
 fprintf(stdout, "Attempting to perform complex by real multiplication!!!\n");
  
 for(k=0;k<n3;k+=num_slices)
  {   
   if(k+num_slices > n3)
   {
   num_slices = n3 - k;
   }
   /* 
    * Retrieve the data from the file 
    */
   start[0] = (unsigned long)k; start[1] = start[2] = 0;
   count[0] = num_slices;
   count[1] = (unsigned long)n2;
   count[2] = (unsigned long)n1;
    
   fprintf(stdout, "processing slice %ld \n", start[0]);
   result = miget_voxel_value_hyperslab(inputfile1,MI_TYPE_FCOMPLEX,start,count, (fdr_complex *) data1_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error loading 2D data file1.\n");
    return 1;
   }
    
   result = miget_voxel_value_hyperslab(inputfile2,MI_TYPE_FLOAT,start,count, (float *) data2_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error loading 2D data file2.\n");
    return 1;
   }
        
   for(i=0;i<n1*n2*num_slices;i++)
   {
    data3_3d[i] = ( (float)creal(data1_3d[i]) * (float)data2_3d[i] ) + 
                  ( (float)cimag(data1_3d[i]) * (float)data2_3d[i] ) * I;
   }
    
   /* Write that out to the file */
   result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)data3_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error in setting values.\n");
    return 1;
   }
  }
 fftwf_free(data1_3d); 
 fftwf_free(data2_3d); 
 fftwf_free(data3_3d);
  
 return 0;
}

int multiply_complex_by_complex(mihandle_t inputfile1, mihandle_t inputfile2, mihandle_t outputfile, 
                                long n3, long n2, long n1)
{
  
 long k, slice, i;
 unsigned long start[3], count[3];
  
 fdr_complex *data1_3d = NULL;
 fdr_complex *data2_3d = NULL;
 fdr_complex *data3_3d = NULL;
  
 int result;
 
 int num_slices = NUMSLICES;
 
 data1_3d = (fdr_complex *)fftwf_malloc(num_slices*n2*n1*sizeof(fdr_complex));
 data2_3d = (fdr_complex *)fftwf_malloc(num_slices*n2*n1*sizeof(fdr_complex));
 data3_3d = (fdr_complex *)fftwf_malloc(num_slices*n2*n1*sizeof(fdr_complex));
 fprintf(stdout, "Attempting to perform complex by complex multiplication!!!\n");
  
 for(k=0;k<n3;k+=num_slices)
 {   
 if(k+num_slices > n3)
  {
   num_slices = n3 - k;
  }

  /* 
   * Retrieve the data from the file 
   */
   start[0] = (unsigned long)k; start[1] = start[2] = 0;
   count[0] = num_slices;
   count[1] = (unsigned long)n2;
   count[2] = (unsigned long)n1;
    
   fprintf(stdout, "processing slice %ld \n", start[0]);

   result = miget_voxel_value_hyperslab(inputfile1,MI_TYPE_FCOMPLEX,start,count, (fdr_complex *) data1_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error loading 2D data file1.\n");
    return 1;
   }
    
   result = miget_voxel_value_hyperslab(inputfile2,MI_TYPE_FCOMPLEX,start,count, (fdr_complex *) data2_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error loading 2D data file2.\n");
    return 1;
   }
        
   for(i=0;i<n1*n2*num_slices;i++)
   {
    //data3_3d[i] = (fdr_complex)(data1_3d[i] * data2_3d[i]);
    data3_3d[i] = (fdr_complex) ((creal(data1_3d[i])*creal(data2_3d[i])- cimag(data1_3d[i])*cimag(data2_3d[i])) + 
                                 (creal(data1_3d[i])*cimag(data2_3d[i])+ cimag(data1_3d[i])*creal(data2_3d[i])) * I);
   }
    
   /* Write that out to the file */
   result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)data3_3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error in setting values.\n");
    return 1;
   }
 }
 fftwf_free(data1_3d); 
 fftwf_free(data2_3d); 
 fftwf_free(data3_3d);
  
 return 0;
}

int complex_magnitude(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1) 
{
 unsigned long start[3], count[3];
 long k,i;
 fdr_complex tmpv;
 fdr_complex *tmpc;
 float *tmpf;
 float tmp;
 fdr_complex *data;
 double dmin = 1e10;
 double dmax = 0;
 int result;
	  	
 data = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
	
 for(k=0;k<n3;k++)
 {
  start[0] = (unsigned long)k; 
  start[1] = start[2] = 0;
  count[0] = 1;
  count[1] = (unsigned long)n2;
  count[2] = (unsigned long)n1;
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FCOMPLEX,start,count,(fdr_complex *) data);
  if(result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting data.\n");
   return 1;
  }
  /* 
   * We're going to do the magnitude in-place
   */
  tmpc = (fdr_complex *)data;
  tmpf = (float *)data;
  	
  for(i=0;i<n2*n1;i++) 
  {
   tmpv = tmpc[i];
   tmp  = sqrt((creal(tmpv)*creal(tmpv) + cimag(tmpv)*cimag(tmpv)));
   //if(isinf(tmp))
    //tmp=0;
   if(tmp < dmin)
    dmin = tmp;
   if(tmp > dmax)
    dmax = tmp;
   tmpf[i] = tmp; 
  }		
   /* Write that out to the file */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FLOAT,start,count,(float *)data);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error in setting values.\n");
   fftwf_free(data);
   return 1;
  }
  }
  fftwf_free(data);
	
  /* Set the range on the file */
  result = miset_volume_valid_range(outputfile, dmax, dmin);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Could not set volume valid range\n");
   return 1;
  }
  result = miset_volume_range(outputfile, dmax, dmin);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Could not set volume range\n");
   return 1;
  }	
  printf("magnitude of complex image COMPLETE!\n");
			
  return 0;

}

