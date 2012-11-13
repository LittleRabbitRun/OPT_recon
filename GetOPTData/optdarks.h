#include<filehandle.h>
#define MAX_NUMBER_OF_CHARS 128
class opt_darks {
  public:
  opt_darks();
  ~opt_darks();
  int averageDarkOptFiles(vector<string> &filename, char* path, long size, unsigned short * darkbuffer);
};

