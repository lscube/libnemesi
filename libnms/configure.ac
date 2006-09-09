dnl Process this file with autoconf to produce a configure script.
AC_INIT(libnms, 0.6.0)
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(dist-bzip2 check-news)

AC_PREFIX_PROGRAM(libnms)

test "x$prefix" = x. || test "x$prefix" = xNONE && prefix=/usr/local
test "x$exec_prefix" = xNONE && exec_prefix=$prefix
test "x$sysconfdir" = 'x${prefix}/etc' && sysconfdir="/etc"
test "x$localstatedir" = 'x${prefix}/var' && localstatedir="/var"
test "x$mandir" = 'x${prefix}/man' && mandir="${prefix}/share/man"

AM_CONFIG_HEADER(config.h)

AC_ARG_ENABLE(debug,
[  --enable-debug          enable gcc dubugging flags [[default=no]]],,
	enable_debug="no")
AC_ARG_ENABLE(efence,
[  --enable-efence         link against Electric Fence library to debug memory usage
                          (this will involve --enable-debug) [[default=no]]],,
	enable_efence="no")
AC_ARG_ENABLE(warnings,
[  --enable-warnings       enable/choose gcc warnings level: none, normal, pedantic [[default=none]]],,
	enable_warnings="none")
AC_ARG_ENABLE(errors,
[  --enable-errors         make gcc warnings behave like errors: none, normal, pedantic [[default=none]]],,
	enable_errors="none")
AC_ARG_ENABLE(optimize,
[  --enable-optimize       enable/chose optimization level: none, poor, normal, full, super [[default=normal]]],,
	enable_optimize="normal")
AC_ARG_ENABLE(ipv6,
[  --enable-ipv6            enable IPv6 support [[default=autodetected]]],,
	enable_ipv6="yes")
AC_ARG_ENABLE(sctp,
[  --enable-sctp            enable SCTP support [[default=autodetected]]],,
	enable_sctp="yes")

dnl Check for programs.
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

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

dnl AC_CHECK_LIB(pthread,pthread_create)

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
AC_CHECK_FUNCS(select socket gettimeofday uname getcwd getwd strcspn strdup strtoul strerror strstr setenv)
AC_CHECK_FUNC(getaddrinfo)
AC_CHECK_LIBM

dnl check if we have the generic struct for net addresses the has max possible size
AC_CHECK_TYPES(struct sockaddr_storage,,, [#include <sys/socket.h>])
dnl AC_CHECK_MEMBERS(struct sockaddr_in.sin_len,,, [#include <netinet/in.h>])
EXTRA_FLAGS="-DHAVE_STRUCT_SOCKET_STORAGE ${EXTRA_FLAGS}"
dnl CFLAGS=""

dnl Check for IPv6 support
if test "$enable_ipv6" = "yes"; then
	AC_CHECK_TYPE(struct sockaddr_in6, AC_DEFINE([IPV6], 1, [Define IPv6 support]), , [#include <netinet/in.h>])
else
	AC_MSG_WARN([IPv6 disabled: check skipped.])
	dnl AM_CONDITIONAL(NEMESI_HAVE_IPV6, /bin/false)
	dnl NEMESI_HAVE_IPV6=no
fi

dnl Check for SCTP support
if test "$enable_sctp" = "yes"; then
	AC_CHECK_TYPE(struct sctp_sndrcvinfo, AC_DEFINE([HAVE_SCTP], 1, [Define SCTP support]), , [#include <netinet/sctp.h>])
else
	AC_MSG_WARN([SCTP disabled: check skipped.])
	dnl AM_CONDITIONAL(NEMESI_HAVE_SCTP, /bin/false)
	dnl NEMESI_HAVE_SCTP=no
fi

AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

AC_SUBST(LIBM)
AC_SUBST(EXTRA_FLAGS)
AC_SUBST(PTHREADS_LIBS)
AC_SUBST(PTHREADS_CFLAGS)

AM_CONDITIONAL(NEMESI_EFENCE, /bin/false)
if test "$enable_efence" = "yes"; then
	AM_CONDITIONAL(NEMESI_EFENCE, /bin/true)
	AC_DEFINE(ENABLE_EFENCE, 1,[Debug enabled])
	enable_debug=yes
	efence_msg="   You chose to compile against Electric Fence lib"
fi

AM_CONDITIONAL(NEMESI_DEBUG, /bin/false)
if test "$enable_debug" = "yes"; then
	CFLAGS="$CFLAGS -g -ggdb"
	AM_CONDITIONAL(NEMESI_DEBUG, /bin/true) \
	AC_DEFINE(ENABLE_DEBUG, 1,[Debug enabled])
	debug_msg="
   Debuging flags enabled: welcome developer!!!"
fi

case "$enable_warnings" in
	pedantic)
		CFLAGS="$CFLAGS -W -Wall -pedantic"
	;;
	normal | yes)
		CFLAGS="$CFLAGS -W -Wall"
	;;
	none | *)
		CFLAGS="$CFLAGS"
	;;
esac

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

case "$enable_optimize" in
	poor)
		CFLAGS="$CFLAGS -O1" 
	;;
	full)
		CFLAGS="$CFLAGS -O3"
	;;
	super)
		CFLAGS="$CFLAGS -O3 -funroll-loops -fstrength-reduce -fomit-frame-pointer -ffast-math -malign-functions=2 -malign-jumps=2 -malign-loops=2"
	;;
	none)
		CFLAGS="$CFLAGS"
	;;
	normal | *)
		CFLAGS="$CFLAGS -O2"
	;;
esac


ARCH_DEFINES=""
case "$host" in 
	i386-*-* | i486-*-* | i586-*-* | i686-*-* | i86pc-*-*)
		dnl CFLAGS="$CFLAGS -mcpu=i486"
		CFLAGS="$CFLAGS -march=i386"
		ARCH_DEFINES="-DI386_ASSEM"
		arch_type=ix86
	;;
	ppc-*-*)
		arch_type=ppc
	;;
	*-hpux-*)
		ARCH_DEFINES="-DHPUX"
	;;
	*-sun-*)
		have_solaris=yes
	;;
esac


AC_SUBST(ARCH_DEFINES)
AM_CONDITIONAL(ARCH_X86, test "x$arch_type" = "xix86")
AM_CONDITIONAL(ARCH_PPC, test "x$arch_type" = "xppc")
AM_CONDITIONAL(HAVE_SOLARIS, test "x$have_solaris" = xyes)

dnl nemesi_bin=$PACKAGE_NAME
dnl AC_SUBST(nemesi_bin)
dnl nemesi_gui=g$nemesi_bin
dnl AC_SUBST(nemesi_gui)

libnms_docsdir=$datadir/doc/$PACKAGE_NAME
AC_SUBST(libnms_docsdir)

libnms_libdir="$prefix"/lib
AC_SUBST(libnms_libdir)
dnl nemesi_plugindir="$nemesi_libdir"/plugins
dnl AC_SUBST(nemesi_plugindir)

libnms_sharedir="$prefix"/share/$PACKAGE_NAME
AC_SUBST(libnms_sharedir)
dnl nemesi_throbberdir="$nemesi_sharedir"/throbber
dnl AC_SUBST(nemesi_throbberdir)
dnl nemesi_pixmapdir="$nemesi_sharedir"/pixmaps
dnl AC_SUBST(nemesi_pixmapdir)

if test -f svnrev.h; then
	AC_DEFINE(NEMESI_SVN, 1,[Define if this is a development (subversion) tree])
	REVISION=`sed -n -e 's/\(.*Rev: \)\([[0-9]]\+\)\(.*\)/\2/p' svnrev.h`
fi

dnl AC_DEFINE_UNQUOTED(NEMESI_PLUGIN_DIR_DEFAULT, "$nemesi_plugindir", [Define default directory for NeMeSI plugins])
dnl AC_DEFINE(NEMESI_PLUGIN_DIR_ENV, "NEMESI_PLUGIN_DIR",[Define the name for the ENV variable forn NeMeSI plugins dir])

dnl AC_DEFINE_UNQUOTED(NEMESI_THROBBER_DIR_DEFAULT, "$nemesi_throbberdir", [Define default directory for NeMeSI throbber])
dnl AC_DEFINE(NEMESI_THROBBER_DIR_ENV, "NEMESI_THROBBER_DIR",[Define the name for the ENV variable forn NeMeSI throbber dir])

dnl AC_DEFINE_UNQUOTED(NEMESI_PIXMAPS_DIR_DEFAULT, "$nemesi_pixmapdir", [Define default directory for NeMeSI pixmaps])
dnl AC_DEFINE(NEMESI_PIXMAPS_DIR_ENV, "NEMESI_PIXMAP_DIR",[Define the name for the ENV variable forn NeMeSI pixmaps dir])

AC_CONFIG_FILES([Makefile
                 bufferpool/Makefile
                 libCCstreaming/Makefile
                 rtp/Makefile
                 rtp/rtpparsers/Makefile
                 rtcp/Makefile
                 sdp/Makefile
                 rtsp/Makefile
                 wsocket/Makefile
                 md5/Makefile
                 utils/Makefile
                 example/Makefile
                 libnms.pc])
AC_OUTPUT

AC_MSG_NOTICE([Release is .......... : $VERSION])

if test -f svnrev.h; then
AC_MSG_NOTICE([Revision is ......... : $REVISION
])
fi

AC_MSG_NOTICE([config files successfully generated!
 
libnms version is .............. : $VERSION

libnms Install prefix .......... : $prefix
libnms Lib path ................ : $libnms_libdir

Byte order ..................... : this is a $BIGENDIAN machine

$debug_msg
$efence_msg

 'make' will now compile NeMeSI and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
