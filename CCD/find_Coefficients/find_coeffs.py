
from pyminc.volumes.factory import *
from numpy import *
from scipy import *
from scipy.linalg import *
from string import *

# read text file with all information
f = open('all_gf_R.txt','r')
lines=f.readlines()
f.close()

number_of_files = len(lines) - 1

# create the exposure_time array and the filenames array
filenames=[]
d=[]
for i in range(1,len(lines)):
    filenames.append((lines[i].split(','))[0])
    d.append(atoi((lines[i].split(','))[1]))

d = array(d)	
q = ones(number_of_files)

d = reshape(d,(d.shape[0],1))
q = reshape(q,(q.shape[0],1))
A = concatenate([q,d,d**2],axis=1)

volhandles = []
for i in range(0,number_of_files):
    volhandles.append(volumeFromFile(filenames[i], dtype='ushort'))
#array of voxel values
image_dim1 = volhandles[0].sizes[1]
image_dim2 = volhandles[0].sizes[2]

p = zeros((number_of_files,image_dim2))
coefs=zeros((3,image_dim1*image_dim2))
counter=0
for i in range(0,image_dim2):
    for j in range(0,number_of_files):
    	p[j,:] = volhandles[j].getHyperslab((0,i,0),(1,1,image_dim1))
    for k in range(0,image_dim1):
	b = p[:,k]
	[x, resid, rank, s] = lstsq(A,b)
	coefs[0,counter]=x[0]
	coefs[1,counter]=x[1]
	coefs[2,counter]=x[2]
	counter = counter + 1

f = file("coeffs.txt","w")

for i in range(0,image_dim1*image_dim2):
    print >> f, coefs[0,i], coefs[1,i], coefs[2,i]

f.close()


f=file("coeffs_columns.txt","w")
for i in range(0,3):
    print >> f, join(map(str,coefs[i,:]))

f.close()
