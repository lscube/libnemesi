nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libdiskwriter.la
libdiskwriter_la_SOURCES =	diskwriter.c \
				diskwriter_init.c \
				db_init.c \
				db_get.c \
				get_file_fd.c \
				close_file.c \
				diskwriter_close.c

libdiskwriter_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
