
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libwsocket.la
libwsocket_la_SOURCES = 	server_connect.c \
				server_create.c \
				tcp_open.c \
				tcp_write.c \
				tcp_read.c \
				gethostinfo.c \
				sockaddrcmp.c \
				sockaddrdup.c \
				sock_cmp_addr.c \
				sock_cmp_port.c \
				sock_set_addr.c \
				sock_set_port.c \
				sock_get_port.c \
				sock_ntop_host.c
libwsocket_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
