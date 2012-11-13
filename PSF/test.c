#include "useMINC2.h"
#include "fdr.h"


int main(int argc, char *argv[]) 
{
 
 float result1, result2;
 fdr_complex value,value2;

 value = -5.0+(7.5*I);

 result1=sqrt(fabs(creal(value)*creal(value)+cimag(value)*cimag(value)));

 printf(" %6.4f \n", result1);

 value2 = creal(value)+(-1*cimag(value)*I);
 printf(" %6.4f %6.4f \n", creal(value2), cimag(value2));
 
 return 0;
}
