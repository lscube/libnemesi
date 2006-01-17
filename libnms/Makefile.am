SUBDIRS =	bufferpool \
			wsocket \
			md5 \
			utils \
			rtp \
			rtcp \
			libCCstreaming \
			sdp \
			rtsp

nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libnemesi.la

libnemesi_la_SOURCES =

libnemesi_la_LIBADD =	rtsp/librtsp.la \
						sdp/libsdp.la \
						libCCstreaming/libCCstreaming.la \
						rtcp/librtcp.la \
						rtp/librtp.la \
						bufferpool/libbuffpool.la \
						wsocket/libwsocket.la \
						utils/libutils.la \
						md5/libmd5.la

# libnemesi_la_LDFLAGS = -static
libnemesi_la_LDFLAGS = -shared
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
