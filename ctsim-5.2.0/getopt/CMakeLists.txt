INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}") # a hack to get to config.h becaus it is included as ../config.h

ADD_LIBRARY(getopt getopt.c getopt1.c  getopt.h)
