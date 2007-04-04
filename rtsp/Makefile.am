libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtsp.la

librtsp_la_SOURCES =	body_exists.c \
			check_response.c \
			check_status.c \
			close_cmd.c \
			full_msg_rcvd.c \
			get_curr_sess.c \
			get_transport_str.c \
			get_transport_str_tcp.c \
			get_transport_str_udp.c \
			handle_get_response.c \
			handle_pause_response.c \
			handle_play_response.c \
			handle_rtsp_pkt.c \
			handle_setup_response.c \
			handle_teardown_response.c \
			init_state.c \
			open_cmd.c \
			pause_cmd.c \
			play_cmd.c \
			playing_state.c \
			ready_state.c \
			recording_state.c \
			remove_pkt.c \
			rtsp.c \
			rtsp_clean.c \
			rtsp_close.c \
			rtsp_get_rtp_queue.c \
			rtsp_get_rtp_th.c \
			rtsp_info_print.c \
			rtsp_infos.c \
			rtsp_init.c \
			rtsp_is_busy.c \
			rtsp_med_create.c \
			rtsp_open.c \
			rtsp_pause.c \
			rtsp_play.c \
			rtsp_recv.c \
			rtsp_reinit.c \
			rtsp_sess_create.c \
			rtsp_sess_dup.c \
			rtsp_stop.c \
			rtsp_unbusy.c \
			rtsp_uninit.c \
			rtsp_wait.c \
			send_get_request.c \
			send_pause_request.c \
			send_play_request.c \
			send_setup_request.c \
			send_teardown_request.c \
			set_rtsp_media.c \
			set_rtsp_sessions.c \
			set_transport_str.c \
			set_transport_str_tcp.c \
			set_transport_str_udp.c \
			seturlname.c \
			stop_cmd.c
if NEMESI_SCTP
librtsp_la_SOURCES +=	get_transport_str_sctp.c \
			set_transport_str_sctp.c
endif
			
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
