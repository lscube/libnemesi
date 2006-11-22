dnl Process this file with autoconf to produce a configure script.

AC_INIT(nemesi, 0.6.0)
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(dist-bzip2 check-news)

AC_PREFIX_PROGRAM(nemesi)

test "x$prefix" = x. || test "x$prefix" = xNONE && prefix=/usr/local
test "x$exec_prefix" = xNONE && exec_prefix=$prefix
test "x$libdir" = 'x${exec_prefix}/lib' && libdir="${exec_prefix}/lib"
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
AC_ARG_ENABLE(sdl,
[  --enable-sdl            enable/chose sdl as default output library [[default=autodetected]]],,
	enable_sdl="yes")
AC_ARG_ENABLE(avc,
[  --enable-avc            enable libavcodec support [[default=autodetected]]],,
	enable_avc="yes")
AC_ARG_ENABLE(gui,
[  --enable-gui            enable GUI [[default=autodetected]]],,
	enable_gui="yes")
dnl AC_ARG_ENABLE(ipv6,
dnl [  --enable-ipv6            enable IPv6 support [[default=autodetected]]],,
dnl 	enable_ipv6="yes")

dnl Check for programs.
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL
AC_PROG_CPP
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl Check for LibTool Dynamic Loading
AC_CHECK_LIB(ltdl,lt_dlopen,,\
AC_MSG_ERROR([
 *********************************************************************
 *                                                                   *
 *         NeMeSI needs LibTool's libltdl to support plug-ins.       *
 *                                                                   *
 *            Please install libtool or libtool-libs from            *
 *               http://www.gnu.org/software/libtool                 *
 *           or check for a version suitable to your distro.         *
 *                                                                   *
 *********************************************************************]))

m4_undefine([AC_PROG_CXX])
m4_defun([AC_PROG_CXX],[])
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])

AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_SUBST([LIBTOOL_DEPS])

dnl Check for pthread libraries.
AC_CHECK_LIB(pthread,pthread_create)

dnl Check for Endianess
AC_C_BIGENDIAN([BIGENDIAN="Big Endian"] AC_DEFINE([WORDS_BIGENDIAN], 1, [Define if manchine is big-endian]),[BIGENDIAN="Little Endian"] )

dnl Replace `main' with a function in -lm:
AC_CHECK_LIB(m, main)

dnl Check for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(sys/time.h unistd.h strings.h errno.h fcntl.h limits.h malloc.h sys/ioctl.h sys/soundcard.h soundcard.h)
AC_CHECK_HEADERS(ffmpeg/common.h ffmpeg/avcodec.h)

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

dnl check if we have the generic struct for net addresses the has max possible size
AC_CHECK_TYPES(struct sockaddr_storage,,, [#include <sys/socket.h>])
dnl AC_CHECK_MEMBERS(struct sockaddr_in.sin_len,,, [#include <netinet/in.h>])

dnl CFLAGS=""

dnl Check for IPv6 support
dnl if test "$enable_ipv6" = "yes"; then
	dnl AC_CHECK_TYPE(struct sockaddr_in6, AC_DEFINE([IPV6], 1, [Define IPv6 support]), , [#include <netinet/in.h>])
dnl else
	dnl AC_MSG_WARN([IPv6 disabled: check skipped.])
	dnl dnl AM_CONDITIONAL(NEMESI_HAVE_IPV6, /bin/false)
	dnl dnl NEMESI_HAVE_IPV6=no
dnl fi

dnl Check for SDL libraryISL3880
if test "$enable_sdl" = "yes"; then
	AM_PATH_SDL("1.2.2",
		AC_DEFINE([HAVE_SDL], 1, [Define if SDL library is present]) \
		AC_SUBST(SDL_CFLAGS) \
		AC_SUBST(SDL_LIBS) \
		AM_CONDITIONAL(NEMESI_HAVE_SDL, /bin/true) \
		NEMESI_HAVE_SDL=yes, \
		AC_MSG_WARN([Falling back to OSS Audio output!]) \
		AM_CONDITIONAL(NEMESI_HAVE_SDL, /bin/false) \
		NEMESI_HAVE_SDL=no)
else
	AC_MSG_WARN([SDL library disabled!])
	AM_CONDITIONAL(NEMESI_HAVE_SDL, /bin/false)
	NEMESI_HAVE_SDL=no
fi

dnl Check for GTK+ support
if test "$enable_gui" = "yes"; then
	pkg_modules="gtk+-2.0 >= 2.4.0"
	PKG_CHECK_MODULES(PACKAGE, [$pkg_modules], \
		AC_DEFINE([HAVE_GUI], 1, [Define if GUI is present]) \
		AM_CONDITIONAL(NEMESI_HAVE_GTK, /bin/true) \
		NEMESI_HAVE_GTK=yes, \
		AC_MSG_WARN([GUI will not be compiled!]) \
		AM_CONDITIONAL(NEMESI_HAVE_GTK, /bin/false) \
		NEMESI_HAVE_GTK=no)
else
	AC_MSG_WARN([GUI disabled!])
	AM_CONDITIONAL(NEMESI_HAVE_GTK, /bin/false)
	NEMESI_HAVE_GTK=no
fi
	
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)


dnl Check for FFMpeg libavcodec library
if test "x$enable_avc" = "xyes"; then
	AC_CHECK_LIB(avcodec,avcodec_open, \
	AM_CONDITIONAL(NEMESI_HAVE_AVC, /bin/true) \
	NEMESI_HAVE_AVC=yes, \
	AC_MSG_WARN([libavcodec not present!]) \
	AM_CONDITIONAL(NEMESI_HAVE_AVC, /bin/false) \
	NEMESI_HAVE_AVC=no)
else
	AC_MSG_WARN([libavcodec disabled!])
	AM_CONDITIONAL(NEMESI_HAVE_AVC, /bin/false)
	NEMESI_HAVE_AVC=no
fi


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

nemesi_bin=$PACKAGE_NAME
AC_SUBST(nemesi_bin)
nemesi_gui=g$nemesi_bin
AC_SUBST(nemesi_gui)

nemesi_docsdir="$datadir/doc/$PACKAGE_NAME"
AC_SUBST(nemesi_docsdir)

nemesi_libdir="$libdir/$PACKAGE_NAME"
AC_SUBST(nemesi_libdir)
nemesi_plugindir="$nemesi_libdir/plugins"
AC_SUBST(nemesi_plugindir)

nemesi_sharedir="$prefix/share/$PACKAGE_NAME"
AC_SUBST(nemesi_sharedir)
nemesi_throbberdir="$nemesi_sharedir/throbber"
AC_SUBST(nemesi_throbberdir)
nemesi_pixmapdir="$nemesi_sharedir/pixmaps"
AC_SUBST(nemesi_pixmapdir)

if test -f svnrev.h; then
	AC_DEFINE(NEMESI_SVN, 1,[Define if this is a development (subversion) tree])
	REVISION=`sed -n -e 's/\(.*Rev: \)\([[0-9]]\+\)\(.*\)/\2/p' svnrev.h`
	AC_SUBST(REVISION)
fi

AC_DEFINE_UNQUOTED(NEMESI_PLUGIN_DIR_DEFAULT, "$nemesi_plugindir", [Define default directory for NeMeSI plugins])
AC_DEFINE(NEMESI_PLUGIN_DIR_ENV, "NEMESI_PLUGIN_DIR",[Define the name for the ENV variable forn NeMeSI plugins dir])

AC_DEFINE_UNQUOTED(NEMESI_THROBBER_DIR_DEFAULT, "$nemesi_throbberdir", [Define default directory for NeMeSI throbber])
AC_DEFINE(NEMESI_THROBBER_DIR_ENV, "NEMESI_THROBBER_DIR",[Define the name for the ENV variable forn NeMeSI throbber dir])

AC_DEFINE_UNQUOTED(NEMESI_PIXMAPS_DIR_DEFAULT, "$nemesi_pixmapdir", [Define default directory for NeMeSI pixmaps])
AC_DEFINE(NEMESI_PIXMAPS_DIR_ENV, "NEMESI_PIXMAP_DIR",[Define the name for the ENV variable forn NeMeSI pixmaps dir])

AC_CONFIG_SUBDIRS(libnms)
AC_CONFIG_FILES([Makefile])
dnl AC_CONFIG_FILES([libnms/bufferpool/Makefile])
AC_CONFIG_FILES([etui/Makefile])

dnl if test x$NEMESI_HAVE_GTK = xyes; then
AC_CONFIG_FILES([egui/Makefile])
dnl fi

dnl AC_CONFIG_FILES([libnms/libCCstreaming/Makefile])
dnl AC_CONFIG_FILES([libnms/rtp/Makefile])
dnl AC_CONFIG_FILES([libnms/rtcp/Makefile])
dnl AC_CONFIG_FILES([libnms/sdp/Makefile])
dnl AC_CONFIG_FILES([libnms/rtsp/Makefile])
AC_CONFIG_FILES([src/Makefile])
dnl AC_CONFIG_FILES([libnms/wsocket/Makefile])
dnl AC_CONFIG_FILES([libnms/Makefile])
AC_CONFIG_FILES([output/Makefile])
AC_CONFIG_FILES([output/audio/Makefile])
AC_CONFIG_FILES([output/video/Makefile])
AC_CONFIG_FILES([output/diskwriter/Makefile])
AC_CONFIG_FILES([decoder/Makefile])
AC_CONFIG_FILES([decoder/L16_8k_m/Makefile])
AC_CONFIG_FILES([decoder/gsm_amr_float/Makefile])

dnl if test x$NEMESI_HAVE_AVC = xyes; then
AC_CONFIG_FILES([decoder/libffmp3/Makefile])
AC_CONFIG_FILES([decoder/libffmpeg/Makefile])
AC_CONFIG_FILES([decoder/libffmpeg4/Makefile])
dnl fi

dnl AC_CONFIG_FILES([libnms/md5/Makefile])
dnl AC_CONFIG_FILES([libnms/utils/Makefile])
AC_CONFIG_FILES([plugins/Makefile])
AC_CONFIG_FILES([docs/Makefile])
AC_OUTPUT

AC_MSG_NOTICE([Release is .......... : $VERSION])

if test -f svnrev.h; then
AC_MSG_NOTICE([Revision is ......... : $REVISION
])
fi

AC_MSG_NOTICE([config files successfully generated!

NeMeSI Install prefix .......... : $prefix
NeMeSI Binary path ............. : ${exec_prefix}/bin/$nemesi_bin
NeMeSI Plugins dir ............. : $nemesi_plugindir
NeMeSI Throbbers dir ........... : $nemesi_throbberdir
NeMeSI Pixmaps dir ............. : $nemesi_pixmapdir

Byte order ..................... : this is a $BIGENDIAN machine

LibSDL support enabled ......... : $NEMESI_HAVE_SDL
LibAVCodec support enabled ..... : $NEMESI_HAVE_AVC

GUI support enabled ............ : $NEMESI_HAVE_GTK
NeMeSI GUI Binary path ......... : $exec_prefix/bin/$nemesi_gui
$debug_msg
$efence_msg

 'make' will now compile NeMeSI and 'su -c make install' will install it.
 Note: On non-Linux systems you might need to use 'gmake' instead of 'make'.
])
dnl
