/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         psfstack_helpers.c (adapted from Jwalls code libfdr2.c)
**      Purpose:      functions for manipulating PSF stack data
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
#include "psfstack_helpers.h"

int psfstack_fft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1) 
{
 long index1, index2;
 long i,j,k;
 long dim1,dim2;
 fdr_complex *datain2d = NULL;
 fdr_complex *dataout2d = NULL;

 fdr_complex tmp;
 fftwf_plan plan2d; 
 unsigned long start[3], count[3];
 float scale;
 int result;
  
 datain2d  = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 dataout2d = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 
 plan2d = fftwf_plan_dft_2d(n2,n1, (fdr_complex *)datain2d, (fdr_complex *)dataout2d, FFTW_FORWARD, FFTW_MEASURE);
  
 /* 
  * First we do a series of 2D FFTs along the z-direction, and store the
  * values in fileout
  */
 fprintf(stdout, "Attempting to perform a series of 2D FFT on the PSF file\n");
 printf(" %d %d %d \n", n3,n2,n1);
 for(k=0;k<n3;k++)
 {
  start[0] = (unsigned long)k; start[1] = start[2] = 0;
  count[0] = 1;
  count[1] = (unsigned long)n2;
  count[2] = (unsigned long)n1;
      

  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start,count, (float *) datain2d);
  if(result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting data.\n");
   return 1;
  }
      
  /* Convert to complex */
  shift_float_to_complex(n2*n1, (float *)datain2d);
       
  scale = 1.0/sqrt(1.0*n2*n1);
      
  fftwf_execute(plan2d);
          
 if(n2%2)
  dim2=(n2-1)/2;
 else
  dim2=n2/2;
 if(n1%2)
  dim1=(n1-1)/2;
 else
  dim1=n1/2;

 for(j=0;j<dim2;j++)
  {
   for(i=0;i<dim1;i++)
   {

    // swap quadrant 1 and 3
    index1 = j*n1 + i;
    index2 = (j+dim2)*n1 + i+dim1;
    
    tmp = dataout2d[index1];
    dataout2d[index1] = dataout2d[index2] * scale;
    dataout2d[index2] = tmp * scale;

    // swap quadrant 2 and 4
    index1 = j*n1 + (i+dim1);
    index2 = (j+dim2)*n1 + i;
    
    tmp = dataout2d[index1];
    dataout2d[index1] = dataout2d[index2] * scale;
    dataout2d[index2] = tmp * scale;

   }
  }
  
  /* Write that out to the file */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)dataout2d);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error in setting values.\n");
   return 1;
  }
        
  }
  
  fftwf_destroy_plan(plan2d);   
  fftwf_free(datain2d); 
  fftwf_free(dataout2d); 
  fprintf(stdout, "COMPLETED 2D FFT of PSF file!!!\n");

  return 0;
}

int psfstack_normalize(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1) 
{
 long i, j, k;
 unsigned long start[3], count[3];
 long index;
 float tempsum;
 int result;
 float *data;
 float dmax, dmin;

 dmax = -1e100;
 dmin = 1e100;
 
 data = (float *) fftwf_malloc(n2*n1*sizeof(float));
 printf(" %d %d %d \n", n3,n2,n1);
 /* Iterate over depth point */
 for(k=0;k<n3;k++) 
 {
  start[0] = (unsigned long)k;
  start[1] = start[2] = 0;
  count[0] = 1;
  count[1] = (unsigned long) n2; count[2] = (unsigned long)n1;
    
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start,count, (float *) data);
  if(result == MI_ERROR)
  {
   fftwf_free(data);
   fprintf(stdout, "Could not read data at z=%ld\n", k);
   return 1;
  }
  
  tempsum = 0;
  for(j=0;j<n2;j++) 
  {
   for(i=0;i<n1;i++) 
   {
    index = j*n1 + i;
    tempsum += data[index];
   }
  }
  
  for(j=0;j<n2;j++) 
  {
   for(i=0;i<n1;i++) 
   {
    index = j*n1 + i;
    data[index] /= tempsum;
    if(data[index] < dmin)
     dmin = data[index];
    if(data[index] > dmax)
     dmax = data[index];
   }
  }
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FLOAT,start,count, (float *) data);
  if(result == MI_ERROR)
  {
   fftwf_free(data);
   fprintf(stdout, "Could not read data at z=%ld\n", k);
   return 1;
  }
   }

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

 fftwf_free(data);

 return 0;
}

