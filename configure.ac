AC_PREREQ(2.61)

dnl Process this file with autoconf to produce a configure script.
dnl
dnl immediately before every release do:
dnl ===================================
dnl if (the interface is totally unchanged from previous release)
dnl    *_lt_revision ++;
dnl else { /* interfaces have been added, removed or changed */
dnl    *_lt_revision = 0;
dnl    *_lt_current ++;
dnl    if (any interfaces have been _added_ since last release)
dnl       *_lt_age ++;
dnl    if (any interfaces have been _removed_ or _incompatibly changed_)
dnl       *_lt_age = 0;
dnl }
dnl
dnl If you want to know more about what you are doing, here are some details:
dnl  * *_lt_current is the current API version
dnl  * *_lt_revision is an internal revision number which is increased when
dnl    the API itself did not change
dnl  * *_lt_age is the number of previous API versions still supported by
dnl    this library
dnl  * libtool has its own numbering scheme, because local library numbering
dnl    schemes are platform dependent
dnl  * in Linux, the library will be named
dnl    libname.so.(*_lt_current - *_lt_age).*_lt_age.*_lt_revision

m4_define([nemesi_major_version], [0])
m4_define([nemesi_minor_version], [1])
m4_define([nemesi_micro_version], [1])
m4_define([nemesi_interface_age], [0])
m4_define([nemesi_binary_age],
          [m4_eval(100 * nemesi_minor_version + nemesi_micro_version)])
m4_define([nemesi_version],
          [nemesi_major_version.nemesi_minor_version.nemesi_micro_version])

m4_define([nemesi_lt_revision], [0])
m4_define([nemesi_lt_current], [1])
m4_define([nemesi_lt_age],     [0])

AC_INIT([libnemesi], [nemesi_version], [https://live.polito.it/roundup/lscube])

AC_CONFIG_AUX_DIR(config)
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip foreign subdir-objects])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE

AC_PREFIX_DEFAULT("/usr")

AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
  AC_MSG_ERROR([no C99 compiler found, feng requires a C99 compiler.])
])

AC_USE_SYSTEM_EXTENSIONS
dnl Force usage of POSIX.1-2001 (clock_gettime)
dnl Force usage of _XOPEN_SOURCE (strdup)
CPPFLAGS="${CPPFLAGS} -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600"

AC_CONFIG_HEADER([config.h])

AC_ARG_ENABLE(examples,
	AS_HELP_STRING([--disable-examples], [Don't build examples during make]),,
	enable_examples="yes")

dnl Check for programs.
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_SUBST([LIBTOOL_DEPS])

dnl Check for pthread libraries.
ACX_PTHREAD(,AC_MSG_ERROR([Missing POSIX Threads support]))

CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
LIBS="$LIBS $PTHREAD_LIBS"

dnl Check for Endianess
AC_C_BIGENDIAN([BIGENDIAN="Big Endian"] AC_DEFINE([WORDS_BIGENDIAN], 1, [Define if manchine is big-endian]),[BIGENDIAN="Little Endian"] )

dnl Check for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(sys/time.h unistd.h strings.h errno.h fcntl.h limits.h malloc.h byteswap.h)


dnl Check for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_C_INLINE
AC_HEADER_TIME

dnl Check for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS(select socket gettimeofday uname getcwd getwd strcspn strdup strtoul strerror strstr setenv nanosleep strdup)
AC_CHECK_FUNC(getaddrinfo)
AC_CHECK_LIBM

dnl check if we have the generic struct for net addresses the has max possible size
AC_CHECK_TYPES(struct sockaddr_storage,
            [EXTRA_FLAGS="-DHAVE_STRUCT_SOCKADDR_STORAGE ${EXTRA_FLAGS}"],
            ,
            [#include <sys/socket.h>])


LSC_CHECK_IPV6
LSC_CHECK_SCTP
LIBS="$LIBS $SCTP_LIBS"

AS_IF([test "x$have_sctp" = "xyes"],
  [LIBNETEMBRYO_REQUIRES="libnetembryo-sctp >= 0.1.1"],
  [LIBNETEMBRYO_REQUIRES="libnetembryo >= 0.1.1"])

PKG_CHECK_MODULES(LIBNETEMBRYO,[$LIBNETEMBRYO_REQUIRES])
AC_SUBST(LIBNETEMBRYO_REQUIRES)

CFLAGS="$CFLAGS $LIBNETEMBRYO_CFLAGS"
LIBS="$LIBS $LIBNETEMBRYO_LIBS"

AC_SUBST(EXTRA_FLAGS)

AM_CONDITIONAL(EXAMPLES, [test "$enable_examples" = "yes"])

LSC_ERRORS
LSC_DEBUG

dnl libtool nonsense
LT_CURRENT=nemesi_lt_current
LT_REVISION=nemesi_lt_revision
LT_AGE=nemesi_lt_age

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_CONFIG_FILES([Makefile
                 libnemesi.pc])
AC_OUTPUT

LSC_DEBUG_STATUS
AC_MSG_NOTICE([config files successfully generated!

libnemesi version is .............. : $VERSION

libnemesi Install prefix .......... : $prefix

libnemesi has SCTP support ........ : $have_sctp

Byte order ..................... : this is a $BIGENDIAN machine

 'make' will now compile libnemesi and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
