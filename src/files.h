/***************************************************************************
*   Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*   Michael Seifert, Hans Henrik Stærfeldt, Tom Madsen, and Katja Nyboe.   *
*                                                                          *
*   Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
*   Chastain, Michael Quan, and Mitchell Tse.                              *
*                                                                          *
*       ROM 2.4 is copyright 1993-1995 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@pacinfo.com)                              *
*           Gabrielle Taylor (gtaylor@pacinfo.com)                         *
*           Brian Moore (rom@rom.efn.org)                                  *
*       ROT 2.0 is copyright 1996-1999 by Russ Walsh                       *
*       DRM 1.0a is copyright 2000-2002 by Joshua Chance Blackwell         *
*        SD 2.0 is copyright 2004-2006 by Patrick Mylund Nielsen           *
***************************************************************************/

/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR	""          /* Player files */
#define TEMP_FILE	"romtmp"
#define NULL_FILE	"proto.are" /* To reserve one stream */
#endif

#if defined(MSDOS)
#define PLAYER_DIR	""          /* Player files */
#define TEMP_FILE	"romtmp"
#define NULL_FILE	"nul"       /* To reserve one stream */
#endif

#if defined(unix)
#define PLAYER_DIR      "../player/"    /* Player files */
#define BACKUP_DIR      "../player/backupped/"    /* Backup Player Files */
//#define GOD_DIR         "../player/gods/"  /* list of gods */
#define TEMP_FILE	"../data/romtmp"
#define NULL_FILE	"/dev/null" /* To reserve one stream */
#endif

#define AREA_LIST       "../area/area.lst"  /* List of areas */
#define SHUTDOWN_FILE   "../data/text/shutdown.txt"  /* For 'shutdown' */
#define LAST_COMMAND	"../data/text/last_command.txt"  /* logs last command before a crash */
#define WIZLOCK_FILE	"../data/wizlock.dat"
#define NEWLOCK_FILE	"../data/newlock.dat"
#define BAN_FILE	"../data/text/ban.txt"
#define WIZ_FILE	"../data/text/wizlist.txt"
#define ANC_FILE	"../data/text/anclist.txt"
// #define MUSIC_FILE	"../data/text/music.txt"
#define HELP_FILE       "../data/text/help.txt"  /* For undefined helps */
#define QUEST_FILE      "../data/text/quest.txt" /*for purchase logging. */
#define STAT_FILE       "../data/text/statlist.txt"
#define BANK_FILE       "../data/text/bank.txt"
#define ILLEGAL_FILE	"../data/text/illegal.txt"
#define CORE_EXAMINE_SCRIPT "../data/gdbscript"
#define WRONGPW_FILE "../data/text/wrong_passwords.txt"
#define MAIN_HELP_FILE "../data/helps/help.are"
#define MUDCHECK_SCRIPT "/home/httpd/vhosts/sdmud.com/sd/data/mudcheck.pl"
#define DEVCHECK_SCRIPT "/home/httpd/vhosts/sdmud.com/sd/dev/data/mudcheck.pl"
#define MAX_EVER_FILE "../data/text/max_ever.txt"
// #define VOTES_FILE "../data/text/votes.txt" /* For counting votes */
#define HOST_DIR       "../player/hosts/"      /* Player hosts */
#define REBOOT_FILE "../data/reboot.data"
#define EXE_FILE	  "../data/sdmud"
#define BOUNTY_FILE "../data/text/bounties.txt"
