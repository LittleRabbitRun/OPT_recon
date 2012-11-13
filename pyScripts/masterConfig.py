import os,sys

# First, some shell utilities

def _find(pathname, matchFunc=os.path.isfile):
    for dirname in sys.path:
        candidate = os.path.join(dirname, pathname)
        if matchFunc(candidate):
            return candidate
    raise IOError, "Can't find file %s" % pathname

def findFile(pathname):
    return _find(pathname)

def makeDir(path):
    if not path:
        return os.getcwd()+'/'
    elif (path[-1] != '/'):
        return path+'/'
    else:
        return path
