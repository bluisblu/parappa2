/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *                      Emotion Engine Library
 *                          Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       <netglue - netglue.h>
 *               <glue library header for protocol stack >
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Sep,21,2001     komaki      first version
 */

#include <netglue/netdb.h>
#include <netglue/sys/socket.h>
#include <netglue/netinet/in.h>
#include <netglue/netinet/tcp.h>
#include <netglue/arpa/inet.h>

int *__sceNetGlueErrnoLoc(void);
int *__sceNetGlueHErrnoLoc(void);

#define	sceNetGlueErrno		(*__sceNetGlueErrnoLoc())
#define	sceNetGlueHErrno	(*__sceNetGlueHErrnoLoc())

#ifdef __cplusplus
extern "C" {
#endif

int sceNetGlueAbort(int sockfd);
int sceNetGlueThreadInit(int thread_id);
int sceNetGlueThreadTerminate(int thread_id);

#ifdef __cplusplus
}
#endif

