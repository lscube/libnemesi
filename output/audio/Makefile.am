nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libaudio.la
libaudio_la_SOURCES =	ab_get.c \
			ab_init.c \
			audio_close.c \
			audio_format_bits.c \
			audio_format_name.c \
			audio_preinit.c \
			audio_oss.c \
			audio_pause.c \
			audio_play.c \
			audio_sdl.c \
			empty_audio_buffer.c \
			enable_output.c \
			oss_play.c \
			set_audiodev.c

libaudio_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
