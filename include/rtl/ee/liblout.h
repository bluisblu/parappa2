/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/*
 *                     Emotion Engine Library
 *                          Version 0.60
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                       liblout - liblout.h
 *                         EE PCM Line Out
 *
 *     Version   Date          Design     Log
 *  --------------------------------------------------------------------
 *     0.60      Oct.12.1999   katayama   first checked in.
 */
#ifndef _liblout_h_
#define _liblout_h_
#define sceLoutNoError	0
#define sceLoutError	-1

#define sceLoutInputUnit	512
#define sceLoutMaxLine		4
#define	sceLoutNoOutPut		0xff
#define	sceLoutDmaPreWait	(1<<7)
typedef struct {
	unsigned char	attrib;
	unsigned char	lineAssign[sceLoutMaxLine];
	unsigned int	iopBufSize;
	void*			iopBuf[2];
	unsigned int	nDmaBuf;
	sceSifDmaData	dma[0];
} sceLoutConf;

int sceLout_Init(sceCslCtx*,unsigned int);
int sceLout_ATick(sceCslCtx*);
//int sceLout_Load(sceCslCtx*);
#define sceLout_Load(x)
#endif //!_liblout_h_
/* $Id: liblout.h,v 1.4 1999/12/13 01:41:50 morita Exp $ */

