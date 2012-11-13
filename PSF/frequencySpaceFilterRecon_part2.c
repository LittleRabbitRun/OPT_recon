/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         frequencySpaceFilterRecon_part2.c
**      Purpose:      frequency space filter reconstruction psf
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
  char history[STRINGLENGTH*4];

  long n3, n2, n1;
  double step3, step2, step1;
  double start3, start2, start1;

  long psfn3, psfn2, psfn1;
  double psfstep3, psfstep2, psfstep1;
  double psfstart3, psfstart2, psfstart1;
  char path[STRINGLENGTH];
  char psfdir[STRINGLENGTH];
  char psfcenter[STRINGLENGTH];
  char f1[STRINGLENGTH], f2[STRINGLENGTH], f3[STRINGLENGTH];
  mihandle_t inputfile, psffile, outputfile;
  mihandle_t fdrfile, deconvfile, tmpfile1;
  double fdrlimit=5;
  double noise_reduction=0.2;
  float distance_to_psf = 0.0;
  int result, i;
  int keep=0;
  double starts[NDIMS];
  double separations[NDIMS];
  long zoom;
  char buffer[6];
  char *pch;
  float val[3];
  history[0]='\0';
  path[0]=f1[0]=f2[0]=f3[0]='\0';
  psfdir[0]=psfcenter[0]='\0';
  
  int usage(void)
  {
   fprintf(stderr, "Usage: projection file and psf file (optional) and projections_fdr_output ...\n");
   fprintf(stderr, " -n noise_reduction (wiener filter) .. \n");
   fprintf(stderr, " -r fdrlimit (limit recovery ) -d distance to center of psf!\n");
   fprintf(stderr, " -k keep all filters in -p specified path!\n");
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
     case 'n':
       noise_reduction = atof(argv[i+1]);
       break;
     case 'r':
       fdrlimit = atof(argv[i+1]);
       break;
     case 'd':
       distance_to_psf = atof(argv[i+1]);
       break;
     case 'k':
       keep = 1;
       break;
     case 'p':
       strcat(path,argv[i+1]);
       if(path[strlen(path)-1] !="/")
         strcat(path,"/");
       break;
     case 'f':
       strcat(psfdir,argv[i+1]);
       break;
     case 'c':
       strcat(psfcenter,argv[i+1]);
       i++;
       break;
     default:
       printf("Wrong Argument: %s\n", argv[i]);
       usage();
     }   
    }
   }
  strcat(history,"\n");
  if (psfcenter[0] != '\0')
  {
   pch = strtok (psfcenter,",");
   for (i=0; i< 3; i++)
   {
    if (pch!=NULL)
    {
    val[i]=atof(pch);
    printf ("psfstart%d= %5.2f\n",i, val[i]);
    pch = strtok (NULL, ",");
    }
   else
    {
    printf("Error need three center values xcenter,ycenter,zcenter \n");
    exit(1);
    }
   }
  }

  result = open_minc_file_and_read(argv[1], &inputfile);
  if(result) { return 1; }
  result = get_minc_spatial_dimensions_from_handle(inputfile, &n3, &n2, &n1);
  if(result) { return 1; }
  result = get_minc_spatial_separations_from_handle(inputfile, &step3,&step2,&step1); 
  if(result) { return 1; }
  result = get_minc_spatial_starts_from_handle(inputfile, &start3,&start2,&start1); 
  if(result) { return 1; }
 
  result = get_minc_attribute(inputfile, "zoom", MI_TYPE_INT,&zoom);
  if(result) { return 1; }

/*
  result = get_minc_attribute(inputfile, "focus", MI_TYPE_FLOAT,&distance_to_psf);
  if(result) { return 1; }
*/

  
  /* make a file name for the psf using the zoom level */
  if (psfdir[0] == '\0')
  {
   strcat(psfdir,"psf");
   sprintf(buffer,"%d",zoom);
   strcat(psfdir,buffer);
   strcat(psfdir,".mnc");
  }
  result = open_minc_file_and_read(psfdir, &psffile);
  if(result) { return 1; }
  result = get_minc_spatial_dimensions_from_handle(psffile, &psfn3, &psfn2, &psfn1);
  if(result) { return 1; }
  result = get_minc_spatial_separations_from_handle(psffile, &psfstep3,&psfstep2,&psfstep1);
  if(result) { return 1; }

  if (n2 != psfn2 || n1 != psfn1)
  {
   printf("Warning: dimensions of projections and psf file do not match!!! \n");
   printf("Warning: results might not be satisfactory!! \n");
  }
  
  starts[0] = start3;
  starts[1] = start2;
  starts[2] = start1;
  separations[0] = step3;
  separations[1] = step2;
  separations[2] = step1;
  
  result = open_minc_file_and_write(argv[2], &outputfile, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }

  strcat(f1,path);
  strcat(f1,"psfstack_fft.mnc");
  result = open_minc_file_and_write(f1, &tmpfile1, psfn3, psfn2, psfn1,  starts, separations,COMPLEX);
  if(result) { return 1; }
  result = psfstack_fft(psffile, tmpfile1, psfn3, psfn2, psfn1);
  if(result) { return 1; }
  
  //psfstart3 = (-psfn3/2.0)*psfstep3; changed to the following instead!
  /* figure out if psf center was given by user or use default */
  if (psfcenter[0] == '\0')
  {
   psfstart3 = -384.71;
   psfstart2 = -psfn2/2.0;
   psfstart1 = -psfn1/2.0 + distance_to_psf;
  }
  else
  {
   psfstart1=val[0];
   psfstart2=val[1];
   psfstart3=val[2];

  }
  strcat(f2,path);
  strcat(f2,"fdr_filter.mnc");
  result = open_minc_file_and_write(f2, &fdrfile, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }
  strcat(history,"psf in fdr filter (psf_in_fdr_filter)\n");

  result = psf_in_fdr_filter(tmpfile1, fdrfile, 
                             step3, step2, step1,
                             n3, n2, n1,
                             psfn3, psfn2, psfn1,
                             psfstep3, psfstep2, psfstep1,
                             psfstart3, psfstart2, psfstart1);
  if(result) { return 1; }

  result = miclose_volume(tmpfile1);
  if(result) { return 1; }

  strcat(f3,path);
  strcat(f3,"deconv_filter.mnc");
  result = open_minc_file_and_write(f3, &deconvfile, n3, n2, n1,  starts, separations,COMPLEX);
  if(result) { return 1; }

  strcat(history,"deconvolution in fdr filter (deconvolution_in_fdr_space_filter)\n");
  printf(" NOISE REDUCTION IS %5.2f \n", noise_reduction);   
  result = deconvolution_in_fdr_space_filter(fdrfile, deconvfile, n3, n2, n1, noise_reduction);               
  if(result) { return 1; }

  result = miclose_volume(fdrfile);
  if(result) { return 1; }
  

  strcat(history,"limit recovery filter (limit_recovery_filter)\n");

  result = limit_recovery_filter(deconvfile, outputfile, fdrlimit, n3, n2, n1);
  if(result) { return 1; }

  result = miclose_volume(deconvfile);
  if(result) { return 1; }
  

  result = micopy_attr(inputfile,"/OPT",outputfile);
  if(result) {printf("could not copy attributes at path /OPT or does not exist\n");}

  result = miadd_history_attr(outputfile, strlen(history), history);
  if(result) {printf("could not history to minc header!!!\n");} 

  result = miclose_volume(inputfile);
  if(result) { return 1; }
  result = miclose_volume(outputfile);
  if(result) { return 1; }
  result = miclose_volume(psffile);
  if(result) { return 1; }

  if (!keep)
   {
    result = remove(f1);
    result = remove(f2);
    result = remove(f3);
   }
  return 0;
}


