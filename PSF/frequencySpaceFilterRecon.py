#!/usr/bin/python

import os
import subprocess
import optparse
from OPT_recon.masterConfig import *
from pyminc.volumes.factory import *
program_name = 'frequencySpaceFilterRecon.py'
    
def call_command(command):
    process = subprocess.Popen(command,shell=True,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, 
                               close_fds=True)
    return process.communicate()

def process_options(projections, psf,output,options):
    
    if (options.path):
       if (options.path[-1] != '/'):
          filter_path=options.path+'/'
       else:
          filter_path=options.path
    else:
       filter_path="/tmp/"
    tmpfile1 =filter_path+"prj_fft_shift_rolloff_bandlimit.mnc"
    #f=findFile('opt_frequencySpaceFilterRecon_projections')
    command = "/projects/souris/baghdadi/OPT/OPT_recon_bin/PSF/opt_frequencySpaceFilterRecon_projections %s %s -w %s -m %s -b %s -k %s -p %s -l %s -r %s" % (projections, tmpfile1, options.weight, options.maxslope, options.bandlimit, options.keep, filter_path, options.blimit, options.roff)
    print command
    result = call_command(command)
    if (result[1]):
      print result[1]
      raise IOError,"Error runnning the first component of PSF correction"
    else:
      print "Finished running the first component of PSF correction" 
    
    tmpfile2 =filter_path+"limitrecovery_filter.mnc"
    #f=findFile('opt_frequencySpaceFilterRecon_fdr')
    command = "/projects/souris/baghdadi/OPT/OPT_recon_bin/PSF/opt_frequencySpaceFilterRecon_fdr %s %s %s -n %s -r %s -d %s -k %s -p %s" % (projections,psf,tmpfile2, options.noise, options.fdrlimit, options.distance, options.keep, filter_path)
    print command
    result = call_command(command)
    if (result[1]):
      print result[1]
      raise IOError,"Error runnning the second component of PSF correction"
    else:
      print "Finished running the second component of PSF correction" 

    tmpfile3 =filter_path+"final_fdr_corrected.mnc"
    #f=findFile('opt_frequencySpaceFilterRecon_complete')
    command = "/projects/souris/baghdadi/OPT/OPT_recon_bin/PSF/opt_frequencySpaceFilterRecon_complete %s %s %s -k %s -p %s" % (tmpfile1,tmpfile2,tmpfile3, options.keep, filter_path)
    print command
    result = call_command(command)
    if (result[1]):
      print result[1]
      raise IOError,"Error runnning the final component of PSF correction"
    else:
      print "Finished running the final component of PSF correction" 

    if (options.flip):
      #f=findFile('volrot')
      command = "/projects/souris/baghdadi/bin/volrot -z_rotation 180 %s %s" %(tmpfile3, output)
      result = call_command(command)
      if (not result[1]):
        print result[1]
        raise IOError,"Error flipping the PSF corrected image"
      else:
        print "Finished flipping the PSF corrected image"

    else:
      infile=volumeFromFile(tmpfile3)
      outputfile = volumeLikeFile(tmpfile3, output)
      outputfile.data=infile.data
      outputfile.writeFile()
      outputfile.closeVolume()
      infile.closeVolume()
    
    #remove tmp files if the user did not want to keep
    if not (options.keep):
      os.remove(tmpfile1)
      os.remove(tmpfile2)
      os.remove(tmpfile3)
    
     
if __name__ == "__main__":
    usage = "Usage: "+program_name+" projections.mnc PSF.mnc psf_corrected.mnc options\n"+\
            "   or  "+program_name+" --help";
    description = '''PSF correction of OPT projections'''

    parser = optparse.OptionParser(usage, description=description)
    parser.add_option("-w", "--weight", dest="weight",
                    default="100",
                    help="roll off filter weight")
    parser.add_option("-m", "--maxslope", dest="maxslope",
                    default="0.95",
                    help="roll off filter max slope")
    parser.add_option("-b", "--bandlimit", dest="bandlimit",
                    default="0.9",
                    help="bandlimit filter")
    parser.add_option("-n", "--noise", dest="noise",
                    default="0.2",
                    help="noise reduction wiener filter")
    parser.add_option("-r", "--fdrlimit", dest="fdrlimit",
                    default="5",
                    help="fdr limit of limit recovery filter")
    parser.add_option("-d", "--distance", dest="distance",
                    default="0.01",
                    help="distance to center of PSF")
    parser.add_option("-f", "--flip",dest="flip", default=True,
                    action="store_false",help="do not flip image")
    parser.add_option("-p","--path", dest="path", default=0,
                    action="store", help="save all filters in specified path",
                    type = "string")
    parser.add_option("-k","--keep", action="store_true", dest="keep",
                    default=0, help="keep all intermediate filters")

    parser.add_option("-l","--blimit", action="store_true", dest="keep",
                    default=0, help="use bandlimit filter")
    parser.add_option("-r","--roff", action="store_true", dest="keep",
                    default=0, help="use rolloff filter")


    options, args = parser.parse_args()
    #print len(args)
    if(len(args) < 3):
      parser.error("Abort! Incorrect arguments")
    projections = args[0]
    psf =args[1]
    output=args[2]
    #print projections, psf, output

    process_options(projections,psf,output,options)
