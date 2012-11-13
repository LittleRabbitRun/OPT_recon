#!/usr/bin/env python

#
#  libOPT.py
#
#  back projection reconstruction of OPT data
#
#  Leila Baghdadi (adapted from jwalls code)

from scipy import *
from scipy.signal import *

from pylab import *
from pyminc.volumes.factory import *

from OPT_recon.masterConfig import *
import tempfile
import os
import shutil

import subprocess
from scipy.weave import converters
from scipy.optimize import leastsq

from OPT_recon.libprojections import *
from OPT_recon.libimagefile import *

import warnings
warnings.simplefilter("ignore", ComplexWarning)
warnings.filterwarnings("ignore",category=DeprecationWarning)

valid_dimensions = ("zspace","yspace","xspace")
valid_minc_datatypes = ("byte","short","int","long","float","double")
valid_recon_filters = ( "abs_bandlimit","abs_sinc","abs_cosine","abs_hamming","abs_hanning","shepp",
    "bandlimit", "sinc", "cosine", "triangle", "hamming", "hanning" )

def cleanup(d):
  '''Deletes the file or directory passed as argument.'''
  if(os.path.isdir(d)):
    shutil.rmtree(d)
  else:
    os.remove(d)

## General utilities

def IsEven(d):
  '''Returns 1 if d is even, 0 if it is odd.
  '''
  return (not (d%2))

## Numeric utilities

def gaussian_err_calc(parameters, y, x):
  '''Takes the tuple of parameters (a,b,c,d) and returns the error between 
  the data points y and the calculated fit for data points x.'''
  a, b, c, d = parameters
  err = y -  float(c)* exp(-((x-float(a))/float(b))**2) - float(d)
  return err

# Normalized 2D cross-correlation routine
def viewalias(mat, fact):
  '''Interpolates along the first dimension by factor sz.  
  Does this by zero padding the Fourier space.'''
  if(len(shape(mat)) != 2):
    print "I can only do 2d matrices"
    return mat

  if(fact == 1):
    return mat
  (views, dets) = shape(mat)
  newviews = fact*views
  b = zeros((newviews,dets),float32)
  for i in range(views):
    for j in range(fact):
      if(i<views-1):
        b[i*fact+j,:] = (1.0*(fact-j)/fact*mat[i,:]+1.0*(j)/fact*mat[i+1,:]).astype(float32)
      else:
        b[i*fact+j,:] = (1.0*(fact-j)/fact*mat[i,:]+1.0*(j)/fact*mat[0,:]).astype(float32)
    
  return b


def SafeDivide(numerator,denominator):
    '''A function which will do the divide without returning NaN.'''
    denom = where(equal(denominator,0),1,denominator)
    numer = where(equal(denominator,0),0,numerator)
    
    return numer / denom

def SafeSqrt(nums):
    '''A function which will do the sqrt without returning imaginary numbers.'''

    newnums = where(less(nums,0),0,nums)

    return sqrt(newnums)

## These functions are duplicated from MatLab normxcorr2

def local_sum(A,m,n):

    mA = shape(A)[0]
    nA = shape(A)[1]
    
    B = zeros((2*m + mA - 1, nA * 2 + n -1), float32)
    B[mA:mA+m,n:n+nA] = A.astype(float32);
    s = cumsum(B,0)
    c = s[m:,:] - s[:-m,:]
    s = cumsum(c,1)
    local_sum_A = s[:,n:] - s[:,:-n]
    
    return local_sum_A.astype(float32)

def xcorr2_fast(T,A):
    '''A quick cross correlation in fourier space.'''
    T_size = shape(T)
    A_size = shape(A)
    outsize = zeros(2)
    outsize[0] = A_size[0] + T_size[0] - 1
    outsize[1] = A_size[1] + T_size[1] - 1
    
    aa = freqxcorr2(T,A,outsize);
    
    return aa

def freqxcorr2(a,b,outsize):
    '''The frequency space 2d cross corr'''
    Fa = fft2(rot90(a,2), outsize)
    Fb = fft2(b,outsize)
    xcorr_ab = real(ifft2(Fa * Fb))
    return xcorr_ab


def normxcorr2(T,A):
    '''The normalized 2d cross corr'''
    xcorr_TA = xcorr2_fast(T,A);
    m = shape(T)[0]
    n = shape(T)[1]
    mn = m*n
    T = T.astype(float32)
    A = A.astype(float32)
    local_sum_A = local_sum(A,m,n)
    local_sum_A2 = local_sum(A*A,m,n)
    tmp = (local_sum_A2 - (local_sum_A ** 2)/mn ) / (mn - 1.)
    denom_A = sqrt(abs(tmp))
    denom_T = std(ravel(T))
    denom = denom_T*denom_A

    numerator = (xcorr_TA - local_sum_A * sum(ravel(T))/mn) / (mn -1)

    C = zeros(shape(numerator))
    C = SafeDivide(numerator,denom)

    return C

## Some OPT specific utilities
# Next, some pj/if utilities (file format from CTSim)
def create_phantom(fullpath,str):
  '''Creates a .phm file from the supplied phantom string and saves it in
  the specified output file.  Does not check for overwrite.'''
  f = open(fullpath,'w')
  f.writelines(str)
  f.close()

def get_pj_data(file):
  '''Gets the data in a .pj file'''
  pj = Projections()
  pj.read(file)
  return pj.get_data()

def write_pj_data(data,file):
  '''Takes a Numeric array and outputs it to a .pj file.'''
  pj = Projections()
  pj.read(file)
  pj.set_data(data)
  pj.write(file)

def get_if_data(file):
  '''Gets the data in a .if file'''
  iffile = ImageFile()
  iffile.fileRead(file)
  return iffile.get_data()

def correlate_images(im1, im2, blur=1):
  ''' returns a (col,row) tuple of shift necessary for image alignment'''    
  cols,rows = im1.shape
    
  # create a blurring kernel
  x, y = mgrid[-10:10,-10:10]
  r = sqrt(x**2 + y**2)
  lw = blur
  blurf = 1./(2*pi)*exp(-(r**2)/lw**2)

  print "Blurring image 1"
  im1 = convolve2d(im1,blurf,'same');
  print "Blurring image 2"
  im2 = convolve2d(im2,blurf,'same');

  print "Performing cross-correlation"
  my_result = normxcorr2(im1,im2)
  mymax = max(abs(ravel(my_result)))
  listlist = abs(ravel(my_result)).tolist()
  ind = listlist.index(mymax)
  a,b = divmod(ind,rows*2-1)
  col_offset = (a - shape(im1)[0]+1)
  row_offset = (b - shape(im1)[1]+1)
  
  return (col_offset, row_offset)

def get_coarse_offset(a,flip=True):
  '''Calculates a coarse offset of the rotational axis using normxcorr2.'''
  print "Calculating a coarse guess at the alignment"
  if (flip):
    views, dets, slices = a.data.count
    im1 = a.getHyperslab((0,0,0),(1,dets,slices))
    im1.shape=(dets,slices)
    im1 = transpose(im1)
    im2 = a.getHyperslab((views/2,0,0),(1,dets,slices))
    im2.shape=(dets,slices)
    im2 = transpose(im2)
  else:
    views, slices, dets = a.data.count
    im1 = a.getHyperslab((0,0,0),(1,slices,dets))
    im1.shape=(slices,dets)
    im2 = a.getHyperslab((views/2,0,0),(1,slices,dets))
    im2.shape=(slices,dets)
  
  im2 = fliplr(im2)

  return correlate_images(im1, im2, blur=12)[1]/2.0
  
  
def get_fine_offset(infile=False, slice=512, accuracy=0.2, start_offset=False, end_offset=False, plotfile=False , flip=True):
  '''Calculates a fine offset of the rotational axis from the center pixel by
  doing many sample recons of various potential offsets.  Takes the max as
  the correct offset, and returns it.'''
  if( (start_offset is False) or (end_offset is False)):
    raise TypeError, "The start_offset or end_offset was not defined"
    
  accuracy = abs(accuracy)
  nz,ny,nx = infile.data.count
  if(slice > ny or slice > nx):
    raise TypeError, "The stated slice is not within the bounds of slices: %d %d" % ny,nx
  first = start_offset
  last = end_offset
  ranges = arange(first,last,accuracy)

  if (flip):  
    inslice = infile.getHyperslab((0,0,slice),(nz,ny,1))
    inslice.shape =(nz,ny)
    maxdets = int(ceil(ny+2*max(abs(first),abs(last))))
    dets=ny
    slices=nx
  else:
    inslice = infile.getHyperslab((0,slice,0),(nz,1,nx))
    inslice.shape =(nz,nx)
    maxdets = int(ceil(nx+2*max(abs(first),abs(last))))
    dets=nx
    slices=ny

  b = zeros((ranges.size))
  j=0
  for i in ranges:
    outslice = reconstruct(inslice, nz,slices,dets, offset=i,reconsize=(maxdets,maxdets))
    #print outslice.shape
    w = ndarray(shape=outslice.shape, dtype=float)
    for p in range(outslice.shape[0]):
      for q in range(outslice.shape[1]):
        w[p][q] = outslice[p][q]

    b[j]=w.std()
    j=j+1
       
  ind = b.argmax()
  return(ranges[ind])

def reconstruct(inslice, views, slices, dets, offset=0, reconsize = "", 
                pjFOV=1.0, ROI="", offsetview=0, antialias=1, filt="abs_hanning"):

  '''Performs parallel-ray FBP on a sinogram infile and outputs it to outfile.    
  Defaults
  --------
  offset (detector offset of rotational axis): 0
  reconsize (number of pixels to reconstruct, format XxY): dets + 2*offset
  pjFOV (field of view of projections): 1.0
  ROI (what portion of the image to reconstruct): full
  offsetview (offset number of views from zero degrees): 0
  antialias (factor to antialias views) : 1
  filt (filter to use in recon) : abs_hanning
  '''

  ## First set some defaults
  workdir = tempfile.mkdtemp() 
  phantom = "%s/phantom.phm" % workdir
  proj = "%s/projections.pj" % workdir
  reconim = "%s/outfile.if" % workdir
  

  newviews = views * antialias
  detInc = pjFOV*2/dets
  dts = dets + abs(offset*2) + 10
     
  dets1 = (dts)*sqrt(2)
  FOV1 = pjFOV*dts/dets
     
  dets2 = int(ceil(dets1))
    
  if((IsEven(dets) and not IsEven(dets2)) or (not IsEven(dets) and IsEven(dets2))):
    dets2 = dets2 + 1
    
  newFOV = FOV1 * dets2/dets1
    
  # Create the phantom
  ph = "ellipse 0 0 %f %f 0 1" % (newFOV, newFOV)
  create_phantom(phantom, ph)

  #locate the latest ctsim
  f=findFile('ctsimtext')
  
  # Simulate projections
  cmd = "%s phm2pj %s %d %d --offsetview %d --rotangle -1 --phmfile %s" % (f,proj, dets2, newviews, offsetview, phantom)
  
  p= subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, 
                            stdin=subprocess.PIPE, 
                            stderr=subprocess.PIPE,close_fds=True)
  stdout, stderr = p.communicate()
  print cmd
  
  # Stuff the right data in the projection file
  pjdat = get_pj_data(proj)
    
  pjdat = zeros(shape(pjdat),float32)
  
  # Note: These will always be even, as it is either odd-odd or even-even
  left = (dets2 - dets)/2  
  right = (dets2 + dets)/2
    
  # Now interpolate out the data
  newdat = viewalias(squeeze(inslice),antialias)
  pjdat[:,left:right] = newdat.astype(float32)
  write_pj_data(pjdat,proj)
    
  nx = reconsize[0]
  ny = reconsize[1]
  
    
  if(not ROI):
    sx=-detInc*nx/2.
    ex=detInc*nx/2.
    sy=-detInc*ny/2.
    ey=detInc*ny/2.
  else:
    sx = float(ROI[0])
    ex = float(ROI[1])
    sy = float(ROI[2])
    ey = float(ROI[3])
    
  # Reconstruct the projection file
  cmd = "%s pjrec %s %s %d %d --offset %f --roi %f,%f,%f,%f --filter %s" % (f,proj, reconim, nx, ny, offset, sx,ex,sy,ey,filt)
  p= subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, 
                            stdin=subprocess.PIPE, 
                            stderr=subprocess.PIPE,close_fds=True)  
  stdout, stderr = p.communicate()
  print cmd

  outslice = get_if_data(reconim)

  #cleanup(workdir)

  return outslice
