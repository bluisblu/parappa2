/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/*
 * multitap library
 *	Copyright(C) 1999,2000  Sony Computer Entertainment Inc.,
 *	All rights reserved.
 */

#ifndef _LIBMTAP_H_
#define _LIBMTAP_H_

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
int sceMtapInit( void );
int sceMtapPortOpen( int port );
int sceMtapPortClose( int port );
int sceMtapGetConnection( int port );
int sceMtapChangeThreadPriority( int priority_high, int priority_low );
#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
