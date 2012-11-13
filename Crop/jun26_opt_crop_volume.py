#!/usr/bin/python
# L.B Apr 2012
# modified the original pmincaverage.py (JP Lerch) to use
# with OPT image to determine the slices to crop from top and bottom
# added an ERROR correction to avoid spurious values,

ERROR = 10
import os 
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
    ysums = zeros((volhandles[0].sizes[0],2), dtype=int)
    xsums = zeros((volhandles[0].sizes[0],2), dtype=int)

    zmin=volhandles[0].sizes[0]
    zmax=0
    ymin=volhandles[0].sizes[1]
    ymax=0
    xmin=volhandles[0].sizes[2]
    xmax=0
      
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
            x=where(sl.sum(axis=0)>float(threshold))[0]    
            y=where(sl.sum(axis=1)>float(threshold))[0]
            
            if (x !=[] and i+k > ERROR):
                if (x[0] < ERROR):
                    xsums[i+k,0] = volhandles[0].sizes[2]
                else:
                    xsums[i+k,0] = x[0]
                if (x[-1] < volhandles[0].sizes[2]-ERROR):
                    xsums[i+k,1] = x[-1]
                else:
                    xsums[i+k,1] = 0
            else:
                xsums[i+k,0] = volhandles[0].sizes[2]
                xsums[i+k,1] = 0
            if (y !=[] and i+k > ERROR):
                if (y[0] < ERROR):
                    ysums[i+k,0] = volhandles[0].sizes[1]
                else:
                    ysums[i+k,0] = y[0]
                if (y[-1] < volhandles[0].sizes[1]-ERROR):
                    ysums[i+k,1] = y[-1]
                else:
                    ysums[i+k,1] = 0
            else:
                ysums[i+k,0] = volhandles[0].sizes[1]
                ysums[i+k,1] = 0

            zsums[i+k] = sl.sum()
       
    zmin = where(zsums>0)[0][0]
    zmax = where(zsums>0)[0][-1]
   
    x = where(xsums[:,0]>0)[0]
    xmin = (xsums[x[0]:x[-1],0]).min()
    xmax = (xsums[x[0]:x[-1],1]).max()

    y = where(ysums[:,0]>0)[0]
    ymin = (ysums[y[0]:y[-1],0]).min()
    ymax = (ysums[y[0]:y[-1],1]).max()
    
    """
    print x[0], x[-1]
    print y[0], y[-1]
    print xmin, xmax
    print ymin, ymax
    print zmin, zmax
    if (zmin > x[0]):
       zmin = x[0]
    if (zmax < x[-1]):
       zmax = x[-1]
    """
    volhandles[0].closeVolume()
  
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
    
    
                                                          
    
