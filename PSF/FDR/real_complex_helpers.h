#include "fdr.h"

int multiply_real_by_real(mihandle_t inputfile1, mihandle_t inputfile2, mihandle_t outputfile, 
                          long n3, long n2, long n1);
int multiply_complex_by_real(mihandle_t inputfile1, mihandle_t inputfile2, mihandle_t outputfile, 
                             long n3, long n2, long n1);
int multiply_complex_by_complex(mihandle_t inputfile1, mihandle_t inputfile2, mihandle_t outputfile,
                                long n3, long n2, long n1);

int complex_magnitude(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
