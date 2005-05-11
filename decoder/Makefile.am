SUBDIRS = L16_8k_m gsm_amr_float


if NEMESI_HAVE_AVC
SUBDIRS += libffmpeg libffmp3 libffmpeg4
endif

nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libdecoder.la

libdecoder_la_SOURCES =	decoder.c \
			dec_create.c \
			dec_clean.c

libdecoder_la_LDFLAGS = -static

INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
