/*
 * Common parsing for xiph format
 */


#define RTP_XIPH_ID(pkt)    (   RTP_PKT_DATA(pkt)[0]<<16 +\
                                RTP_PKT_DATA(pkt)[1]<<8 +\
                                RTP_PKT_DATA(pkt)[2]    )
#define RTP_XIPH_F(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0xc0)>> 6)
#define RTP_XIPH_T(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0x30)>> 4)
#define RTP_XIPH_PKTS(pkt)  (RTP_PKT_DATA(pkt)[3]& 0x0F)
#define RTP_XIPH_LEN(pkt,off)   (RTP_PKT_DATA(pkt)[off]<<8+\
                                 RTP_PKT_DATA(pkt)[off+1])
#define RTP_XIPH_DATA(pkt,off)  (RTP_PKT_DATA(pkt)[off+2])

