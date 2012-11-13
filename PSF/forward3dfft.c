#include "useMINC2.h"
#include "fdr.h"

int main(int argc, char *argv[]) 
{
 char history[STRINGLENGTH];
 int result, i;
 long n3,n2,n1;
 mihandle_t inputfile, outputfile;
 double step3, step2, step1;
 double start3, start2, start1;
 double starts[3], separations[3];
 history[0]='\0';
 if(argc!=3) 
 {
  fprintf(stderr, "Usage: input file and an output file!\n");
  return 1;
 }
 if (argc > 1)
 {
  for (i=0;i<argc;i++)
  {
  strcat(history, argv[i]);
  strcat(history," ");
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

 starts[0] = start3;
 starts[1] = start2;
 starts[2] = start1;
 separations[0] = step3;
 separations[1] = step2;
 separations[2] = step1;

 result = open_minc_file_and_write(argv[2], &outputfile, n3, n2, n1,  starts, separations,COMPLEX);
 if(result) { return 1; }
 result = miadd_history_attr(outputfile, strlen(history), history);
 if(result) {printf("could not history to minc header!!!\n");} 
 result = micopy_attr(inputfile,"/OPT",outputfile);
 if(result) {printf("could not copy attributes at path /OPT or does not exist\n");}
		
 result = threedfft(inputfile, outputfile, n3, n2, n1);
 //result = full_3dfft(inputfile, outputfile, n3, n2, n1);

 if(result) { return 1;}
    
 result = miclose_volume(inputfile);
 if(result) { return 1;}

 result = miclose_volume(outputfile);
 if(result) { return 1;}

 return 0;

}
