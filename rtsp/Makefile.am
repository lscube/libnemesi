
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtsp.la
librtsp_la_SOURCES =	init_rtsp.c \
			reinit_rtsp.c \
			init_state.c \
			ready_state.c \
			playing_state.c \
			recording_state.c \
			open_cmd.c \
			rtsp.c \
			send_get_request.c \
			send_setup_request.c \
			stop_cmd.c \
			pause_cmd.c \
			play_cmd.c \
			close_cmd.c \
			send_play_request.c \
			send_pause_request.c \
			send_teardown_request.c \
			full_msg_rcvd.c \
			rtsp_recv.c \
			body_exists.c \
			check_response.c \
			handle_rtsp_pkt.c \
			handle_get_response.c \
			handle_setup_response.c \
			handle_play_response.c \
			handle_pause_response.c \
			handle_teardown_response.c \
			check_status.c \
			set_rtsp_sessions.c \
			set_rtsp_media.c \
			rtsp_sess_copy.c \
			rtsp_sess_create.c \
			rtsp_med_create.c \
			get_curr_sess.c \
			set_transport_str.c \
			get_transport_str.c \
			remove_pkt.c \
			rtsp_clean.c \
			close_rtsp.c \
			seturlname.c
			
librtsp_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
