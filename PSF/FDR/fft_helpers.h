#include "fdr.h"

int shift_float_to_complex(long elems, fdr_complex *data);
int rfftshift(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int cfftshift(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int threedfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int threedifft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int series_1dfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int full_2dfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int full_2difft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int full_3dfft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);
int full_3difft(mihandle_t inputfile, mihandle_t outputfile, long n3, long n2, long n1);


