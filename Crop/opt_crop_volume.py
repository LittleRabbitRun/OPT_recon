#!/usr/bin/python
# L.B Apr 2012
# modified the original pmincaverage.py (JP Lerch) to use
# with OPT image to determine the slices to crop from top and bottom
# added an ERROR correction to avoid spurious values,

ERROR = 20
import os, sys
from pyminc.volumes.factory import *
from numpy import *
from optparse import OptionParser
import multiprocessing as mp
from OPT_recon.masterConfig import *
import subprocess
program_name = 'opt_crop_volume.py'
volhandles = []

def getslice(volhandle, slice, q,nslices):
    """collects hyperslabs for a particular volume handle. Designed to
    be used with the multiprocessing module"""
    t = volhandles[volhandle].getHyperslab((slice,0,0),
                                           (nslices,volhandles[0].sizes[1],
                                            volhandles[0].sizes[2]))
    t.shape = (nslices,volhandles[0].sizes[1], volhandles[0].sizes[2])
    q.put((volhandle, t))

def getfile(q, filename):
    q.put(volumeFromFile(filename, dtype='double'))

def process_options(infile = None, outfile = None, threshold = 0.0, clobber=None):
    # open all the file handles (read all files), 
    #get filename and path separeted 
    p1 = os.path.dirname(infile)
    p2 = os.path.dirname(outfile)
    path=None
    if not p1:
       path=makeDir(path)
       infile=path+infile
    if not p2:
       path=makeDir(path)
       outfile=path+outfile

    nfiles = 1
    
    for i in range( nfiles ):
        #print("VOLHANDLES: %d" % i)
        #print("Processing Projections")
        volhandles.append(volumeFromFile(infile, dtype='float'))

    # create the slice array (projections)
    nslices = 10
    sliceArray = zeros( (nfiles,nslices,
                         volhandles[0].sizes[1],
                         volhandles[0].sizes[2]))
    # loop over all slices
    p = mp.Pool(processes=2)
    manager = mp.Manager()
    
    zsums = zeros((volhandles[0].sizes[0]))
    ysums = zeros((volhandles[0].sizes[1]))
    xsums = zeros((volhandles[0].sizes[2]))

    zmin=volhandles[0].sizes[0]
    zmax=0
    ymin=volhandles[0].sizes[1]
    ymax=0
    xmin=volhandles[0].sizes[2]
    xmax=0
    ct=0  
    for i in range(0,volhandles[0].sizes[0],nslices):
        # last set might have less than n-slices - adjust.
        if (i + nslices) > volhandles[0].sizes[0]:
            nslices = volhandles[0].sizes[0] - i
            sliceArray = zeros( (nfiles,nslices,
                                 volhandles[0].sizes[1],
                                 volhandles[0].sizes[2]))
        print "Processing 10 Slices starting at: %i" % i
        # use parallel processing to get the projection data
        q = manager.Queue()
        for j in range(nfiles):
            t = p.apply_async(getslice, (j,i,q,nslices))

        t.get() # seems to be necessary as a sort of waitpid call ...

        # retrieve the data from the queue
        while not q.empty():
            ix, t = q.get()
            sliceArray[ix,::,::,::] = t
        
        # get each slice and create the average
        for k in range(nslices):
            sl = sliceArray[::,k,::,::]
            sl.shape=(volhandles[0].sizes[1], volhandles[0].sizes[2])        
            zsums[ct] = sl.max()
            ct+=1
            ysums +=sl.sum(axis=1)
            xsums +=sl.sum(axis=0)
               

    volhandles[0].closeVolume()
    #make sure the first four high digits do not mess up the calculations
    for i in range(ERROR):
        xsums[i]=-1
        ysums[i]=-1
        zsums[i]=-1
        xsums[volhandles[0].sizes[2]-1-i]=-1
        ysums[volhandles[0].sizes[1]-1-i]=-1
        zsums[volhandles[0].sizes[0]-1-i]=-1
    zmin = where(zsums>100)[0][0]
    zmax = where(zsums>100)[0][-1]
    
    ymin = where(ysums>0)[0][0] - ERROR
    ymax = where(ysums>0)[0][-1] + ERROR
    
    xmin = where(xsums>0)[0][0] - ERROR
    xmax = where(xsums>0)[0][-1] + ERROR
    
    z_start =zmin
    z_end = zmax-zmin
    y_start = ymin
    y_end = ymax-ymin
    x_start = xmin
    x_end = xmax-xmin

   
    if (clobber):
      clstr="-clobber"
    else:
      clstr=" "
    
    command = "mincreshape -start %d,%d,%d -count %d,%d,%d %s %s %s" % (z_start,y_start,x_start,z_end, y_end,x_end, infile,outfile, clstr)   
    print command
    process = subprocess.Popen(command,shell=True,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE, 
                               close_fds=True)
    result = process.communicate()
    
    if (result[0]):
      print result[0]
      raise IOError,"Error creating cropped 3D minc file"
    else:     
      print result[1]
    
if __name__ == "__main__":

    usage = "Usage: " + program_name+" input.mnc output.mnc\n"+\
            "   or  " + program_name+" --help";
    description = '''cropping the EMPTY opt projections from top and bottom of image'''
    
    parser = OptionParser(usage=usage, description=description)
    parser.add_option("--clobber", action="store_true", dest="clobber",
                       default=False, help="overwriting output file(s)")
    parser.add_option("-t","--threshold",dest="threshold",default="0.0",
                  help="The threshold that is used to determine whether any slice contains data")
    
    (options, args) = parser.parse_args()
    if len(args) < 2:
        parser.error("Incorrect number of arguments")
    
    infile, outfile = args
    
    if(not os.path.exists(infile)):
      parser.error( "The infile does not exist", infile)

    if(os.path.exists(outfile) and not options.clobber):
      parser.error( "The outfile already exists!" )
    
    process_options(infile, outfile, options.threshold, options.clobber)
    
    
                                                          
    
