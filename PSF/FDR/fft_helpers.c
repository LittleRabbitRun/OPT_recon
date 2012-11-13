/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         fft_helpers.c (adapted from Jwalls code libfdr2.c)
**      Purpose:      functions for manipulating FFT data
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
#include "fft_helpers.h"

int shift_float_to_complex(long elems, fdr_complex *data) 
{
 float *tmp;
 long i;

 tmp = (float *)data;
 tmp += elems-1;
 // Start at the end of the data and transpose
 for(i=elems-1;i>=0;i--) 
 {
  data[i]= (float complex) *tmp;
  tmp = tmp - 1;
 }

 return 0;
} 
int rfftshift(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1) 
{
  
 fdr_complex tmp;
 long i,j,k, index1, index2, Z2, Y2, X2;
 float *temp2d_q1 = NULL; 
 float *temp2d_q2 = NULL;
 unsigned long start_q1[3], start_q2[3], count[3];
  
 int result;
  
 Z2 = n3/2;
 Y2 = n2/2;
 X2 = n1/2;
 temp2d_q1 = (float *)fftwf_malloc(n2*n1*sizeof(float));
 temp2d_q2 = (float *)fftwf_malloc(n2*n1*sizeof(float));
 fprintf(stdout, "Attempting to perform a shift on REAL FFT\n");

 for(k=0;k<Z2; k++) 
 {
  /* Load the data in 2 slices */
  start_q1[0] = (unsigned long)k; start_q1[1] = start_q1[2] = 0;
  start_q2[0] = (unsigned long)k+Z2; start_q2[1] = start_q2[2] = 0;
  count[0] = 1; count[1] = (unsigned long)n2; count[2] = (unsigned long)n1;
  fprintf(stdout, "swapping two slices  %ld and %ld \n", 
	          start_q1[0], start_q2[0]);   
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start_q1,count, (float *) temp2d_q1);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error loading slice1.\n");
   return 1;
  }
    
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start_q2,count, (float *) temp2d_q2);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error loading slice 2.\n");
   return 1;
  }
    
  /* Data from two slices is now loaded, so do the swap */
  for(j=0;j<Y2;j++) 
  {
   for(i=0;i<X2;i++) 
   {
    // Swap quadrant 1 and 5
    index1 = j*n1 + i;
    index2 = (j+Y2)*n1 + i+X2;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
    // Swap quadrant 2 and 6
    index1 = j*n1 + i+X2;
    index2 = (j+Y2)*n1 + i;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
    // Swap quadrant 3 and 7
    index1 = (j+Y2)*n1 + i;
    index2 = j*n1 + i+X2;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
    // Swap quadrant 4 and 8
    index1 = (j+Y2)*n1 + i+X2;
    index2 = j*n1 + i;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
   } 
  }     
  /* Write out the data */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FLOAT,start_q1,count, (float *) temp2d_q1);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error writing slice 1.\n");
   fftwf_free(temp2d_q1);
   return 1;
  }
    
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FLOAT,start_q2,count, (float *) temp2d_q2);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error writing slice 2.\n");
   fftwf_free(temp2d_q2);
   return 1;
  }
    
 } // end zloop

 fftwf_free(temp2d_q1); fftwf_free(temp2d_q2);
 fprintf(stdout, "COMPLETED shifting REAL FFT\n");
 
 return 0;
}
 
int cfftshift(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1) 
{
 fdr_complex tmp;
 long i,j,k, index1, index2, Z2, Y2, X2;
 fdr_complex *temp2d_q1 = NULL; 
 fdr_complex *temp2d_q2 = NULL;
 unsigned long start_q1[3], start_q2[3], count[3];
  
 int result;
  
 Z2 = n3/2;
 Y2 = n2/2;
 X2 = n1/2;
 temp2d_q1 = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 temp2d_q2 = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 fprintf(stdout, "Attempting to perform a shift on COMPLEX FFT\n");

 for(k=0;k<Z2; k++) 
 {
  /* Load the data in 2 slices */
  start_q1[0] = (unsigned long)k; start_q1[1] = start_q1[2] = 0;
  start_q2[0] = (unsigned long)k+Z2; start_q2[1] = start_q2[2] = 0;
  count[0] = 1; count[1] = (unsigned long)n2; count[2] = (unsigned long)n1;
    
  fprintf(stdout, "swapping two slices  %ld and %ld \n", 
	          start_q1[0], start_q2[0]);
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FCOMPLEX,start_q1,count, (fdr_complex *) temp2d_q1);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error loading slice1.\n");
   return 1;
  }
    
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FCOMPLEX,start_q2,count, (fdr_complex *) temp2d_q2);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error loading slice 2.\n");
   return 1;
  }
    
  /* Data from two slices is now loaded, so do the swap */
  for(j=0;j<Y2;j++) 
  {
   for(i=0;i<X2;i++) 
   {
    // Swap quadrant 1 and 5
    index1 = j*n1 + i;
    index2 = (j+Y2)*n1 + i+X2;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
    // Swap quadrant 2 and 6
    index1 = j*n1 + i+X2;
    index2 = (j+Y2)*n1 + i;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
    // Swap quadrant 3 and 7
    index1 = (j+Y2)*n1 + i;
    index2 = j*n1 + i+X2;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
    // Swap quadrant 4 and 8
    index1 = (j+Y2)*n1 + i+X2;
    index2 = j*n1 + i;
    tmp = temp2d_q1[index1];
    temp2d_q1[index1] = temp2d_q2[index2];
    temp2d_q2[index2] = tmp;
   } // end xloop
  } // end yloop
    
  /* Write out the data */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start_q1,count, (mifcomplex_t *) temp2d_q1);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error writing slice 1.\n");
   fftwf_free(temp2d_q1); 
   return 1;
  }
    
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start_q2,count, (mifcomplex_t *) temp2d_q2);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error writing slice 2.\n");
   fftwf_free(temp2d_q2);
   return 1;
  }
    
 } // end zloop

 fftwf_free(temp2d_q1); fftwf_free(temp2d_q2);
 fprintf(stdout, "COMPLETED shift on COMPLEX FFT\n");
 
 return 0;
}

int threedfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1)
{
 long i,j,k, slice;
 fdr_complex *temp3d = NULL;
 fdr_complex *datain2d = NULL;
 fdr_complex *dataout2d = NULL;
 fdr_complex *datain1d = NULL;
 fdr_complex *dataout1d = NULL;
 fftwf_plan plan2d, plan1d;
 unsigned long start[3], count[3];
 int num_slices = NUMSLICES;
 float scale;
 int result;

 temp3d = (fdr_complex *)fftwf_malloc(num_slices*n2*n1*sizeof(fdr_complex));	
 datain2d  = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 datain1d  = (fdr_complex *)fftwf_malloc(n3*sizeof(fdr_complex));
 dataout2d = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 dataout1d = (fdr_complex *)fftwf_malloc(n3*sizeof(fdr_complex));
 
 plan2d = fftwf_plan_dft_2d(n2,n1, datain2d, dataout2d, FFTW_FORWARD, FFTW_MEASURE);  
	
/* 
 * First we do a series of 2D FFTs along the z-direction, and store the
 * values in fileout
 */
 fprintf(stdout, "Attempting to perform a series of 2D FFT along the z direction\n");
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
  /*
   * Assume that the filein is real, load it in, then shift to complex
   */
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start,count, (float *) temp3d);
  if(result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting data.\n");
   return 1;
  }
      
  /* Convert to complex */
  shift_float_to_complex(num_slices*n2*n1, temp3d);
       
  for(slice=0;slice<num_slices;slice++)
  {
   for(j=0;j<n2;j++)
   {
    for(i=0;i<n1;i++)
    {
     datain2d[j*n1+i] = (float complex)temp3d[slice*n2*n1+j*n1+i];
    }
   }
   /* Perform the 2D FFT */
   fftwf_execute(plan2d);
   for(j=0;j<n2;j++)
   {
    for(i=0;i<n1;i++)
    {
     temp3d[slice*n2*n1+j*n1+i] = (float complex)dataout2d[j*n1+i];
    }
   }
  }
  /* Write that out to the file */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)temp3d);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error in setting values.\n");
   return 1;
  }
  		
  }
	
  fftwf_destroy_plan(plan2d);   
  fftwf_free(temp3d); 
  fftwf_free(datain2d); 
  fftwf_free(dataout2d);

  plan1d = fftwf_plan_dft_1d(n3, datain1d, dataout1d, FFTW_FORWARD, FFTW_PATIENT);
  num_slices = NUMSLICES;
  /*
   * Reinitialize the temp3d buffer with different # elements
   */
  temp3d = (fdr_complex *)fftwf_malloc(num_slices*n3*n1*sizeof(fdr_complex));
  scale = 1.0/sqrt(1.0*n3*n2*n1);
  
  for(j=0;j<n2;j+=num_slices)
  {
   if(j+num_slices > n2)
   {
    num_slices = n2 - j;
   }

   /*
    * Load up the 2D slices here in order to avoid loading up nx number of
    * 1D slices below
    */
   start[0] = 0;
   start[1] = (unsigned long)j;
   start[2] = 0;
   count[0] = (unsigned long)n3;
   count[1] = num_slices;
   count[2] = (unsigned long)n1;
   result = miget_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(fdr_complex *) temp3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error opening 1D values.\n");
    return 1;
   }
  		
   for(slice=0;slice<num_slices;slice++)
   {
    for(i=0;i<n1;i++)
    {
     /*
      * Now initialize the 1d array
      */
      for(k=0;k<n3;k++)
      {
       datain1d[k] = temp3d[k*num_slices*n1+slice*n1+i];
      }
               
      fftwf_execute(plan1d);
                
      for(k=0;k<n3;k++)
      {
       temp3d[k*num_slices*n1+slice*n1+i] = dataout1d[k] * scale;
      }
     }
  			
   }
   result = miset_voxel_value_hyperslab(outputfile, MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *) temp3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error writing 2D collection of 1D values.\n");
    return 1;
   }
    }
	
 fftwf_destroy_plan(plan1d);
 fftwf_free(temp3d); 
 fftwf_free(datain1d); 
 fftwf_free(dataout1d);
 fprintf(stdout, "COMPLETED series of 2D FFT along the z direction!!!\n");
			
 return 0;
	
}

int threedifft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1)
{
 long i,j,k, slice;
 fdr_complex *temp3d = NULL;
 fdr_complex *datain2d = NULL;
 fdr_complex *dataout2d = NULL;
 fdr_complex *datain1d = NULL;
 fdr_complex *dataout1d = NULL;
 fftwf_plan plan2d, plan1d;
 unsigned long start[3], count[3];
 float scale;
 int result;
 int num_slices = NUMSLICES;	
 
 temp3d = (fdr_complex *)fftwf_malloc(num_slices*n2*n1*sizeof(fdr_complex));	
 datain2d  = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 dataout2d = (fdr_complex *)fftwf_malloc(n2*n1*sizeof(fdr_complex));
 datain1d  = (fdr_complex *)fftwf_malloc(n3*sizeof(fdr_complex));
 dataout1d = (fdr_complex *)fftwf_malloc(n3*sizeof(fdr_complex));
	
 plan2d = fftwf_plan_dft_2d(n2,n1, datain2d, dataout2d, FFTW_BACKWARD, FFTW_MEASURE);
 plan1d = fftwf_plan_dft_1d(n3, datain1d, dataout1d, FFTW_BACKWARD, FFTW_MEASURE);
	
  /* 
  * First we do a series of 2D INVERSE FFTs along the z-direction, and store the
  * values in fileout
  */
 fprintf(stdout, "Attempting to perform a series of 2D INVERSE FFT along the z direction\n");

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
  /*
   * Assume that the inputfile is real, load it in, then shift to complex
   */
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FCOMPLEX,start,count, (fdr_complex *) temp3d);
  if(result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting data.\n");
   return 1;
  }
  
  for(slice=0;slice<num_slices;slice++)
  {
   for(j=0;j<n2;j++)
   {      
    for(i=0;i<n1;i++)
    {
     datain2d[j*n1+i] = (fdr_complex) temp3d[slice*n2*n1+j*n1+i];
    }
   } // end setting 2d array in
      
    /* Perform the 2D FFT */
   fftwf_execute(plan2d);
         
   for(j=0;j<n2;j++)
   {      
    for(i=0;i<n1;i++)
     {
      temp3d[slice*n2*n1+j*n1+i] = (fdr_complex) dataout2d[j*n1+i];
     }
    } // end setting 2d array out
  } // end iterating over slices
      
  /* Write that out to the file */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)temp3d);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error in setting values.\n");
   return 1;
  }
   } // end iterating over z
  
 fftwf_destroy_plan(plan2d);
 fftwf_free(datain2d); 
 fftwf_free(dataout2d); 
 fftwf_free(temp3d);

 num_slices = NUMSLICES;
 /* 
  * Reinitialize temp3d with different dimensions
  */
 temp3d = (fdr_complex *)fftwf_malloc(num_slices*n3*n1*sizeof(fdr_complex));  
 scale = 1.0/sqrt(1.0*n3*n2*n1);
  
 for(j=0;j<n2;j+=num_slices)
 {
  if(j+num_slices > n2)
  {
   num_slices = n2 - j;
  }

  /*
   * Load up the 2D slices here in order to avoid loading up nx number of
   * 1D slices below
   */
  start[0] = 0;
  start[1] = (unsigned long)j;
  start[2] = 0;
  count[0] = (unsigned long)n3;
  count[1] = num_slices;
  count[2] = (unsigned long)n1;
  result = miget_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(fdr_complex *) temp3d);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error opening 1D values.\n");
   return 1;
  }
    
  for(slice=0;slice<num_slices;slice++)
  {
      
   for(i=0;i<n1;i++)
   {
    /*
     * Set up the data for the transform from the already-read in 2D slice
     */
     for(k=0;k<n3;k++)
     {
      datain1d[k] = temp3d[k*num_slices*n1+slice*n1+i];
     }			
     fftwf_execute(plan1d);
  
    /*
     * Need to perform intensity scaling and set back into 3d buffer
     */
     for(k=0;k<n3;k++)
     {
      temp3d[k*num_slices*n1+slice*n1+i] = dataout1d[k] * scale;
     }
    }   
   }     
   result = miset_voxel_value_hyperslab(outputfile, MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *) temp3d);
   if(result == MI_ERROR)
   {
    fprintf(stderr, "Error writing 3D collection of 1D values.\n");
    return 1;
   }
   }
	
 fftwf_destroy_plan(plan1d);
 fftwf_free(datain1d); 
 fftwf_free(dataout1d); 
 fftwf_free(temp3d);
 fprintf(stdout, "COMPLETED series of 2D INVERSE FFT along the z direction!!!\n");
			
 return 0;
	
}


int series_1dfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1)
{
 long i,j,k, slice;
 fdr_complex *datain = NULL;
 fdr_complex *dataout = NULL;
 fftwf_plan plan1d;
 unsigned long start[3], count[3];
 float scale;
  
 int result;
    
 datain = (fdr_complex *)fftwf_malloc(n1*sizeof(fdr_complex));
 dataout = (fdr_complex *)fftwf_malloc(n1*sizeof(fdr_complex));
  
 plan1d = fftwf_plan_dft_1d(n1, datain, dataout, FFTW_FORWARD, FFTW_MEASURE);
  
 for(k=0;k<n3;k++)
 {
  start[0] = (unsigned long)k; start[1] = start[2] = 0;
  count[0] = (unsigned long)1;
  count[1] = (unsigned long)n2;
  count[2] = (unsigned long)n1;
  result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start,count, (float *) datain);
  if(result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting data.\n");
   return 1;
  }
     
  shift_float_to_complex(n1, datain);
      
  fftwf_execute(plan1d);
      
  /* Now scale the data since FFTW does not*/
  scale = 1.0/sqrt(1.0*n1);
  for(i=0;i<n1;i++)
  {
   dataout[i] *= scale;
  } 
  /* Write that out to the file */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)dataout);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error in setting values.\n");
   return 1;
  }
 }
  
 fftwf_destroy_plan(plan1d);   
 fftwf_free(datain); 
 fftwf_free(dataout); 
  
 /* Now in the other direction */
 datain = (fdr_complex *)fftwf_malloc(n3*sizeof(fdr_complex));
 dataout = (fdr_complex *)fftwf_malloc(n3*sizeof(fdr_complex));
  
 plan1d = fftwf_plan_dft_1d(n3, datain, dataout, FFTW_FORWARD, FFTW_MEASURE);
  
 for(i=0;i<n1;i++)
 {
  start[0] = 0; start[1] = 0; start[2] = (unsigned long)i;
  count[0] = (unsigned long)n3;
  count[1] = (unsigned long)n2;
  count[2] = (unsigned long)1;
  result = miget_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count, (fdr_complex *) datain);
  if(result == MI_ERROR) 
  {
   fprintf(stderr, "Error getting data.\n");
   return 1;
  }
   fftwf_execute(plan1d);
  /* Now scale the data since FFTW does not*/
  scale = 1.0/sqrt(1.0*n3);
  for(k=0;k<n3;k++)
  {
   dataout[k] *= scale;
  } 
  /* Write that out to the file */
  result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)dataout);
  if(result == MI_ERROR)
  {
   fprintf(stderr, "Error in setting values.\n");
   return 1;
  }
 }
 fftwf_destroy_plan(plan1d);   
 fftwf_free(datain); 
 fftwf_free(dataout); 
   
 return 0;
}

int full_2dfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1)
{
 long i;
 fdr_complex *datain = NULL;
 fdr_complex *dataout = NULL;
 fftwf_plan plan2d;
 unsigned long start[3], count[3];
 float scale;
  
 int result;
   
 datain = (fdr_complex *)fftwf_malloc(n3*n1*sizeof(fdr_complex));
 dataout = (fdr_complex *)fftwf_malloc(n3*n1*sizeof(fdr_complex));
  
 plan2d = fftwf_plan_dft_2d(n3,n1, datain, dataout, FFTW_FORWARD, FFTW_MEASURE);
  
 start[0] = (unsigned long)0; start[1] = start[2] = 0;
 count[0] = (unsigned long)n3;
 count[1] = (unsigned long)n2;
 count[2] = (unsigned long)n1;
 result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start,count, (float *) datain);
 if(result == MI_ERROR) 
 {
  fprintf(stderr, "Error getting data.\n");
  return 1;
 }
  
 shift_float_to_complex(n3*n2*n1, datain);
   
 fftwf_execute(plan2d);
 /* Now scale the data since FFTW does not*/
 scale = 1.0/sqrt(1.0*n3*n2*n1);
 for(i=0;i<n3*n2*n1;i++)
 {
  dataout[i] *= scale;
 } 
 /* Write that out to the file */
 result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)dataout);
 if(result == MI_ERROR)
 {
  fprintf(stderr, "Error in setting values.\n");
  return 1;
 }
  
 fftwf_destroy_plan(plan2d);   
 fftwf_free(datain); 
 fftwf_free(dataout); 
   
 return 0;
}

int full_2difft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1)
{
 long i;
 fdr_complex *datain = NULL;
 fdr_complex *dataout = NULL;
 fftwf_plan plan2d;
 unsigned long start[3], count[3],nelems;
 float scale;
  
 int result;
  
 datain = (fdr_complex *)fftwf_malloc(n3*n1*sizeof(fdr_complex));
 dataout = (fdr_complex *)fftwf_malloc(n3*n1*sizeof(fdr_complex));
  
 plan2d = fftwf_plan_dft_2d(n3,n1, datain, dataout, FFTW_BACKWARD, FFTW_MEASURE);
  
 start[0] = (unsigned long)0; start[1] = start[2] = 0;
 count[0] = (unsigned long)n3;
 count[1] = (unsigned long)n2;
 count[2] = (unsigned long)n1;
 result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FCOMPLEX,start,count, (fdr_complex *) datain);
 if(result == MI_ERROR) 
 {
  fprintf(stderr, "Error getting data.\n");
  return 1;
 }
 fftwf_execute(plan2d);
   
 /* Now scale the data since FFTW does not*/
 scale = 1.0/sqrt(1.0*n3*n2*n1);
 for(i=0;i<n3*n2*n1;i++)
 {
  dataout[i] *= scale;
 } 
 /* Write that out to the file */
 result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)dataout);
 if(result == MI_ERROR)
 {
  fprintf(stderr, "Error in setting values.\n");
  return 1;
 }
  
 fftwf_destroy_plan(plan2d);
 fftwf_free(datain); 
 fftwf_free(dataout); 
  
    
 return 0;
}

int full_3dfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1)
{
 long i;
 fdr_complex *datain = NULL;
 fdr_complex *dataout = NULL;
 fftwf_plan plan3d;
 unsigned long start[3], count[3];
 float scale;
  
 int result;
  
 datain = (fdr_complex *)fftwf_malloc(n3*n2*n1*sizeof(fdr_complex));
 dataout = (fdr_complex *)fftwf_malloc(n3*n2*n1*sizeof(fdr_complex));
  
 plan3d = fftwf_plan_dft_3d(n3,n2,n1, datain, dataout, FFTW_FORWARD, FFTW_MEASURE);
  
 start[0] = (unsigned long)0; start[1] = start[2] = 0;
 count[0] = (unsigned long)n3;
 count[1] = (unsigned long)n2;
 count[2] = (unsigned long)n1;
 result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FLOAT,start,count, (float *) datain);
 if(result == MI_ERROR) 
 {
  fprintf(stderr, "Error getting data.\n");
  return 1;
 }
   
 shift_float_to_complex(n3*n2*n1, datain);
   
 fftwf_execute(plan3d);
   
 /* Now scale the data since FFTW does not*/
 scale = 1.0/sqrt(1.0*n3*n2*n1);
 for(i=0;i<n3*n2*n1;i++)
 {
  dataout[i] *= scale;
 }
 /* Write that out to the file */
 result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)dataout);
 if(result == MI_ERROR)
 {
  fprintf(stderr, "Error in setting values.\n");
  return 1;
 }
  
 fftwf_destroy_plan(plan3d);   
 fftwf_free(datain); 
 fftwf_free(dataout); 
  
     
 return 0;
}

int full_3difft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1)
{
 long i;
 fdr_complex *datain = NULL;
 fdr_complex *dataout = NULL;
 fftwf_plan plan3d;
 unsigned long start[3], count[3];
 float scale;
  
 int result;
  
 datain = (fdr_complex *)fftwf_malloc(n3*n2*n1*sizeof(fdr_complex));
 dataout = (fdr_complex *)fftwf_malloc(n3*n2*n1*sizeof(fdr_complex));
  
 plan3d = fftwf_plan_dft_3d(n3,n2,n1, datain, dataout, FFTW_BACKWARD, FFTW_MEASURE);
  
 start[0] = (unsigned long)0; start[1] = start[2] = 0;
 count[0] = (unsigned long)n3;
 count[1] = (unsigned long)n2;
 count[2] = (unsigned long)n1;

 result = miget_voxel_value_hyperslab(inputfile,MI_TYPE_FCOMPLEX,start,count, (fdr_complex *) datain);
 if(result == MI_ERROR) 
 {
  fprintf(stderr, "Error getting data.\n");
  return 1;
 }
   
 fftwf_execute(plan3d);
 
  
 /* Now scale the data since FFTW does not*/
 scale = 1.0/sqrt(1.0*n3*n2*n1);
 for(i=0;i<n3*n2*n1;i++)
 {
  dataout[i] *= scale;
 } 
 /* Write that out to the file */
 result = miset_voxel_value_hyperslab(outputfile,MI_TYPE_FCOMPLEX,start,count,(mifcomplex_t *)dataout);
 if(result == MI_ERROR)
 {
  fprintf(stderr, "Error in setting values.\n");
  return 1;
 }

 fftwf_destroy_plan(plan3d);   
 fftwf_free(datain); 
 fftwf_free(dataout); 
  
 return 0;
}

