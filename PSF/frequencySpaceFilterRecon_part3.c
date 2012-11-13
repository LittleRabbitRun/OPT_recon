/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         frequencySpaceFilterRecon_part3.c
**      Purpose:      frequency space filter reconstruction projections
**      Programmer:   L.B
**      Date Started: Jul 2012
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
#include "fdr.h"


int main(int argc, char *argv[])
{
  char history[STRINGLENGTH*10];
  char hist1[STRINGLENGTH*2];
  char hist2[STRINGLENGTH*2];
  char hist3[STRINGLENGTH*2];

  long n3, n2, n1;
  long psfn3, psfn2, psfn1;
  mihandle_t inputfile1, inputfile2, outputfile;
  mihandle_t tmpfile1, tmpfile2, tmpfile3;
  double step3, step2, step1;
  double start3, start2, start1;
  char path[STRINGLENGTH];
  char f1[STRINGLENGTH], f2[STRINGLENGTH], f3[STRINGLENGTH];
  int result, i;
  int keep=0;
  double starts[NDIMS];
  double separations[NDIMS];
  history[0]=hist1[0]=hist2[0]=hist3[0]='\0';
  path[0]=f1[0]=f2[0]=f3[0]='\0';

  int usage(void)
  {
   fprintf(stderr, "Usage: max_limit_recovery and rolloff_bandlimit file  and projections_fdr_output\n");
   fprintf(stderr, " -k keep all filters in -p specified path!\n");
   exit(1);
   }
 
  if (argc < 4)
   usage();
   
  for (i=0;i<argc;i++)
  {
   strcat(hist3, argv[i]);
   strcat(hist3," ");
   if (argv[i][0] == '-')
   {
    switch (argv[i][1])
    {
     case 'k':
       keep = 1;
       break;
     case 'p':
       strcat(path,argv[i+1]);
       if(path[strlen(path)-1] !="/")
         strcat(path,"/");
       break;
     default:
       printf("Wrong Argument: %s\n", argv[i]);
       usage();
     }   
    }
   }
  strcat(history,"\n");

  result = open_minc_file_and_read(argv[1], &inputfile1);
  if(result) { return 1; }
  result = get_minc_spatial_dimensions_from_handle(inputfile1, &n3, &n2, &n1);
  if(result) { return 1; }
  result = get_minc_spatial_separations_from_handle(inputfile1, &step3,&step2,&step1); 
  if(result) { return 1; }
  result = get_minc_spatial_starts_from_handle(inputfile1, &start3,&start2,&start1); 
  if(result) { return 1; }

  starts[0] = start3;
  starts[1] = start2;
  starts[2] = start1;
  separations[0] = step3;
  separations[1] = step2;
  separations[2] = step1;

  result = open_minc_file_and_read(argv[2], &inputfile2);
  if(result) { return 1; }

  
  result = open_minc_file_and_write(argv[3], &outputfile, n3, n2, n1,  starts, separations,FLOAT);
  if(result) { return 1; }
    
  result = get_minc_history(inputfile1,hist1);
  if(result) { return 1; }
 
  result = get_minc_history(inputfile2,hist2);
  if(result) { return 1; }
  /** attempt to get all history from input files**/
  strcat(history,hist1);strcat(history,"\n");
  strcat(history,hist2);strcat(history,"\n");
  
  /** perform a complex multiplication **/
  strcat(f1,path);
  strcat(f1,"prj_fft_shift_realfilters_limit_recovery.mnc");
  result = open_minc_file_and_write(f1, &tmpfile1, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }
 
  strcat(f2,path);
  strcat(f2,"final_fdr_corrected_inverse.mnc");
  result = open_minc_file_and_write(f2, &tmpfile2, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }

  strcat(f3,path);
  strcat(f3,"final_fdr_corrected_inverse_shift.mnc");
  result = open_minc_file_and_write(f3, &tmpfile3, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }

  strcat(history,"complex multiply shifted FFT projections with rolloff and bandlimit to limit recovery\n");

  result = multiply_complex_by_complex(inputfile1, inputfile2, tmpfile1, n3, n2, n1);
  if(result) { return 1; }
  /** perform a 3D FFT inverse **/
  strcat(history,"apply the 3d inverse FFT \n");
  result = threedifft(tmpfile1, tmpfile2, n3, n2, n1);
  if(result) { return 1;}
  /** perform a 3D FFT SHIFT  **/
  strcat(history,"apply the 3d shift on the inverse FFT \n");
  result = cfftshift(tmpfile2, tmpfile3, n3, n2, n1);
  if(result) { return 1; }
  strcat(history,"calculate the magnitude \n");
  result = complex_magnitude(tmpfile3, outputfile, n3, n2, n1);
  if(result) { return 1; }
 
  result = miclose_volume(tmpfile1);
  if(result) { return 1; }

  result = miclose_volume(tmpfile2);
  if(result) { return 1; }
 
  result = miclose_volume(tmpfile3);
  if(result) { return 1; }

  result = micopy_attr(inputfile1,"/OPT",outputfile);
  if(result) {printf("could not copy attributes at path /OPT or does not exist\n");}

  result = miadd_history_attr(outputfile, strlen(history), history);
  if(result) {printf("could not history to minc header!!!\n");} 

  result = miclose_volume(inputfile1);
  if(result) { return 1; }
  result = miclose_volume(inputfile2);
  if(result) { return 1; }

  result = miclose_volume(outputfile);
  if(result) { return 1; }

  if (!keep)
  {
    result = remove(f1);
    result = remove(f2);
    result = remove(f3);
  }
  return 0;
}


