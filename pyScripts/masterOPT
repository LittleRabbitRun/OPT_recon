#!/usr/bin/python

import os
import subprocess
import optparse
from OPT_recon.masterConfig import *
from pyminc.volumes.factory import *
program_name = 'masterOPT.py'
    
def call_command(command):
    process = subprocess.Popen(command,shell=True,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, 
                               close_fds=True)
    return process.communicate()
    
def process_options(filename, path):
    # first build a 3D (mnc) file from the stack of bin files
    print "Attempting to build 3D minc file OPT bin files"
    f=findFile('optdataTOminc')
    command = "%s  -f %s -p %s" % (f,filename,path) 
    print command
    result = call_command(command)
    if (result[1]):
      print result[1]
      raise IOError,"Error creating 3D minc projection file"
    else:
      print "Finished building 3D minc file"      
    basefile=filename.split('.')[0]+"_projections.mnc"
    outfile = basefile
    # next apply the ccd correction using the default coefficients
    print "Attempting to apply CCD correction"
    import opt_applyccdcoeffs
    infile=outfile
    outfile=basefile.split('.')[0]+'_linearity_corrected.'+basefile.split('.')[-1]
    opt_applyccdcoeffs.process_options(infile=infile, outfile=outfile, path=path)
   
    print "Finished applying CCD correction"
    # next correct for fluctuations in illumination and photo-bleaching
    print "Attempting to correct for fluctuations and photo-bleaching"
    import opt_signaldecay
    infile=outfile
    outfile=basefile.split('.')[0]+'_linearity_decay_corrected.'+basefile.split('.')[-1]
    val=opt_signaldecay.process_options(infile=infile, outfile=outfile, path=path)
    if ( not val):
      print "Finished correcting for fluctuations and photo-bleaching"
    else:
      print "Proceed without correction"
      outfile=infile  
    # next apply the PSF correction of the recon
    print "Attempting to create and apply the PSF correction to the projections"
    import opt_frequencySpaceFilterRecon
    infile=outfile
    outfile=basefile.split('.')[0].split('_projections')[0]+'_psf.'+basefile.split('.')[-1]
    opt_frequencySpaceFilterRecon.process_options(infile=infile, psffile, outfile=outfile)
    # next apply back projection filter
    print "Attempting back projection with automatic offset correction"
    import opt_backproject
    infile=outfile
    outfile=basefile.split('.')[0].split('_projections')[0]+'_volume.'+basefile.split('.')[-1]
    opt_backproject.process_options(infile=infile, outfile=outfile,offset='auto', path=path)
    print "Finished reconstructing the OPT volume"

    #finally apply the cropping algorithm to chop the two ends of image
    print "Attempting to crop the reconstructed volume"
    import opt_crop_volume
    infile=outfile
    outfile=infile.split('.')[0]+'_cropped.'+infile.split('.')[-1]
    opt_crop_volume.process_options(infile=infile, outfile=outfile)
    print "Finished cropping the OPT volume"

if __name__ == "__main__":
    usage = "Usage: "+program_name+" -p inputpath output.mnc\n"+\
            "   or  "+program_name+" --help";
    description = '''building, correcting and reconstructing OPT projections'''
    parser = optparse.OptionParser(usage, description=description)
    parser.add_option("-p", "--path", dest="path",
                    action="store",
                    default=0,
                    help="create image using the specified path")
    options, args = parser.parse_args()
    
    if(len(args) < 1 and not options.path):
      parser.error("Abort! Incorrect arguments")
    
    #get filename and path separeted 
    if (len(args) == 0):
       path=os.path.dirname(options.path)
       path=path+'/'
       filename=os.path.basename(options.path)
    else:
       filename=args[0]
       path = makeDir(options.path)

    if(os.path.exists(filename)):
      parser.error( "The infile exists!", filename)
    
    process_options(filename,path)
