#include "useMINC2.h"
#include "fdr.h"


int main(int argc, char *argv[]) 
{
 char history[STRINGLENGTH];
 int result,i;
 mihandle_t inputfile, outputfile;
 long n3,n2,n1;
 double step3,step2,step1;
 double start3, start2, start1;
 double weight = 100;
 double maxslope = 0.95;
 double starts[3], separations[3];
 history[0]='\0';
 void usage(void)
  {
   fprintf(stderr, "Usage: projections_filename rolloff_filter and options\n");
   fprintf(stderr, " -m maxslope -w weight!\n");
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
     case 'm':
       maxslope = atof(argv[i+1]);
       break;

     case 'w':
       weight = atof(argv[i+1]);
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
 
 result = open_minc_file_and_write(argv[2], &outputfile, n3, n2, n1, starts, separations,FLOAT);
 if(result) { return 1; }
 result = miadd_history_attr(outputfile, strlen(history), history);
 if(result) {printf("could not history to minc header!!!\n");} 
 result = micopy_attr(inputfile,"/OPT",outputfile);
 if(result) {printf("could not copy attributes at path /OPT or does not exist\n");} 
     
 result = rolloff_filter(outputfile, weight, maxslope, n3,n2,n1,step2,step1);
 if(result) { return 1; }

 result = miclose_volume(inputfile);
 if(result) { return 1; }

 result = miclose_volume(outputfile);
 if(result) { return 1; }


 return 0;
}
