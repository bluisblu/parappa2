#ifndef WP2CD_H
#define WP2CD_H

/* WP2 commands */
#define WP2_NONE                 (0)

#define WP2_INIT                 0x8000 /* Arg -> Block size        */
#define WP2_QUIT                 0x0001 /* Arg -> Status (unused)   */
#define WP2_OPEN                 0x8002 /* Arg -> File name         */
#define WP2_CLOSE                0x0003 /* Arg -> Status (unused)   */
#define WP2_PRELOAD              0x0004 /*         No args          */
#define WP2_START                0x8005 /*         No args          */
#define WP2_STOP                 0x0006 /* Arg -> Volume (unused)   */
#define WP2_SEEK                 0x8007 /* Arg -> Seek offset/pos.  */
#define WP2_SETVOLUME            0x0008 /* Arg -> Volume            */
#define WP2_SETVOLDIRECT         0x0009 /* Arg -> Direct volume     */
#define WP2_SETMASTERVOL         0x000a /* Arg -> Master volume     */
#define WP2_SETMODE              0x800c /* Arg -> Mode              */
#define WP2_GETMODE              0x800b /*         No args          */
#define WP2_SDINIT               0x000d /* Arg -> Status            */
#define WP2_SETCHANNEL           0x000f /* Arg -> Channel           */
#define WP2_CDINIT               0x000e /* Arg -> Mode              */
#define WP2_GETTIME              0x8010 /*         No args          */
#define WP2_GETTIMESAMPLE        0x8011 /*         No args          */
#define WP2_GETCDERRCODE         0x8012 /*         No args          */
#define WP2_OPENFLOC             0x8013 /* Arg -> File name/CD str. */
#define WP2_SEEKFLOC             0x8014 /* Arg -> File name/CD str. */
#define WP2_PRELOADBACK          0x0015 /*         No args          */
#define WP2_SETTRPOINT           0x0016 /* Arg -> Transfer address  */
#define WP2_READBUFF             0x8017 /*         No args          */

/* WP2 modes */
/* Set */
#define WP2_MODE_REPEAT_OFF      0x0000
#define WP2_MODE_REPEAT_DEFAULT  0x0001
#define WP2_MODE_REPEAT_FORCED   0x0002

#define WP2_MODE_STEREO          0x0000
#define WP2_MODE_MONO            0x0010

/* Get */
#define WP2_MODE_IDLE            0x0000
#define WP2_MODE_RUNNING         0x1000
#define WP2_MODE_PAUSE           0x2000
#define WP2_MODE_FADE            0x4000
#define WP2_MODE_TERMINATE       0x8000

/* WaveP2 module ID */
#define WP2CD_DEV (0x8800)

#endif
