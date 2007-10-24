libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include

noinst_LTLIBRARIES = libcc.la

libcc_la_SOURCES =	issdplicense.c \
			cc_newlicense.c \
			cc_set_sdplicense.c \
			cc_parse_urilicense.c \
			cc_perm_chk.c \
			cc_printmask.c \
			cc_tag_file.c \
			cc_id3v2.c

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
