
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libCCstreaming.la
libCCstreaming_la_SOURCES =	issdplicense.c \
				cc_newlicense.c \
				cc_set_sdplicense.c \
				cc_parse_urilicense.c
			
libCCstreaming_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
