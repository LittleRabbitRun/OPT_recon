
#include "useMINC2.h"
#include "fdr.h"

int main(int argc, char *argv[]) 
{
  char history[STRINGLENGTH];
  int result,i;
  long n3,n2,n1;

  mihandle_t inputfile1, inputfile2, outputfile;
  double step3, step2, step1;
  double start3, start2, start1;

  double starts[3], separations[3];
  history[0]='\0';
  if(argc!=4) 
  {
    fprintf(stderr, "Usage: input file-complex and real  and an output file!\n");
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
  result = open_minc_file_and_read(argv[1], &inputfile1);
  if(result) { return 1; }

  result = open_minc_file_and_read(argv[2], &inputfile2);
  if(result) { return 1; }
      
  // Get dimensions from first file to duplicate for second file
  
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
    
  open_minc_file_and_write(argv[3], &outputfile, n3, n2, n1, starts, separations,COMPLEX);
  if(result) { return 1; }
  result = miadd_history_attr(outputfile, strlen(history), history);
  if(result) {printf("could not history to minc header!!!\n");} 
  result = micopy_attr(inputfile1,"/OPT",outputfile);
  if(result) {printf("could not copy attributes at path /OPT or does not exist\n");} 
    
  result = multiply_complex_by_real(inputfile1, inputfile2, outputfile, n3, n2, n1);
  if(result) { return 1; }
    
  result = miclose_volume(inputfile1); 
  if(result) { return 1; }
  result = miclose_volume(inputfile2); 
  if(result) { return 1; }

  result = miclose_volume(outputfile);
  if(result) { return 1; }

  return 0;

}
