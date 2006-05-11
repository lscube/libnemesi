
libnmsdir = $(top_srcdir)

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir = $(top_srcdir)/include

bin_PROGRAMS = dump_info

dump_info_SOURCES = dump_info.c

dump_info_LDADD = $(libnmsdir)/libnemesi.la

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)

$(OBJECTS): libtool

$(libnmsdir)/libnemesi.la:
	make -C $(libnmsdir) libnemesi.la

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
