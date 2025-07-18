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
 *                       libssyn - libssyn.h
 *                      EE Software Synthesizer
 *
 *     Version   Date          Design     Log
 *  --------------------------------------------------------------------
 *     0.60      Oct.12.1999   katayama   first checked in.
 */
#ifndef _libssyn_h_
#define _libssyn_h_
#define sceSSynNoError	0
#define sceSSynError		-1

#define sceSSynMaxOutput	8
#define sceSSynMaxInput		8
#define sceSynthesizerChOutNum	4

#define sceSSynVoiceBufferSize	576
#define sceSSynInputBufferSize	13952

typedef struct {
	unsigned int	unit_samples;
	unsigned int	sampling_frequency;
	unsigned int	n_voices;
	void			*voice_buffer;
	unsigned int	voice_buffer_size;
	unsigned int	n_input_port;
	void			*input_port_buffer;
	unsigned int	input_port_buffer_size;
} sceSSynConf;
#define sceSSynEnvSize	28
typedef struct {
	unsigned int	input_buff_len;
	void			*input_buff;
	void			*tone_param;
	unsigned int	(*msg_callback)(unsigned int,unsigned int);
	unsigned int	msg_callback_private_data;
	unsigned int	(*exc_callback)(unsigned int,unsigned char*,unsigned int,
									unsigned char*,unsigned int);
	unsigned int	exc_callback_private_data;
	unsigned int	system[(sceSSynEnvSize+sizeof(int)-1)/sizeof(int)];
} sceSSynEnv;

int sceSSyn_Init(sceCslCtx *pCtx, unsigned int interval );
int sceSSyn_ATick(sceCslCtx*);
int sceSSyn_Load(sceCslCtx*,unsigned int,void*);
int sceSSyn_RegisterRpc(sceCslCtx*,int);
int sceSSyn_PrepareParameter(void*,unsigned int);
#define SSYN_VOLUME_0DB 0x10000
int sceSSyn_SetMasterVolume(sceCslCtx*,unsigned int);
int sceSSyn_SetOutPortVolume(sceCslCtx*,unsigned int,unsigned int);
typedef struct {
	unsigned char	ch;
	unsigned char	ch_output;
#define sceSSynMuteOut	0
#define sceSSynMonoOut	1
#define sceSSynLOut		2
#define sceSSynROut		3
	unsigned char	mode;
	unsigned char	output_line;
	unsigned int	att;
} sceSSynChOutAttrib;
int sceSSyn_SetOutputAssign(sceCslCtx*,unsigned int,sceSSynChOutAttrib*);
int sceSSyn_SetPortVolume(sceCslCtx*,unsigned int,unsigned int);
int sceSSyn_SendShortMsg(sceCslCtx*,unsigned int,unsigned int);
int sceSSyn_SendExcMsg(sceCslCtx*,unsigned int,unsigned char*,unsigned int);
typedef struct {
	unsigned char	ch;
	unsigned short	num;
	unsigned short	data;
} sceSSynRpnMsg;
typedef sceSSynRpnMsg sceSSynNrpnMsg;
int sceSSyn_SendRpnMsg(sceCslCtx*,unsigned int,sceSSynRpnMsg*);
int sceSSyn_SendNrpnMsg(sceCslCtx*,unsigned int,sceSSynNrpnMsg*);
int sceSSyn_SetChPriority(sceCslCtx*,unsigned int,unsigned int,unsigned char);
int sceSSyn_SetPortMaxPoly(sceCslCtx*,unsigned int,unsigned char);
int sceSSyn_ClearBreakAtick(sceCslCtx*);
int sceSSyn_BreakAtick(sceCslCtx*);
#define sceSSynOutputMode_Mono		0
#define sceSSynOutputMode_Stereo	1
int sceSSyn_SetOutputMode(int);
#define sceSSynTvaEnvMode_Fixed				0
#define sceSSynTvaEnvMode_ChangeByLevel		1
int sceSSyn_SetTvaEnvMode(int);
#endif //!_libssyn_h_
/* $Id: libssyn.h,v 1.6 2000/07/07 10:04:35 kaol Exp $ */

