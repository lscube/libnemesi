nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libaudio.la
libaudio_la_SOURCES =	ab_get.c \
			ab_init.c \
			ab_uninit.c \
			audio_format_bits.c \
			audio_format_name.c \
			audio_init.c \
			audio_oss.c \
			audio_sdl.c \
			audio_utils.c

libaudio_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
