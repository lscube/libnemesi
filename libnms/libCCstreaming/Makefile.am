libnmsdir = $(top_srcdir)/libnms
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libCCstreaming.la

libCCstreaming_la_SOURCES =	issdplicense.c \
				cc_newlicense.c \
				cc_set_sdplicense.c \
				cc_parse_urilicense.c \
				cc_prms_chk.c \
				cc_printmask.c \
				cc_tag_file.c \
				cc_id3v2.c
			
libCCstreaming_la_LDFLAGS = -static
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
