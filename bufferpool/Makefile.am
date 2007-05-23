libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libbuffpool.la

libbuffpool_la_SOURCES =	poadd.c \
				poinit.c \
				podel.c \
				bpfree.c \
				bpinit.c \
				bpkill.c \
				bpget.c \
				bprmv.c
				
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
