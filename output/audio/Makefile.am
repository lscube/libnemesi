nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libaudio.la
libaudio_la_SOURCES =	set_audiodev.c \
			enable_output.c \
			sys_buff_size.c \
			init_SDL.c \
			SDL_mixaudio.c \
			ab_init.c \
			ab_get.c \
			oss_play.c \
			get_sys_buff.c \
			empty_audio_buffer.c \
			audio_init.c \
			audio_pause.c \
			audio_play.c \
			audio_close.c

libaudio_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
