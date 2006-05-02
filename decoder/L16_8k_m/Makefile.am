libnmsdir = $(top_srcdir)/libnms

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = L16_8k_m.la

libdir = $(nemesi_plugindir)

L16_8k_m_la_SOURCES = L16-8k-m.c
L16_8k_m_la_LDFLAGS = -module -avoid-version

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir) -I$(nemesiincludedir)
