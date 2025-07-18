/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/* 
 *                      I/O Processor Library
 *                          Version 1.02
 *                           Shift-JIS
 *
 *      Copyright (C) 1995-2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         inet - udp.h
 *                    User Datagram Protocol
 *
 * $Id: udp.h,v 1.4 2001/01/25 04:25:16 ywashizu Exp $
 */

#if !defined(_UDP_H)
#define _UDP_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
	u_short sport;		/* source port */
	u_short dport;		/* destination port */
	u_short len;		/* length */
	u_short chksum;		/* checksum */
} UDP_HDR;

#ifdef BIG_ENDIAN
#define udp_ntoh(p)
#define udp_hton(p)
#else

#define udp_ntoh(p)	((p)->sport = ntohs((p)->sport), \
			 (p)->dport = ntohs((p)->dport), \
			 (p)->len = ntohs((p)->len))

#define udp_hton(p)	((p)->sport = htons((p)->sport), \
			 (p)->dport = htons((p)->dport), \
			 (p)->len = htons((p)->len))
#endif

#if defined(__cplusplus)
}
#endif

#endif	/* _UDP_H */
