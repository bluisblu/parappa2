/* SCEI CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4
 */
/* $Id: dirent.h,v 1.1 2001/02/07 07:02:28 tei Exp $ */

/*
 *                     I/O Processor System Services
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         dirent.h
 *                         IO manager interface
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           1999/11/09      hakama
 *       1.10           2000/09/12      isii
 *       1.20           2000/10/18      koji
 *       1.21           2000/10/21      isii
 */
#ifndef _SYS_STAT_H
#include <sys/stat.h>
#endif

#ifndef _DIRENT_H_DEFS
#define _DIRENT_H_DEFS

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

struct sce_dirent {
	struct sce_stat d_stat;	/* ファイルのステータス */
	char d_name[256]; 	/* ファイル名(フルパスではない) */
	void	*d_private;	/* その他 */
};

extern int dopen (const char *dirname);
extern int dclose (int fd);
extern int dread(int fd, struct sce_dirent *buf);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* _DIRENT_H_DEFS */

/* End of File */
