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
 *                       <libhttp - libhttp.h>
 *                     <header for http library>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Nov,05,2001     komaki      first version
 */

#ifndef _LIBHTTP_H
#define _LIBHTTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libhttp/http_status.h>
#include <libhttp/http_methods.h>
#include <libhttp/http_options.h>

#define sceHTTPProt_HTTP	0
#define sceHTTPProt_HTTPS	1

typedef struct sceHTTPHeaderList {
	struct sceHTTPHeaderList *forw, *back;
	char *name;
	char *value;
} sceHTTPHeaderList_t;

typedef struct sceHTTPResponse {
	int http_ver;
	int http_rev;
	sceHTTPStatusCode_t code;		/* server status return	*/
	char *reason;			/* reason phrase */
	int server_prot;		/* server protocol */
	sceHTTPHeaderList_t *headers;	/* list of returned headers */
	unsigned char *entity;		/* response data */
	unsigned int length;		/* length of entity */
	int interrupted;		/* transfer interrupted */
	int date;			/* response date and time */
} sceHTTPResponse_t;

typedef struct sceHTTPClient {
	char *name;			/* user-agent name */
	int http_ver;
	int http_rev;
	int rtimeout;			/* response timeout */
	int ttimeout;			/* transfer timeout */
	int laptime;
	int prot;			/* protocol */
	int state;
	int errnum;
	int net_errno;
	int reloading;
	int keep_alive;
	int keep_count;
	int non_blocking;
	int abort_req;
	int t_stacksize;
	int t_priority;
	int t_thread;
	void *t_stack;
	int t_rtn;
	void (*t_notify)(int flags);
	unsigned int max_olength;
	struct sceHTTPParsedURI *proxy;
	/* Request */
	sceHTTPMethod_t method;		/* method code */
	struct sceHTTPParsedURI *puri;	/* parsed identifier */
	sceHTTPHeaderList_t *iheaders;	/* list of headers */
	char *idata;			/* input data */
	int ilength;			/* lenght of idata */
	int iflags;			/* input flags */
	/* Response */
	sceHTTPResponse_t response;
	void (*chunkf)(struct sceHTTPClient *, unsigned char *, unsigned int);
	/* io */
	int recv_thread;
	int send_thread;
	void *io_rstack;
	void *io_sstack;
	int io_desc;
	char *io_buf;
	int io_len;
	int io_rtn;
	int io_timer;
	int io_rwait, io_rdone;
	int io_swait, io_sdone;
	int io_flags;
	int io_tcount;
} sceHTTPClient_t;

/* iflags */
/* sceHTTPInputF_ESCAPE wants to encode data with URL encoding. */
#define sceHTTPInputF_ESCAPE	1
/* The input data doesn't copied if sceHTTPInputF_LINK is on. */
#define sceHTTPInputF_LINK	2

/* errnum */
#define	sceHTTPError_KERNEL	-1001
#define	sceHTTPError_NOMEM	-1002
#define	sceHTTPError_IO		-1003
#define	sceHTTPError_INVAL	-1004
#define	sceHTTPError_TIMEOUT	-1005
#define	sceHTTPError_RESOLV	-1006
#define	sceHTTPError_SOCKET	-1007
#define	sceHTTPError_CONNECT	-1008
#define	sceHTTPError_SSL	-1009
#define	sceHTTPError_NOTYET	-1010
#define	sceHTTPError_INTR	-1011

typedef struct sceHTTPParsedURI {
	char *scheme;
	char *username;
	char *password;
	char *hostname;
	int port;
	char *filename;
	char *search;
} sceHTTPParsedURI_t;

#define sceHTTPParseURI_USER		0x02
#define sceHTTPParseURI_FILENAME	0x20
#define sceHTTPParseURI_SEARCHPART	0x40

typedef struct sceHTTPCookie {
	char *name;
	char *value;
	char *domain;
	char *path;
	int expires;
	int secure;
	int version;
	int maxage;
} sceHTTPCookie_t;

typedef struct sceHTTPCookieList {
	struct sceHTTPCookieList *forw, *back;
	struct sceHTTPCookie cookie;
} sceHTTPCookieList_t;

typedef struct sceHTTPAuth {
	int type;
	char *realm;
	char **domains;
	char *uri;
	char *nonce;
	char *opaque;
	int stale;
	int algorithm;
	int qop;
} sceHTTPAuth_t;

typedef struct sceHTTPAuthList {
	struct sceHTTPAuthList *forw, *back;
	struct sceHTTPAuth auth;
} sceHTTPAuthList_t;

typedef struct sceHTTPAuthInfo {
	char *nextnonce;
	char *rspauth;
	char *cnonce;
	int nc;
	int qop;
} sceHTTPAuthInfo_t;

typedef struct sceHTTPDigest {
	char *username;
	char *realm;
	char *password;
	char *uri;
	char *nonce;
	char *cnonce;
	char *opaque;
	int algorithm;
	int nc;
	int qop;
	int method;
	char *entity;
	int length;
} sceHTTPDigest_t;

#define sceHTTPAuth_BASIC		0
#define sceHTTPAuth_DIGEST		1
#define sceHTTPDigestAlg_MD5		1
#define sceHTTPDigestAlg_MD5SESS	2
#define sceHTTPDigestQOP_AUTH		1
#define sceHTTPDigestQOP_AUTHINT	2
#define sceHTTPDigestStale_TRUE		1
#define sceHTTPDigestStale_FALSE	2

typedef struct sceHTTPMimeFilter {
	struct sceHTTPMimeFilter *next;
	struct sceHTTPMimeFilter *prev;
	int itype;
	int idesc;
	unsigned char *ibuf;
	unsigned int ibuflen;
	unsigned char *iptr;
	int idesc_eof;
	int otype;
	int odesc;
	unsigned char *obuf;
	unsigned int obuflen;
	unsigned char *optr;
	sceHTTPHeaderList_t *headers;
	int dflags;
	int (*decoder)(const char *, char *, int);
	unsigned char *dbuf;
	int multipart;
	char *boundary;
} sceHTTPMimeFilter_t;

#define sceHTTPMimeFilter_STRING	0
#define sceHTTPMimeFilter_FILE		1

#define	sceHTTPMultipart_MIXED		1
#define	sceHTTPMultipart_BYTERANGES	2
#define	sceHTTPMultipart_ALTERNATIVE	3

/* Library initialization */
extern int sceHTTPInit(void);

/* HTTP connection */
extern int sceHTTPOpen(sceHTTPClient_t *client);
extern int sceHTTPClose(sceHTTPClient_t *client);
extern int sceHTTPGetNetError(sceHTTPClient_t *client);

/* HTTP client transaction */
extern sceHTTPClient_t *sceHTTPCreate(void);
extern int sceHTTPDestroy(sceHTTPClient_t *client);
extern int sceHTTPRequest(sceHTTPClient_t *client);
extern sceHTTPResponse_t *sceHTTPGetResponse(sceHTTPClient_t *client);
extern int sceHTTPCleanUpResponse(sceHTTPClient_t *client);
extern int sceHTTPGetClientError(sceHTTPClient_t *client);
extern int sceHTTPAbortRequest(sceHTTPClient_t *client);

/* Options */
extern int sceHTTPSetOption(sceHTTPClient_t *client, sceHTTPOption_t opt, ...);
extern int sceHTTPGetOption(sceHTTPClient_t *client, sceHTTPOption_t opt, ...);

/* URI */
extern sceHTTPParsedURI_t *sceHTTPParseURI(const char *uri, int parseflag);
extern int sceHTTPFreeURI(sceHTTPParsedURI_t *puri);
extern sceHTTPParsedURI_t *sceHTTPCloneURI(sceHTTPParsedURI_t *puri);
extern char *sceHTTPUnparseURI(sceHTTPParsedURI_t *puri);
extern int sceHTTPIsAbsoluteURI(const char *uri);
extern char *sceHTTPFindAbsoluteURI(const char *uri, const char *base);

/* Convenience */
extern const char *sceHTTPErrorString(sceHTTPStatusCode_t error);

/* Header List */
extern sceHTTPHeaderList_t *sceHTTPAddHeaderList(sceHTTPHeaderList_t *p,
			const char *name, const char *value);
extern int sceHTTPFreeHeaderList(sceHTTPHeaderList_t *p);

static inline sceHTTPHeaderList_t *sceHTTPNextHeader(sceHTTPHeaderList_t *p){
	return((p)? p->forw : 0);
}

/* Cookies */
extern sceHTTPCookieList_t *sceHTTPAddCookieList(sceHTTPCookieList_t *p,
			sceHTTPCookie_t *cp);
extern int sceHTTPFreeCookieList(sceHTTPCookieList_t *p);
extern sceHTTPCookieList_t *sceHTTPParseCookie(sceHTTPResponse_t *rp);
extern int sceHTTPSetCookie(sceHTTPClient_t *client, sceHTTPCookieList_t *p);

/* Authentication */
extern int sceHTTPFreeAuthList(sceHTTPAuthList_t *p);
extern sceHTTPAuthList_t *sceHTTPParseAuth(sceHTTPResponse_t *rp);
extern int sceHTTPSetBasicAuth(sceHTTPClient_t *client,
			const char *user, const char *passwd);
extern int sceHTTPSetDigestAuth(sceHTTPClient_t *client, sceHTTPDigest_t *dp);
extern sceHTTPAuthInfo_t *sceHTTPParseAuthInfo(sceHTTPResponse_t *rp);

/* Redirection */
extern const char **sceHTTPParseLocations(sceHTTPResponse_t *rp);
extern int sceHTTPFreeLocations(char **vec);
extern int sceHTTPSetRedirection(sceHTTPClient_t *client,
			sceHTTPParsedURI_t *uri, int proxy);
/* Mime */
extern sceHTTPMimeFilter_t *sceHTTPMimeFilterCreate(int itype, void *iarg,
		int ilen, int otype, void *oarg);
extern int sceHTTPMimeFilterFree(sceHTTPMimeFilter_t *p);
extern int sceHTTPMimeFilterParseHeaders(sceHTTPMimeFilter_t *p);
extern int sceHTTPMimeFilterApply(sceHTTPMimeFilter_t *p, int *closep);
extern int sceHTTPMimeFilterGetMultipartType(sceHTTPMimeFilter_t *p);
extern int sceHTTPMimeFilterChangeOutput(sceHTTPMimeFilter_t *p, int otype,
		void *arg);
extern int sceHTTPMimeFilterGetStringOutput(sceHTTPMimeFilter_t *p,
		char **odatap, int *olengthp);
extern sceHTTPHeaderList_t *sceHTTPMimeFilterGetHeaderList(
		sceHTTPMimeFilter_t *p);

/* Version */
extern const char *sceHTTPLibVersion;

/* base64 */
extern int sceBASE64Encoder(unsigned const char *in, unsigned char *out,
		int len);
extern int sceBASE64LineDecoder(unsigned const char *in, unsigned char *out,
		int len);

/* quoted-printable */
extern int sceQPrintableEncoder(unsigned const char *in, unsigned char *out,
		int len);
extern int sceQPrintableLineDecoder(unsigned const char *in, unsigned char *out,
		int len);

/* url escape */
extern unsigned char *sceURLEscape(unsigned const char *in);
extern unsigned char *sceURLUnescape(unsigned const char *in);

#ifdef __cplusplus
}
#endif

#endif /* _LIBHTTP_H */
