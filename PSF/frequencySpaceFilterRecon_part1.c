/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         frequencySpaceFilterRecon_part1.c
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
  char history[STRINGLENGTH*5];
  long n3, n2, n1;
  long psfn3, psfn2, psfn1;
  mihandle_t inputfile, outputfile;
  mihandle_t tmpfile1, tmpfile2;
  mihandle_t tmpfile3, tmpfile4, tmpfile5;
  double step3, step2, step1;
  double start3, start2, start1;
  char path[STRINGLENGTH];
  char f1[STRINGLENGTH], f2[STRINGLENGTH], f3[STRINGLENGTH];
  char f4[STRINGLENGTH], f5[STRINGLENGTH];
  int result, i;
  int keep=0;
  int roff_filter=0;
  int blimit_filter=0;
  double starts[NDIMS];
  double separations[NDIMS];
  double weight=100;
  double maxslope=0.95;
  double bandlimit=0.9;
  history[0]='\0';
  path[0]=f1[0]=f2[0]=f3[0]=f4[0]=f5[0]='\0';

  int usage(void)
  {
   fprintf(stderr, "Usage: projection file and rolloff_bandlimit_output and options ...\n");
   fprintf(stderr, " -w weight -m maxslope (calculation of rolloff).. \n");
   fprintf(stderr, " -b bandlimit  (bandlimit filter)!\n");
   fprintf(stderr, " -k keep all filters in -p specified path!\n");
   fprintf(stderr, " -l use bandlimit filter!\n");
   fprintf(stderr, " -r use rolloff filter!\n");
   exit(1);
   }
  if (argc < 3)
   usage();

  for (i=0;i<argc;i++)
  {
   strcat(history, argv[i]);
   strcat(history," ");

   if (argv[i][0] == '-')
   {
    switch (argv[i][1])
    {
     case 'b':
       bandlimit = atof(argv[i+1]);
       break;
     case 'w':
       weight = atof(argv[i+1]);
       break;
     case 'm':
       maxslope = atof(argv[i+1]);
       break;
     case 'k':
       keep = 1;
       break;
     case 'p':
       strcat(path,argv[i+1]);
       if(path[strlen(path)-1] !="/")
         strcat(path,"/");
       break;
     case 'r':
       roff_filter = 1;
       break;
     case 'l':
       blimit_filter = 1;
       break;
     default:
       printf("Wrong Argument: %s\n", argv[i]);
       usage();
     }   
    }
   }
  strcat(history,"\n");
   
  result = open_minc_file_and_read(argv[1], &inputfile);
  if(result) { return 1; }
  result = get_minc_spatial_dimensions_from_handle(inputfile, &n3, &n2, &n1);
  if(result) { return 1; }
  result = get_minc_spatial_separations_from_handle(inputfile, &step3,&step2,&step1); 
  if(result) { return 1; }
  result = get_minc_spatial_starts_from_handle(inputfile, &start3,&start2,&start1); 
  if(result) { return 1; }

  starts[0] = start3;
  starts[1] = start2;
  starts[2] = start1;
  separations[0] = step3;
  separations[1] = step2;
  separations[2] = step1;
  
  result = open_minc_file_and_write(argv[2], &outputfile, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }
    
  /** starting frequency space reconsturction **/
  /* Step1 : fft and fft shift of projections */

  strcat(f1,path);
  strcat(f1,"prj_fft.mnc");
  result = open_minc_file_and_write(f1, &tmpfile1, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }

  strcat(history,"perform a series of 2D FFT along the z direction (threedfft)\n");
  result = threedfft(inputfile, tmpfile1, n3, n2, n1);
  if(result) { return 1;}

  strcat(f2,path);
  strcat(f2,"prj_fft_shift.mnc");
  result = open_minc_file_and_write(f2, &tmpfile2, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }

  strcat(history,"perform a shift on COMPLEX FFT (cfftshift)\n");
  result = cfftshift(tmpfile1, tmpfile2, n3, n2, n1);
  if(result) { return 1; }

  result = miclose_volume(tmpfile1);
  if(result) { return 1; }
  

  /* Step2 :  roll off filter */
  if (roff_filter)
  {
   strcat(f3,path);
   strcat(f3,"rolloff_filter.mnc");
   result = open_minc_file_and_write(f3, &tmpfile3, n3, n2, n1,  starts, separations,FLOAT);
   if(result) { return 1; }
  
   strcat(history,"building rolloff filter (rolloff_filter)\n");

   result = rolloff_filter(tmpfile3, weight, maxslope, n3,n2,n1,step2,step1);
   if(result) { return 1; }

   
  }
  /* Step2 :  bandlimit filter */
  if (blimit_filter)
  {
   strcat(f4,path);
   strcat(f4,"bandlimit_filter.mnc");
   result = open_minc_file_and_write(f4, &tmpfile4, n3, n2, n1,  starts, separations,FLOAT);
   if(result) { return 1; }
  
   strcat(history,"building bandlimit filter (bandlimit_filter)\n");
   result = bandlimit_filter(tmpfile4, bandlimit,n3,n2,n1,step2,step1);
   if(result) { return 1; }

    }
  if (blimit_filter && roff_filter)
  {
   strcat(f5,path);
   strcat(f5,"rolloff_bandlimit_filter.mnc");
   result = open_minc_file_and_write(f5, &tmpfile5, n3, n2, n1,  starts, separations,FLOAT);
   if(result) { return 1; }

   strcat(history,"multiply all real filters (roll off and bandlimit)\n");
   result = multiply_real_by_real(tmpfile3, tmpfile4, tmpfile5, n3, n2, n1);
   if(result) { return 1; }

   strcat(history,"multiply shifted fft of projections by all filters requested.\n");
   result = multiply_complex_by_real(tmpfile2, tmpfile5, outputfile, n3, n2, n1);
   if(result) { return 1; }
   result = miclose_volume(tmpfile5);
   if(result) { return 1; }
  }
  else if (blimit_filter)
  {
   strcat(history,"multiply shifted fft of projections by bandlimit filter.\n");
   result = multiply_complex_by_real(tmpfile2, tmpfile4, outputfile, n3, n2, n1);
   if(result) { return 1; }

   result = miclose_volume(tmpfile4);
   if(result) { return 1; }
  }
  else if (roff_filter)
  {
   strcat(history,"multiply shifted fft of projections by rolloff filter.\n");
   result = multiply_complex_by_real(tmpfile2, tmpfile3, outputfile, n3, n2, n1);
   if(result) { return 1; }

   result = miclose_volume(tmpfile3);
   if(result) { return 1; }
  }

  result = micopy_attr(inputfile,"/OPT",outputfile);
  if(result) {printf("could not copy attributes at path /OPT or does not exist\n");}

  result = miclose_volume(tmpfile2);
  if(result) { return 1; }
  
  
  result = miadd_history_attr(outputfile, strlen(history), history);
  if(result) {printf("could not history to minc header!!!\n");} 

  result = miclose_volume(inputfile);
  if(result) { return 1; }
  result = miclose_volume(outputfile);
  if(result) { return 1; }

  if (!keep)
  {
    result = remove(f1);
    result = remove(f2);
    if (roff_filter)
     result = remove(f3);
    if (blimit_filter)
     result = remove(f4);
    if (roff_filter && blimit_filter)
     result = remove(f5);
  }
  return 0;
}
