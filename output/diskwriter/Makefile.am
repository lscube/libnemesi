nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libdiskwriter.la
libdiskwriter_la_SOURCES =	diskwriter.c \
				diskwriter_init.c \
				get_file_fd.c \
				close_files.c \
				diskwriter_close.c

libdiskwriter_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
