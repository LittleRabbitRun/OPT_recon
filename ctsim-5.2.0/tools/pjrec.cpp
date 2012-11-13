/*****************************************************************************
** FILE IDENTIFICATION
**
**   Name:          pjrec.cpp
**   Purpose:       Reconstruct an image from projections
**   Programmer:    Kevin Rosenberg
**   Date Started:  Aug 1984
**
**  This is part of the CTSim program
**  Copyright (C) 1983-2000 Kevin Rosenberg
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License (version 2) as
**  published by the Free Software Foundation.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

#include "ct.h"
#include "timer.h"

enum {O_OFFSET, O_INTERP, O_FILTER, O_FILTER_METHOD, O_ZEROPAD, O_FILTER_PARAM, O_FILTER_GENERATION, O_BACKPROJ, O_PREINTERPOLATION_FACTOR, O_VERBOSE, O_TRACE, O_ROI, O_HELP, O_DEBUG, O_VERSION};

static struct option my_options[] =
{
  {"offset", 1, 0, O_OFFSET},
  {"interp", 1, 0, O_INTERP},
  {"preinterpolation-factor", 1, 0, O_PREINTERPOLATION_FACTOR},
  {"filter", 1, 0, O_FILTER},
  {"filter-method", 1, 0, O_FILTER_METHOD},
  {"zeropad", 1, 0, O_ZEROPAD},
  {"filter-generation", 1, 0, O_FILTER_GENERATION},
  {"filter-param", 1, 0, O_FILTER_PARAM},
  {"backproj", 1, 0, O_BACKPROJ},
  {"trace", 1, 0, O_TRACE},
  {"roi", 1, 0, O_ROI},
  {"debug", 0, 0, O_DEBUG},
  {"verbose", 0, 0, O_VERBOSE},
  {"help", 0, 0, O_HELP},
  {"version", 0, 0, O_VERSION},
  {0, 0, 0, 0}
};

static const char* g_szIdStr = "$Id: pjrec.cpp,v 1.1 2005/10/12 23:24:58 jwalls Exp $";

void 
pjrec_usage (const char *program)
{
  std::cout << "usage: " << fileBasename(program) << " raysum-file image-file nx-image ny-image [OPTIONS]" << std::endl;
  std::cout << "Image reconstruction from raysum projections" << std::endl;
  std::cout << std::endl;
  std::cout << "  raysum-file     Input raysum file" << std::endl;
  std::cout << "  image-file      Output image file in SDF2D format" << std::endl;
  std::cout << "  nx-image        Number of columns in output image" << std::endl;
  std::cout << "  ny-image        Number of rows in output image" << std::endl;
  std::cout << "  --offset        Offset of rotational axis from center of detector array (units in pixels" << std::endl;
  std::cout << "  --interp        Interpolation method during backprojection" << std::endl;
  std::cout << "    nearest         Nearest neighbor interpolation" << std::endl;
  std::cout << "    linear          Linear interpolation (default)" << std::endl;
  std::cout << "    cubic           Cubic interpolation\n";
#if HAVE_BSPLINE_INTERP
  std::cout << "    bspline         B-spline interpolation" << std::endl;
#endif
  std::cout << "  --preinterpolate  Preinterpolation factor (default = 1)\n";
  std::cout << "                    Used only with frequency-based filtering\n";
  std::cout << "  --filter       Filter name" << std::endl;
  std::cout << "    abs_bandlimit  Abs * Bandlimiting (default)" << std::endl;
  std::cout << "    abs_sinc       Abs * Sinc" << std::endl;
  std::cout << "    abs_cosine     Abs * Cosine" << std::endl;
  std::cout << "    abs_hamming    Abs * Hamming" << std::endl;
  std::cout << "    abs_hanning    Abs * Hanning" << std::endl;
  std::cout << "    shepp          Shepp-Logan" << std::endl;
  std::cout << "    bandlimit      Bandlimiting" << std::endl;
  std::cout << "    sinc           Sinc" << std::endl;
  std::cout << "    cosine         Cosine" << std::endl;
  std::cout << "    triangle       Triangle" << std::endl;
  std::cout << "    hamming        Hamming" << std::endl;
  std::cout << "    hanning        Hanning" << std::endl;
  std::cout << "  --filter-method  Filter method before backprojections\n";;
  std::cout << "    convolution      Spatial filtering (default)\n";
  std::cout << "    fourier          Frequency filtering with discete fourier\n";
  std::cout << "    fourier_table    Frequency filtering with table lookup fourier\n";
  std::cout << "    fft              Fast Fourier Transform\n";
#if HAVE_FFTW
  std::cout << "    fftw             Fast Fourier Transform West library\n";
  std::cout << "    rfftw            Fast Fourier Transform West (real-mode) library\n";
#endif
  std::cout << "  --zeropad n   Set zeropad level (default = 0)\n";
  std::cout << "                set n to number of powers to two to pad\n";
  std::cout << "  --filter-generation  Filter Generation mode\n";
  std::cout << "    direct       Use direct filter in spatial or frequency domain (default)\n";
  std::cout << "    inverse_fourier  Use inverse fourier transform of inverse filter\n";
  std::cout << "  --backproj    Backprojection Method" << std::endl;
  std::cout << "    trig        Trigometric functions at every point" << std::endl;
  std::cout << "    table       Trigometric functions with precalculated table" << std::endl;
  std::cout << "    diff        Difference method" << std::endl;
  std::cout << "    diff2       Optimized difference method (default)" << std::endl;
  std::cout << "    idiff2      Optimized difference method with integer math" << std::endl;
  std::cout << "    idiff3      Highly-optimized difference method with integer math" << std::endl;
  std::cout << "  --filter-param Alpha level for Hamming filter" << std::endl;
  std::cout << "  --trace        Set tracing to level" << std::endl;
  std::cout << "     none        No tracing (default)" << std::endl;
  std::cout << "     console     Text level tracing" << std::endl;
  std::cout << "  --roi sx,ex,sy,ey Construct only the ROI" << std::endl;
  std::cout << "  --verbose      Turn on verbose mode" << std::endl;
  std::cout << "  --debug        Turn on debug mode" << std::endl;
  std::cout << "  --version      Print version" << std::endl;
  std::cout << "  --help         Print this help message" << std::endl;
}


int 
pjrec_main (int argc, char * const argv[])
{
  Projections projGlobal;
  ImageFile* imGlobal = NULL;
  ReconstructionROI *pROI = NULL;
  char* pszFilenameProj = NULL;
  char* pszFilenameImage = NULL;
  std::string sRemark;
  bool bOptVerbose = false;
  bool bOptDebug = 1;
  int iOptZeropad = 1;
  int optTrace = Trace::TRACE_NONE;
  double dOptFilterParam = -1;
  double dOptRotOffset = 0;
  std::string sOptFilterName (SignalFilter::convertFilterIDToName (SignalFilter::FILTER_ABS_BANDLIMIT));
  std::string sOptFilterMethodName (ProcessSignal::convertFilterMethodIDToName (ProcessSignal::FILTER_METHOD_CONVOLUTION));
  std::string sOptFilterGenerationName (ProcessSignal::convertFilterGenerationIDToName (ProcessSignal::FILTER_GENERATION_DIRECT));
  std::string sOptInterpName (Backprojector::convertInterpIDToName (Backprojector::INTERP_LINEAR));
  std::string sOptBackprojectName (Backprojector::convertBackprojectIDToName (Backprojector::BPROJ_IDIFF));
  int iOptPreinterpolationFactor = 1;
  int nx, ny;
  char *endptr;

  Timer timerProgram;

    while (1) {
      int c = getopt_long(argc, argv, "", my_options, NULL);
      char *endptr = NULL;
      
      if (c == -1)
	break;
      
      switch (c)
	{
	case O_OFFSET:
	  dOptRotOffset = strtod(optarg, &endptr);
	  if(endptr != optarg + strlen(optarg)) {
		pjrec_usage(argv[0]);
		return(1);
	  }
	  break;
	case O_INTERP:
	  sOptInterpName = optarg;
	  break;
	case O_PREINTERPOLATION_FACTOR:
	  iOptPreinterpolationFactor = strtol(optarg, &endptr, 10);
	  if (endptr != optarg + strlen(optarg)) {
	    pjrec_usage(argv[0]);
	    return(1);
	  }
	  break;
	case O_FILTER:
	  sOptFilterName = optarg;
	  break;
        case O_FILTER_METHOD:
	  sOptFilterMethodName = optarg;
          break;
	case O_FILTER_GENERATION:
	  sOptFilterGenerationName = optarg;
	  break;
	case O_FILTER_PARAM:
	  dOptFilterParam = strtod(optarg, &endptr);
	  if (endptr != optarg + strlen(optarg)) {
	    pjrec_usage(argv[0]);
	    return(1);
	  }
	  break;
	case O_ZEROPAD:
	  iOptZeropad = strtol(optarg, &endptr, 10);
	  if (endptr != optarg + strlen(optarg)) {
	    pjrec_usage(argv[0]);
	    return(1);
	  }
	  break;
	case O_BACKPROJ:
	  sOptBackprojectName = optarg;
	  break;
	case O_VERBOSE:
	  bOptVerbose = true;
	  break;
	case O_DEBUG:
	  bOptDebug = true;
	  break;
	case O_TRACE:
	  if ((optTrace = Trace::convertTraceNameToID(optarg)) == Trace::TRACE_INVALID) {
	    pjrec_usage(argv[0]);
	    return (1);
	  }
	  break;
	case O_ROI:
	  // At this point we need to extract 4 doubles from sx,ex,sy,ey
      // Taken from Advanced C Programming by Example by John W. Perry
      // Example 4.7, page 107

	  pROI = new ReconstructionROI;
      sscanf(optarg,"%lf,%lf,%lf,%lf", &(pROI->m_dXMin), &(pROI->m_dXMax), &(pROI->m_dYMin), &(pROI->m_dYMax));

      if((pROI->m_dXMax < pROI->m_dXMin)  || (pROI->m_dYMax < pROI->m_dYMin)) {
		std::cout << " ROI must be Xmin,XMax,YMin,YMax" << std::endl;
        pjrec_usage(argv[0]);
        return(1);
	  }

      break;
  
    case O_VERSION:
#ifdef VERSION
	  std::cout <<  "Version " <<  VERSION << std::endl << g_szIdStr << std::endl;
#else
          std::cout << "Unknown version number" << std::endl;
#endif
	  return (0);
	case O_HELP:
	case '?':
	  pjrec_usage(argv[0]);
	  return (0);
	default:
	  pjrec_usage(argv[0]);
	  return (1);
	}
    }
  
    if (optind + 4 != argc) {
      pjrec_usage(argv[0]);
      return (1);
    }

    pszFilenameProj = argv[optind];
  
    pszFilenameImage = argv[optind + 1];
  
    nx = strtol(argv[optind + 2], &endptr, 10);
    ny = strtol(argv[optind + 3], &endptr, 10);
  
    std::ostringstream filterDesc;
    if (dOptFilterParam >= 0)
      filterDesc << sOptFilterName << ": alpha=" << dOptFilterParam; 
    else
      filterDesc << sOptFilterName;

    std::ostringstream label;
    label << "pjrec: " << nx << "x" << ny << ", " << filterDesc.str() << ", " << sOptInterpName << ", preinterpolationFactor=" << iOptPreinterpolationFactor << ", " << sOptBackprojectName;
    sRemark = label.str();
  
    if (bOptVerbose)
      std::cout << "SRemark: " << sRemark << std::endl;
  projGlobal.read (pszFilenameProj);
  if (bOptVerbose) {
    std::ostringstream os;
    projGlobal.printScanInfo(os);
    std::cout << os.str();
  }

  projGlobal.setRotOffset(dOptRotOffset);

  imGlobal = new ImageFile (nx, ny);
  Reconstructor reconstruct (projGlobal, *imGlobal, sOptFilterName.c_str(), dOptFilterParam, sOptFilterMethodName.c_str(), iOptZeropad, sOptFilterGenerationName.c_str(), sOptInterpName.c_str(), iOptPreinterpolationFactor, sOptBackprojectName.c_str(), optTrace, pROI);
  if (reconstruct.fail()) {
    std::cout << reconstruct.failMessage();
    return (1);
  }
  reconstruct.reconstructAllViews();
    {
      double dCalcTime = timerProgram.timerEnd();
      imGlobal->labelAdd (projGlobal.getLabel());
      imGlobal->labelAdd (Array2dFileLabel::L_HISTORY, sRemark.c_str(), dCalcTime);
      imGlobal->fileWrite (pszFilenameImage);
      if (bOptVerbose)
	std::cout << "Run time: " << dCalcTime << " seconds" << std::endl;
    }
  return (0);
}



#ifndef NO_MAIN
int 
main (int argc, char* argv[])
{
  int retval = 1;

  try {
    retval = pjrec_main(argc, argv);
  } catch (exception e) {
	  std::cerr << "Exception: " << e.what() << std::endl;
  } catch (...) {
	  std::cerr << "Unknown exception" << std::endl;
  }

  return (retval);
}
#endif

