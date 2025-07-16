/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/*
 *                     I/O Processor Library
 *                         Version 2.0.6
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                            csl.h
 *                     Component Sound Library
 *
 *     Version   Date          Design     Log
 *  --------------------------------------------------------------------
 *     2.0.6     Nov.2000      kaol       sceCsl{No}Error are added
 *     0.60      Oct.12.1999   katayama   first checked in.
 */
#ifndef _csl_h_
#define _csl_h_
typedef struct {
    int  	sema;	
    void	*buff;
} sceCslBuffCtx;

typedef struct {
	int				buffNum;
	sceCslBuffCtx	*buffCtx;
} sceCslBuffGrp;

typedef struct {
	int			buffGrpNum;	//�o�b�t�@�O���[�v���@�@�@�@�@�F�K�{
	sceCslBuffGrp*		buffGrp;	//�o�b�t�@�O���[�v�ւ̃|�C���^�F�K�{
	void*			conf;		//���W���[���ݒ�\���́@�@�@�@�F�C��
	void*			callBack;	//�R�[���o�b�N�֐��@�@�@�@�@�@�F�C��
	char**			extmod;		//�O�����W���[���̃G���g���@�@�F�C��
} sceCslCtx;

#define sceCslNoError	0
#define sceCslError	(-1)
#define sceCslWarning	1

#endif //!_csl_h_
