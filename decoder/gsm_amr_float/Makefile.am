nemesiincludedir=$(top_srcdir)/include

noinst_LTLIBRARIES = libdec.la

libdec_la_SOURCES =	sp_dec.c \
			interf_dec.c

EXTRA_DIST = 		interf_dec.h \
			sp_dec.h \
			interf_rom.h \
			rom_dec.h \
			typedef.h

libdec_la_LDFLAGS = -static

lib_LTLIBRARIES = gsm_amr_float.la

libdir = $(DESTDIR)$(nemesi_plugindir)

gsm_amr_float_la_SOURCES = gsm-amr-float.c
gsm_amr_float_la_LDFLAGS = -module -avoid-version
gsm_amr_float_la_LIBADD = libdec.la

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir)
