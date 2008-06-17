bin_PROGRAMS = dump_info dump_stream loop_stream

dump_info_SOURCES = dump_info.c

dump_info_LDADD = $(top_builddir)/libnemesi.la

dump_stream_SOURCES = dump_stream.c

dump_stream_LDADD = $(top_builddir)/libnemesi.la

loop_stream_SOURCES = loop_stream.c

loop_stream_LDADD = $(top_builddir)/libnemesi.la

INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(top_srcdir)

$(OBJECTS): libtool

$(libnmsdir)/libnemesi.la:
	make -C $(libnmsdir) libnemesi.la

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
