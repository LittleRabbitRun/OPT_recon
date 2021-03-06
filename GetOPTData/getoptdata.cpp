/*****************************************************************************
** FILE IDENTIFICATION
**
**      Name:         getdata.cpp
**      Purpose:      reading all raw data along with information
**                    in the log file
**      Programmer:   L.B
**      Date Started: Jul 2011
**
**  This is part of the OPT_RECON program
**  
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License (version 2) as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
******************************************************************************/
#define NUMBER_OF_DIMENSIONS 3
#define MAX_NUMBER_OF_CHARS 128


extern "C" {
#include <netcdf.h>
#include <hdf5.h>
#include <minc2.h>
#include <fdr.h>
}

#include<filehandle.h>
#include<optvars.h>
#include<optdarks.h>
#include <math.h>
#include <sys/stat.h>

using namespace std;

int main(int argc, char *argv[])
{
  FILE *pFile;
  struct stat sb;

  vector<string> files;
  vector<string> darkfiles;
  long nz, ny, nx;
  float step;
  unsigned int k=0;
  mihandle_t fileout;
  unsigned long start[3], count[3];
  int result;
  opt_vars opt;
  opt_darks darkopt;
  unsigned short* buffer;
  unsigned short* darkbuffer;
  double starts[NUMBER_OF_DIMENSIONS];
  double separations[NUMBER_OF_DIMENSIONS];
  char *f=(char*)malloc(MAX_NUMBER_OF_CHARS);
  char *p=(char*)malloc(MAX_NUMBER_OF_CHARS);
  char *specimen=(char*)malloc(MAX_NUMBER_OF_CHARS);
  char *fullbinpath=(char*)malloc(MAX_NUMBER_OF_CHARS);
  char *fullpath=(char*)malloc(MAX_NUMBER_OF_CHARS);
  char *fulloutputpath=(char*)malloc(MAX_NUMBER_OF_CHARS);
  char * filename = getCmdOption(argv, argv + argc, "-f");
  char * path = getCmdOption(argv, argv + argc, "-p");
  bool subtractdarkfiles=cmdOptionExists(argv, argv + argc, "-s");  
  string logfile;
  string history;
  char * pch;
  double zoom;
  
  if (!filename and !path)
    {
     cerr<< "Usage: optdataTOminc -p inputpath -f output.mnc"<< endl;
     cerr<< "converting all .bin files in given directory to a 3D minc image" << endl;
     return 1;
     } 
  if (filename)
    {
     pch=strrchr(filename,'.');
     strncpy(f,filename,pch-filename);
     strcat(f,"_projections");
     strcat(f,".mnc");
     cout << "Creating 3D dataset " << f << endl;
    }
  else
    {
    cout << "Creating 3D dataset 'projections.mnc' " << endl;
    
    strcpy(f,"projections.mnc");
    }
  if (path)
    {
     cout << "Using raw files in directory " << path << endl;
     if (path[strlen(path)-1] != '/')
       {
       int len = strlen(path);
       path[len] = '/';
       path[len+1] = '\0';
       
       }
     
     strcpy(p,path);
     strcpy(fullpath,p);
     
    }
  else
    {
     cout << "In current directory" << endl;
     p[0]='.';
     p[1]='\0';
    }
  
  if (stat(ExtractDirectory(f).c_str(), &sb) == 0 )
    strcpy(fulloutputpath,f);
  else
    {
    strcat(fullpath,f);
    strcpy(fulloutputpath,fullpath);
    }
 
  //get all filesnames.bin, darkfilenames.bin and logfile sorted
  
  result=getDir(p,files, darkfiles,logfile);
  if (result!=0)
    {
    cerr<< "Error: having diffculty locating files, check the directory" << endl;
    return 1;
    }
  // insert path of logfile if its not the current directory
  if (p[0] !='.')
    logfile.insert(0,p);
  
  readOptLogFile(logfile, opt);
  
  nz = files.size();
  ny = nx = opt.getImageSize();
  cout << "Located number of slices in bin format: " << nz << endl;
  
  starts[0]=starts[1]=starts[2]=0.0;
  //separations[0]=opt.getStep(); //view angle in degrees
  zoom=opt.getZoom();
  // find pixel size at focal plane, convert to mm
  separations[0]=separations[1]=separations[2]=((-0.000000000000945*pow(zoom,3))+(0.000000066331990*pow(zoom,2))-(0.001513663713468*zoom)+12.797444248822865)*0.001;
 
  // attempt to subtract background or not
  if (subtractdarkfiles)
    {
    cout<< "NOT Subtracting background images or use -s to subtract dark files" << endl;  
    darkbuffer = (unsigned short*) malloc (sizeof(unsigned short)*nx*ny);
    if (darkbuffer == NULL)
     {
      cerr<< "Memory error" << endl;
      return 1;
      } 
    memset(darkbuffer,0,nx*ny);
    result = darkopt.averageDarkOptFiles(darkfiles,p,nx*ny,darkbuffer);
    if (result!=0)
      {
      cerr<< "Error: averaging dark images," << endl;
      return 1;
      } 
    }
  // allocate memory for one slice only
  buffer = (unsigned short*) malloc (sizeof(unsigned short)*nx*ny);
  if (buffer == NULL)
  {
   cerr<< "Memory error" << endl;
   return 1;
  } 
 
  //create minc file 
  result = open_minc_file_and_write(fulloutputpath, &fileout,nz ,ny, nx, starts, separations, USHORT);
  if(result == MI_ERROR)
    {
     cerr <<"Error creating minc file" << endl;
     return 1;
    }
 
  for(vector<string>::iterator it = files.begin();it != files.end(); it++)
    {
      if (p[0] != '.')
        strcpy(fullbinpath,p);
    strcat(fullbinpath, it->c_str());    
    
    pFile = fopen (fullbinpath, "r");
    if (pFile==NULL)
    {
      cout<< "reading error, can not read file" << endl;
      return 1;
    } 
    // copy the file into the buffer
    result = fread (buffer,sizeof(unsigned short),nx*ny,pFile);  
    if (result != nx*ny) 
    {
      cout<< "reading error, did not retrieve same number of bytes" << endl;
      return 1;
    }
    // check for dark background
   
    if (subtractdarkfiles)
    {
      float ndark_div= 1/(float)darkfiles.size();
      for(int i=0; i < nx*ny; i++)
	{
	  buffer[i] = fabs(buffer[i]-(darkbuffer[i]*ndark_div));
	}
    }
    
    start[0] = (unsigned long)k; start[1] = start[2] = 0;
    count[0] = 1;
    count[1] = (unsigned long)ny;
    count[2] = (unsigned long)nx;
    // Write the slice to file 
    result = miset_voxel_value_hyperslab(fileout,MI_TYPE_USHORT,start,count,buffer);
    if(result == MI_ERROR)
    {
     cerr <<"Error in setting values" << endl;
     return 1;
    }
    k++;
    fullbinpath[0]='\0';
    fclose (pFile);
    }
    
  // set data range to 0 4095 (actual data range 12bit)
  result = miset_volume_range(fileout,4095,0);
  if(result == MI_ERROR)
    {
    cerr <<"Error in setting values" << endl;
    return 1;
    }
  strcpy(specimen,ExtractFileName(logfile).c_str());
  
  // add all opt attributes to mincheader
  result = miset_attr_values(fileout, MI_TYPE_STRING, "/OPT", "specimen", strlen(specimen), specimen);
  result = miset_attr_values(fileout, MI_TYPE_STRING, "/OPT", "zspace", 22, "view angle in degrees");
  result = miset_attr_values(fileout, MI_TYPE_FLOAT, "/OPT", "focus", 1, &opt.getFocus());
  result = miset_attr_values(fileout, MI_TYPE_FLOAT, "/OPT", "height", 1, &opt.getHeight());
  result = miset_attr_values(fileout, MI_TYPE_INT, "/OPT", "zoom", 1, &opt.getZoom());
  result = miset_attr_values(fileout, MI_TYPE_FLOAT, "/OPT", "averages", 1, &opt.getAverages());
  result = miset_attr_values(fileout, MI_TYPE_FLOAT, "/OPT", "binning", 1, &opt.getBinning());
  result = miset_attr_values(fileout, MI_TYPE_FLOAT, "/OPT", "exposure", 1, &opt.getExposure());
  result = miset_attr_values(fileout, MI_TYPE_FLOAT, "/OPT", "gain", 1, &opt.getGain());
  result = miset_attr_values(fileout, MI_TYPE_FLOAT, "/OPT", "pixelsize", 1, &opt.getPixelSize());

  if(result == MI_ERROR)
    {
     cerr <<"Error having trouble setting one or more attributes" << endl;
     return 1;
    }
  // add short history to minc header
  time_t t = time(0);
  tm* localtm = localtime(&t);
  history.append(asctime(localtm));
  history.append("Created using MICe OPT recon software, from ");
  history.append(p);
  history.append("*.bin files and log file ");
  history.append(logfile);
  if (subtractdarkfiles )
    history.append(" and subtracted the averaged dark files");
  history.append("\n");
  result = miadd_history_attr(fileout, history.length(), history.c_str());
  if(result == MI_ERROR)
   {
   cerr <<"Error in setting history" << endl;
   return 1;
   }
  free (f);
  free (p);
  free (fullpath);
  free (fullbinpath);
  free (buffer);
  free (fulloutputpath);
  free (specimen);
  if (subtractdarkfiles)
    {
    free (darkbuffer);
    }
  result = miclose_volume(fileout);
  if(!result) { return 1; }
    return 0;
   
}


