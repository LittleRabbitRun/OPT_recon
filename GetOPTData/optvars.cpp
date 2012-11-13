#include <optvars.h>
opt_vars::opt_vars()
{
  Focus = 24.0;
  Height = 4.5;
  Zoom = 2500;
  Averages = 1.0;
  Step = 1.0;
  ImageSize = 1024;
  Binning = 1.0;
  Exposure = 2500;
  Gain = 1.0;
  PixelSize=2.45;
}
opt_vars::~opt_vars()
{
}
int readOptLogFile(const string & filename, opt_vars &opt)
{
  
 FILE * logFile;
 string currentLine;
 string  currentValue;
 std::ifstream file(filename.c_str());
 std::string line;

 while(!file.eof())
  {
    std::getline(file, line);
    
    currentLine = line.substr(0, line.find_last_of(":") + 1);
    currentValue = line.substr(line.find_last_of(":") + 1);
    
    if (currentLine.compare("Focus:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str()));
    else if (currentLine.compare("Height:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str()));
    else if (currentLine.compare("Zoom:") == 0) 
      opt.setFocus((long)atol(currentValue.c_str()));
    else if (currentLine.compare("Averages:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str())); 
    else if (currentLine.compare("Step:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str())); 
    else if (currentLine.compare("Image Size:") == 0) 
      opt.setFocus((long)atol(currentValue.c_str()));
    else if (currentLine.compare("Binning:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str()));
    else if (currentLine.compare("Exposure:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str()));
    else if (currentLine.compare("Gain:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str()));
    else if (currentLine.compare("Pixel Size:") == 0) 
      opt.setFocus((float)atof(currentValue.c_str())); 
    else
      {
       cout <<"Can not recognize this variable "<< currentValue << endl; 
       return 1;
       }
  }
 return 0;
}
