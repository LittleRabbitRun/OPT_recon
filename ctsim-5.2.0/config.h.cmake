/* Define if building universal (internal helper macro) */
#cmakedefine01 AC_APPLE_UNIVERSAL_BUILD

/* Define to 1 if you have the <assert.h> header file. */
#cmakedefine01 HAVE_ASSERT_H

/* Define to 1 if you have the `basename' function. */
#cmakedefine01 HAVE_BASENAME

/* ctn dicom library */
#cmakedefine HAVE_CTN_DICOM

/* Define to 1 if you have the <ctype.h> header file. */
#cmakedefine01 HAVE_CTYPE_H

/* dmalloc library */
#cmakedefine01 HAVE_DMALLOC

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
#cmakedefine01 HAVE_DOPRNT

/* efence library */
#cmakedefine01 HAVE_EFENCE

/* Define to 1 if you have the <fcntl.h> header file. */
#cmakedefine01 HAVE_FCNTL_H

/* FFTW library */
#cmakedefine01 HAVE_FFTW

/* Define to 1 if you have the `getenv' function. */
#cmakedefine01 HAVE_GETENV

/* Define to 1 if you have the <getopt.h> header file. */
#cmakedefine01 HAVE_GETOPT_H

/* getopt_long library */
#cmakedefine01 HAVE_GETOPT_LONG

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine01 HAVE_GETTIMEOFDAY

/* Define to 1 if you have the `htonl' function. */
#cmakedefine01 HAVE_HTONL

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine01 HAVE_INTTYPES_H

/* Define to 1 if you have the `m' library (-lm). */
#cmakedefine01 HAVE_LIBM

/* Define to 1 if you have the <math.h> header file. */
#cmakedefine01 HAVE_MATH_H

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine01 HAVE_MEMORY_H

/* Define to 1 if you have the <netinet/in.h> header file. */
#cmakedefine01 HAVE_NETINET_IN_H

/* PNG library */
#cmakedefine01 HAVE_PNG

/* Readline library */
#cmakedefine01 HAVE_READLINE

/* Define to 1 if you have the <readline.h> header file. */
#cmakedefine HAVE_READLINE_H

/* Define to 1 if you have the <readline/readline.h> header file. */
#cmakedefine HAVE_READLINE_READLINE_H

/* Define to 1 if you have the `setjmp' function. */
#cmakedefine01 HAVE_SETJMP

/* Define to 1 if you have the <setjmp.h> header file. */
#cmakedefine01 HAVE_SETJMP_H

/* Define to 1 if you have the `setpriority' function. */
#cmakedefine01 HAVE_SETPRIORITY

/* use sgp library */
#cmakedefine HAVE_SGP

/* Define to 1 if you have the `snprintf' function. */
#cmakedefine01 HAVE_SNPRINTF

/* sstream header */
#cmakedefine01 HAVE_SSTREAM

/* Define to 1 if you have the <stdarg.h> header file. */
#cmakedefine01 HAVE_STDARG_H

/* Define to 1 if you have the <stddef.h> header file. */
#cmakedefine01 HAVE_STDDEF_H

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine01 HAVE_STDINT_H

/* Define to 1 if you have the <stdio.h> header file. */
#cmakedefine01 HAVE_STDIO_H

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine01 HAVE_STDLIB_H

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine01 HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine01 HAVE_STRING_H

/* Define to 1 if you have the `strtod' function. */
#cmakedefine01 HAVE_STRTOD

/* Define to 1 if you have the `strtol' function. */
#cmakedefine01 HAVE_STRTOL

/* Define to 1 if you have the <sys/fcntl.h> header file. */
#cmakedefine01 HAVE_SYS_FCNTL_H

/* Define to 1 if you have the <sys/param.h> header file. */
#cmakedefine01 HAVE_SYS_PARAM_H

/* Define to 1 if you have the <sys/resource.h> header file. */
#cmakedefine01 HAVE_SYS_RESOURCE_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine01 HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine01 HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine01 HAVE_SYS_TYPES_H

/* Define to 1 if you have the `time' function. */
#cmakedefine01 HAVE_TIME

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine01 HAVE_UNISTD_H

/* Define to 1 if you have the `usleep' function. */
#cmakedefine01 HAVE_USLEEP

/* Define to 1 if you have the `vprintf' function. */
#cmakedefine01 HAVE_VPRINTF

/* Define to 1 if you have the `vsnprintf' function. */
#cmakedefine01 HAVE_VSNPRINTF

/* Define to 1 if you have the `vsprintf' function. */
#cmakedefine01 HAVE_VSPRINTF

/* have wxthreads library */
#cmakedefine HAVE_WXTHREADS

/* wxwindows library */
#cmakedefine HAVE_WXWINDOWS

/* X11 system */
#cmakedefine01 HAVE_X11


/* version information */
#define PACKAGE_NAME "@PACKAGE_NAME@"
#define PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"
#define PACKAGE_VERSION "@PACKAGE_VERSION@"
#define PACKAGE_STRING "@PACKAGE_STRING@"
#define VERSION PACKAGE_VERSION

/* Name of package */
#define PACKAGE "@PACKAGE@"


/* The size of `double', as computed by sizeof. */
#define SIZEOF_DOUBLE @SIZEOF_DOUBLE@

/* The size of `float', as computed by sizeof. */
#define SIZEOF_FLOAT @SIZEOF_FLOAT@

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT @SIZEOF_INT@

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG @SIZEOF_LONG@

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT @SIZEOF_SHORT@

/*TODO: figure this out*/
/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
#cmakedefine01 TM_IN_SYS_TIME


/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif

/* Define to 1 if the X Window System is missing or not being used. */
#cmakedefine01 X_DISPLAY_MISSING


/*TODO: figure out these */
/* Define to empty if `const' does not conform to ANSI C. */
//#cmakedefine01 const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
//#ifndef __cplusplus
//#cmakedefine01 inline
//#endif

/* Define to `long int' if <sys/types.h> does not define. */
//#cmakedefine01 off_t

/* Define to `unsigned int' if <sys/types.h> does not define. */
//#cmakedefine01 size_t
