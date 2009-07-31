ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libnemesi.la

INCLUDES = -I$(srcdir)/src

libincludedir = $(includedir)/nemesi
libinclude_HEADERS = 	src/nemesi/rtpptdefs.h \
			src/nemesi/cc.h \
			src/nemesi/rtsp.h \
			src/nemesi/rtsp_errors.h \
			src/nemesi/comm.h \
			src/nemesi/sdp.h \
			src/nemesi/version.h \
			src/nemesi/rtcp.h \
			src/nemesi/rtp.h \
			src/nemesi/transport.h

libnemesi_la_SOURCES = \
			src/bufferpool/bufferpool.h \
			src/bufferpool/poadd.c \
			src/bufferpool/poinit.c \
			src/bufferpool/podel.c \
			src/bufferpool/bpfree.c \
			src/bufferpool/bpinit.c \
			src/bufferpool/bpkill.c \
			src/bufferpool/bpget.c \
			src/bufferpool/bprmv.c \
			\
			src/creativecommons/cc_newlicense.c \
			src/creativecommons/cc_sdplicense.c \
			src/creativecommons/cc_parse_urilicense.c \
			src/creativecommons/cc_perm_chk.c \
			src/creativecommons/cc_printmask.c \
			src/creativecommons/cc_tag_file.c \
			src/creativecommons/cc_id3v2.c \
			\
			src/parsers/rtpparser.h \
			src/parsers/rtpparsers.h \
			src/parsers/rtp_xiph.h \
			src/parsers/rtp_utils.h\
			src/parsers/rtpparsers.c \
			src/parsers/rtp_mpa.c \
			src/parsers/rtp_mpv.c \
			src/parsers/rtp_m4v.c \
			src/parsers/rtp_aac.c \
			src/parsers/rtp_utils.c \
			src/parsers/rtp_h263.c \
			src/parsers/rtp_h264.c \
			src/parsers/rtp_theora.c \
			src/parsers/rtp_vorbis.c \
			src/parsers/rtp_speex.c \
			\
			src/rtcp/rtcp_utils.c \
			src/rtcp/rtcp_sdes.c \
			src/rtcp/rtcp_report.c \
			src/rtcp/rtcp_bye.c \
			src/rtcp/rtcp_app.c \
			src/rtcp/rtcp_recv.c \
			src/rtcp/rtcp_events.c \
			src/rtcp/rtcp.c \
			\
			src/rtp/rtp_thread.c \
			src/rtp/rtp_buffer.c \
			src/rtp/rtp_session.c \
			src/rtp/rtp_recv.c \
			src/rtp/rtp_transport.c \
			src/rtp/rtp_ssrc_queue.c \
			src/rtp/rtp_payload_type.c \
			\
			src/sdp/sdp_session_setup.c \
			src/sdp/sdp_media_setup.c \
			src/sdp/sdp_attr.c \
			src/sdp/sdp_session_destroy.c \
			src/sdp/sdp_media_destroy.c \
			src/sdp/sdp_parse_m_descr.c \
			\
			src/utils/utils.h \
			src/utils/urltokenize.c \
			src/utils/random32.c \
			src/utils/timeval.c \
			src/utils/nmsprint.c \
			src/utils/nms_header.c \
			src/utils/transport.c \
			\
			src/rtsp/rtsp.c \
                        src/rtsp/rtsp_handlers.c \
                        src/rtsp/rtsp_internals.c \
                        src/rtsp/rtsp_send.c \
                        src/rtsp/rtsp_sessions.c \
                        src/rtsp/rtsp_state_machine.c \
                        src/rtsp/rtsp_transport.c \
                        src/rtsp/rtsp_errors.c \
			src/rtsp/rtspinternals.h

if EXAMPLES
bin_PROGRAMS = \
	example/dump_info \
	example/dump_stream \
	example/loop_stream

example_dump_info_SOURCES = example/dump_info.c
example_dump_info_LDADD = libnemesi.la

example_dump_stream_SOURCES = example/dump_stream.c
example_dump_stream_LDADD = libnemesi.la

example_loop_stream_SOURCES = example/loop_stream.c
example_loop_stream_LDADD = libnemesi.la
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/libnemesi.pc

libnemesi_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

EXTRA_DIST = Doxyfile CodingStyle
