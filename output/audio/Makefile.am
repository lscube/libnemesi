nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libaudio.la
libaudio_la_SOURCES =	ab_get.c \
			ab_init.c \
			ab_uninit.c \
			audio_format_bits.c \
			audio_format_name.c \
			audio_init.c \
			audio_oss.c \
			audio_utils.c
if NEMESI_HAVE_SDL
libaudio_la_SOURCES += audio_sdl.c
endif


libaudio_la_LDFLAGS = -static
libaudio_la_LIBADD = @SDL_LIBS@
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir) @SDL_CFLAGS@
