nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = L16_8k_m.la

libdir = $(nemesi_plugindir)

L16_8k_m_la_SOURCES = L16-8k-m.c
L16_8k_m_la_LDFLAGS = -module -avoid-version
L16_8k_m_la_LIBADD = @LIBLTDL@

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir)
INCLUDES += @INCLTDL@
