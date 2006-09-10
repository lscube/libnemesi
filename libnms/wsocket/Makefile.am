libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libwsocket.la

libwsocket_la_SOURCES = \
				nmst_init.c \
				nmst_read.c \
				nmst_write.c \
				nmst_close.c \
				nmst_is_active.c \
				server_connect.c \
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
				sock_get_addr.c \
				sock_ntop_host.c \
				addrcmp.c \
				addr_ntop.c
				
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
