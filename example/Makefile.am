
libnmsdir = $(top_builddir)

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir = $(top_srcdir)/include

bin_PROGRAMS = dump_info dump_stream loop_stream

dump_info_SOURCES = dump_info.c

dump_info_LDADD = $(libnmsdir)/libnemesi.la

dump_stream_SOURCES = dump_stream.c

dump_stream_LDADD = $(libnmsdir)/libnemesi.la

loop_stream_SOURCES = loop_stream.c

loop_stream_LDADD = $(libnmsdir)/libnemesi.la

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)

$(OBJECTS): libtool

$(libnmsdir)/libnemesi.la:
	make -C $(libnmsdir) libnemesi.la

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
