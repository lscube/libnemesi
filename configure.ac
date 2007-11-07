dnl Process this file with autoconf to produce a configure script.

m4_define([nemesi_major_version], [0])
m4_define([nemesi_minor_version], [6])
m4_define([nemesi_micro_version], [4])
m4_define([nemesi_interface_age], [0])
m4_define([nemesi_binary_age],
          [m4_eval(100 * nemesi_minor_version + nemesi_micro_version)])
m4_define([nemesi_version],
          [nemesi_major_version.nemesi_minor_version.nemesi_micro_version])
m4_define([nemesi_lt_current],
          [m4_eval(100 * nemesi_minor_version + nemesi_micro_version - nemesi_interface_age)])
m4_define([nemesi_lt_revision], [nemesi_interface_age])
m4_define([nemesi_lt_age], [m4_eval(nemesi_binary_age - nemesi_interface_age)])

AC_INIT(libnemesi, [nemesi_version], [https://live.polito.it/roundup/lscube])

AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(dist-bzip2 check-news)
AC_PREFIX_DEFAULT("/usr")

AM_CONFIG_HEADER(config.h)

AC_ARG_ENABLE(debug,
[  --enable-debug          enable gcc dubugging flags [[default=no]]],,
	enable_debug="no")
AC_ARG_ENABLE(errors,
[  --enable-errors         make gcc warnings behave like errors: none, normal, pedantic [[default=none]]],,
	enable_errors="none")
AC_ARG_ENABLE(ipv6,
[  --enable-ipv6            enable IPv6 support [[default=autodetected]]],,
	enable_ipv6="yes")
AC_ARG_ENABLE(sctp,
[  --enable-sctp            enable SCTP support [[default=autodetected]]],,
	enable_sctp="yes")

dnl Check for programs.
m4_undefine([AC_PROG_CXX])
m4_defun([AC_PROG_CXX],[])
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])

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
AC_CHECK_HEADERS(sys/time.h unistd.h strings.h errno.h fcntl.h limits.h malloc.h)

dnl Check for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_C_INLINE
AC_HEADER_TIME

dnl Check for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(select socket gettimeofday uname getcwd getwd strcspn strdup strtoul strerror strstr setenv nanosleep strdup)
AC_CHECK_FUNC(getaddrinfo)
AC_CHECK_LIBM

dnl check if we have the generic struct for net addresses the has max possible size
AC_CHECK_TYPES(struct sockaddr_storage,
            [EXTRA_FLAGS="-DHAVE_STRUCT_SOCKADDR_STORAGE ${EXTRA_FLAGS}"],
            ,
            [#include <sys/socket.h>])

dnl Check for IPv6 support
if test "$enable_ipv6" = "yes"; then
	AC_CHECK_TYPE(struct sockaddr_in6, AC_DEFINE([IPV6], 1, [Define IPv6 support]), , [#include <netinet/in.h>])
else
	AC_MSG_WARN([IPv6 disabled: check skipped.])
fi


PKG_CHECK_MODULES(LIBNETEMBRYO,[libnetembryo >= 0.0.4])
CFLAGS="$CFLAGS $LIBNETEMBRYO_CFLAGS"
LIBS="$LIBS $LIBM $LIBNETEMBRYO_LIBS"

dnl Check for SCTP support
if test "$enable_sctp" = "yes"; then
    if test "${LIBNETEMBRYO_LIBS//sctp}" = "${LIBNETEMBRYO_LIBS}"; then
        AC_MSG_WARN([sctp required but netembryo does not support it, rebuild netembryo first.])
    fi
        AC_CHECK_TYPE(struct sctp_sndrcvinfo, [
         AC_CHECK_LIB(sctp,sctp_recvmsg)],
         [enable_sctp=no,
         AC_MSG_WARN([SCTP disabled: headers not found])],
         [#include <netinet/sctp.h>])
else
    AC_MSG_WARN([SCTP disabled: check skipped.])
fi

AC_SUBST(EXTRA_FLAGS)

AM_CONDITIONAL(NEMESI_DEBUG, [test "$enable_debug" = "yes"])
if test "$enable_debug" = "yes"; then
	CFLAGS="$CFLAGS -g -ggdb -Wall"
	AC_DEFINE(ENABLE_DEBUG, 1,[Debug enabled])
	debug_msg="
   Debuging flags enabled: welcome developer!!!"
else
        CFLAGS="$CFLAGS -Wall"
fi

case "$enable_errors" in
	pedantic)
		CFLAGS="$CFLAGS -pedantic-errors -Werror"
	;;
	normal | yes)
		CFLAGS="$CFLAGS -Werror"
	;;
	none | *)
		CFLAGS="$CFLAGS"
	;;
esac

dnl libtool nonsense
LT_CURRENT=nemesi_lt_current
LT_REVISION=nemesi_lt_revision
LT_AGE=nemesi_lt_age

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_CONFIG_FILES([Makefile
                 bufferpool/Makefile
                 creativecommons/Makefile
                 rtp/Makefile
                 parsers/Makefile
                 rtcp/Makefile
                 sdp/Makefile
                 rtsp/Makefile
                 utils/Makefile
                 example/Makefile
                 libnemesi.pc])
AC_OUTPUT

AC_MSG_NOTICE([config files successfully generated!
 
libnemesi version is .............. : $VERSION

libnemesi Install prefix .......... : $prefix

libnemesi has SCTP support ........ : $enable_sctp

Byte order ..................... : this is a $BIGENDIAN machine

$debug_msg

 'make' will now compile libnemesi and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
