#include "useMINC2.h"
#include "fdr.h"


int main(int argc, char *argv[]) 
{
 char history[STRINGLENGTH];
 int result, i;
 long n3, n2, n1;
 double step3, step2, step1;
 double start3, start2, start1;
 double noise_reduction = 0.2;
 mihandle_t fdrfile, deconvfile;
 double starts[3], separations[3];
 history[0]='\0';
 void usage(void)
  {
   fprintf(stderr, "Usage: FDR_filename and DECONV_filename (deconvolution filter) and options\n");
   fprintf(stderr, " -n noise_reduction!\n");
   return 1;
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
     default:
       printf("Wrong Argument: %s\n", argv[i]);
       usage();
     }   
    }
   }
  strcat(history,"\n");

 result = open_minc_file_and_read(argv[1], &fdrfile);
 if(result) { return 1; }
  
 result = get_minc_spatial_dimensions_from_handle(fdrfile, &n3, &n2, &n1);
 if(result) { return 1; }  
 result = get_minc_spatial_separations_from_handle(fdrfile, &step3,&step2,&step1);
 if(result) { return 1; }
 result = get_minc_spatial_starts_from_handle(fdrfile, &start3,&start2,&start1);
 if(result) { return 1; }
 
 starts[0] = start3;
 starts[1] = start2;
 starts[2] = start1;
 separations[0] = step3;
 separations[1] = step2;
 separations[2] = step1;

 result = open_minc_file_and_write(argv[2], &deconvfile, n3, n2, n1, starts, separations,COMPLEX);
 if(result) { return 1; }
 result = miadd_history_attr(deconvfile, strlen(history), history);
 if(result) {printf("could not history to minc header!!!\n");} 
 result = micopy_attr(fdrfile,"/OPT",deconvfile);
 if(result) {printf("could not copy attributes at path /OPT or does not exist\n");} 
 printf(" NOISE REDUCTION IS %5.2f \n", noise_reduction);     
 result = deconvolution_in_fdr_space_filter(fdrfile, deconvfile, n3, n2, n1, noise_reduction);               
 if(result) { return 1; }

 result = miclose_volume(fdrfile);
 if(result) { return 1; }
 
 result = miclose_volume(deconvfile);
 if(result) { return 1; }

 return 0;
}
