#include <optdarks.h>

opt_darks::opt_darks()
{

}

opt_darks::~opt_darks()
{
}

int opt_darks::averageDarkOptFiles(vector<string> &filename, char* path, long size, unsigned short * darkbuffer)
{
  FILE *pFile;
  char *fullbinpath=(char*)malloc(MAX_NUMBER_OF_CHARS);
  unsigned short* buffer;
  int result;
 
  // allocate memory to contain the one slice only
  buffer = (unsigned short*) malloc (sizeof(unsigned short)*size);
  memset(buffer,0,size);
  for(vector<string>::iterator it = filename.begin();it != filename.end(); it++)
    {
      if (path[0] != '.')
        strcpy(fullbinpath,path);
      strcat(fullbinpath, it->c_str());    
      pFile = fopen (fullbinpath, "r");
      if (pFile==NULL)
      {
       cout<< "reading error, can not read file" << endl;
       return 1;
      } 
     // copy the file into the buffer
     result = fread (buffer,sizeof(unsigned short),size,pFile);  
     if (result != size) 
     {
      cout<< "reading error, did not retrieve same number of bytes" << endl;
      return 1;
     }
     for(int i=0; i < size; i++)
     {
       darkbuffer[i] += buffer[i];
     }
    fullbinpath[0]='\0';
    fclose (pFile);
    }
  free(buffer);
  return 0;
}
