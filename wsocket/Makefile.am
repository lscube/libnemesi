
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libwsocket.la
libwsocket_la_SOURCES = 	server_connect.c \
				server_create.c \
				tcp_open.c \
				tcp_write.c \
				tcp_read.c \
				gethostinfo.c
libwsocket_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
