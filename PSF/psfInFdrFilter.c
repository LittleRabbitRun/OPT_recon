#include "useMINC2.h"
#include "fdr.h"


int main(int argc, char *argv[]) 
{
 char history[STRINGLENGTH];
 int result, i;
 long psfn3, psfn2, psfn1;
 double psfstep3, psfstep2, psfstep1;
 double psfstart3, psfstart2, psfstart1;

 long n3, n2, n1;
 double step3, step2, step1;
 double start3, start2, start1;
 //double distance_to_psf=217.80/2.0*2.45;
 double distance_to_psf=0.0;

 mihandle_t inputfile, outputfile, psffile;
 double starts[3], separations[3];
 history[0]='\0';
 int usage(void)
  {
   fprintf(stderr, "Usage: projections_filename PSF_filename and fdr_filter_output and options\n");
   fprintf(stderr, " -d distance to center of psf!\n");
   return 1;
  }
 
  if (argc < 4)
   usage();
  for (i=0;i<argc;i++)
  {
   strcat(history, argv[i]);
   strcat(history," ");

   if (argv[i][0] == '-')
   {
    switch (argv[i][1])
    {
     case 'd':
       distance_to_psf = atof(argv[i+1]);
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
  
 result = open_minc_file_and_read(argv[2], &psffile);
 if(result) { return 1; }
  
 result = get_minc_spatial_dimensions_from_handle(psffile, &psfn3, &psfn2, &psfn1);
 if(result) { return 1; }
 /*
 if(n2 != psfn2 || n1 != psfn1) 
 {
  fprintf(stderr, "psf and data dimensions do not match up (%ld,%ld) vs (%ld,%ld)\n", n2, n1, psfn2, psfn1);
  return 1;
 }
*/
 result = get_minc_spatial_separations_from_handle(psffile, &psfstep3,&psfstep2,&psfstep1);
 if(result) { return 1; }
 result = get_minc_spatial_separations_from_handle(inputfile, &step3,&step2,&step1);
 if(result) { return 1; }
 result = get_minc_spatial_starts_from_handle(inputfile1, &start3,&start2,&start1);
 if(result) { return 1; }

 starts[0] = start3;
 starts[1] = start2;
 starts[2] = start1;
 separations[0] = step3;
 separations[1] = step2;
 separations[2] = step1;

  //psfstart1 = (-psfn3/2.0)*psfstep3; changed to the following instead!
 //psfstart3 = -384.71*psfstep3+(217.80/2.0); //for psfn3=785
 //
 psfstart3 = -384.71*step3;
 /*
 psfstart2 = -(psfn2/2.0)*step2;
 psfstart1 = -(psfn1/2.0)*step1 + distance_to_psf; */
 psfstart2 = -(psfn2/2.0);
 psfstart1 = -(psfn1/2.0);
 result = open_minc_file_and_write(argv[3], &outputfile, n3, n2, n1, starts, separations,COMPLEX);
 if(result) { return 1; }
 result = miadd_history_attr(outputfile, strlen(history), history);
 if(result) {printf("could not history to minc header!!!\n");} 
 result = micopy_attr(inputfile,"/OPT",outputfile);
 if(result) {printf("could not copy attributes at path /OPT or does not exist\n");} 
     
 result = psf_in_fdr_filter(psffile, outputfile, 
                            step3, step2, step1,
                            n3, n2, n1, 
                            psfn3, psfn2, psfn1,
                            psfstep3, psfstep2, psfstep1,
                            psfstart3, psfstart2, psfstart1);

                      
 if(result) { return 1; }

 result = miclose_volume(inputfile);
 if(result) { return 1; }

 result = miclose_volume(psffile);
 if(result) { return 1; }
 
 result = miclose_volume(outputfile);
 if(result) { return 1; }


 return 0;
}
