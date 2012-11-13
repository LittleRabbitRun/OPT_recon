from pyminc.volumes.factory import *
from numpy import *
import scipy
from scipy import interpolate, fftpack
import py_minc

sizes=(1024,400,1024)
steps=(2.45,2.45,2.45)
starts=(0,0,0)
centre=(1254.4,490,1254.4)

img = py_minc.ArrayVolume((sizes[0], sizes[2], sizes[1]), dimension_names = ('zspace', 'xspace', 'lspace'), 
                        nc_data_type = py_minc.NC_SHORT, typecode = float_)
img.set_starts((starts[0], starts[2], starts[1]))
img.set_separations((steps[0], steps[2], steps[1]))
width = 5.0
depth_dependent_width = 0.05
                  
# iterate over voxels
for k in range(sizes[0]):
    z = starts[0] + k*steps[0]
    dz = z -  centre[0]

    for j in range(sizes[1]):
        l = starts[1] + j*steps[1]
        dl = l - centre[1] # coordinate relative to focal plane

        combined_width_sq = width**2 + (dl*depth_dependent_width)**2
        c = 1.0/sqrt(2*pi*combined_width_sq)

        for i in range(sizes[2]):
            x = starts[2] + i*steps[2]
            dx = x - centre[2]   # coordinate relative to centre
           
            img.array[k,i,j] = c*exp(-.5*(dx**2+dz**2)/combined_width_sq) # evaluate gaussian
print "finished creating array now writing"
#psf=array(img.array)
#rev_psf=psf[::-1]
#rev_psf=rot90(rot90(psf))
#print rev_psf.shape
outfile = volumeFromDescription('simulated_3d_psf.mnc',('xspace', 'yspace', 'zspace'), (sizes[0], sizes[2], sizes[1]),(starts[0],starts[2],starts[1]),(steps[0], steps[2],steps[1]),dtype='float',volumeType='short')
outfile.data=img.array
outfile.writeFile()
outfile.closeVolume()


