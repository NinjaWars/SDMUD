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

#ifndef GLOBALS_H
#define GLOBALS_H
#ifdef IN_DB_C
#ifdef GLOBAL
#undef GLOBAL
#endif
#ifdef GLOBAL_DEF
#undef GLOABL_DEF
#endif
#define	GLOBAL(str) str
#define GLOBAL_DEF(str, def) str = def

#else
#ifdef GLOBAL
#undef GLOBAL
#endif
#ifdef GLOBAL_DEF
#undef GLOABL_DEF
#endif
#define	GLOBAL(str) extern str
#define GLOBAL_DEF(str, def) extern str
#endif

GLOBAL_DEF ( int mobile_count, 0 );
GLOBAL ( char str_empty[1] );
GLOBAL_DEF ( AREA_DATA * area_first, NULL );
GLOBAL_DEF ( AREA_DATA * area_last, NULL );
GLOBAL_DEF ( SHOP_DATA * shop_last, NULL );

GLOBAL_DEF ( int top_area, 0 );
GLOBAL_DEF ( int top_exit, 0 );
GLOBAL_DEF ( int top_help, 0 );
GLOBAL_DEF ( int top_reset, 0 );
GLOBAL_DEF ( int top_room, 0 );
GLOBAL_DEF ( int top_shop, 0 );
GLOBAL_DEF ( int top_mob_index, 0 );
GLOBAL_DEF ( int top_obj_index, 0 );
GLOBAL_DEF ( int top_affect, 0 );
GLOBAL_DEF ( int top_ed, 0 );
GLOBAL_DEF ( int top_mprog_index, 0 );
GLOBAL_DEF ( int newmobs, 0 );
GLOBAL_DEF ( int newobjs, 0 );
GLOBAL_DEF ( int top_vnum_mob, 0 );
GLOBAL_DEF ( int top_vnum_obj, 0 );
GLOBAL_DEF ( int top_vnum_room, 0 );
GLOBAL_DEF ( long top_vnum, 0 );

GLOBAL ( ROOM_INDEX_DATA * room_index_hash[MAX_KEY_HASH] );
//GLOBAL ( GQUEST gquest_info );

GLOBAL_DEF ( HELP_DATA * help_last, NULL );

GLOBAL_DEF ( int share_value, SHARE_VALUE );

GLOBAL_DEF ( HELP_DATA * help_first, NULL );
GLOBAL_DEF ( SHOP_DATA * shop_first, NULL );

GLOBAL_DEF ( CHAR_DATA * char_list, NULL );
GLOBAL_DEF ( DESCRIPTOR_DATA * descriptor_list, NULL );
GLOBAL_DEF ( OBJ_DATA * object_list, NULL );

GLOBAL_DEF ( MPROG_CODE * mprog_list, NULL );
GLOBAL_DEF ( AUCTION_DATA * auction_list, NULL );

GLOBAL ( char bug_buf[2 * MAX_INPUT_LENGTH] );
GLOBAL ( time_t current_time );
GLOBAL_DEF ( bool fLogAll, FALSE );
// GLOBAL_DEF ( FILE * fpReserve, NULL );
GLOBAL ( KILL_DATA kill_table[MAX_LEVEL] );
GLOBAL ( char log_buf[2 * MAX_INPUT_LENGTH] );
GLOBAL ( TIME_INFO_DATA time_info );
GLOBAL ( TAG_DATA tag_game );
GLOBAL ( WEATHER_DATA weather_info );
GLOBAL ( NOTE_DATA * note_free );
GLOBAL ( OBJ_DATA * obj_free );
GLOBAL_DEF ( int reboot_counter, -1 );
GLOBAL_DEF ( bool MOBtrigger, TRUE );

GLOBAL_DEF ( bool global_quest, FALSE );
GLOBAL_DEF ( bool double_exp, FALSE );
GLOBAL_DEF ( bool double_qp, FALSE );
GLOBAL_DEF ( bool double_damage, FALSE );
GLOBAL_DEF ( bool double_stance, FALSE );
GLOBAL_DEF ( bool double_pracs, FALSE );
GLOBAL_DEF ( bool no_wait_questing, FALSE );
GLOBAL_DEF ( bool its_christmas, FALSE );
GLOBAL_DEF ( bool special_day, FALSE );
GLOBAL ( char last_command[MSL] );
GLOBAL ( char last_command2[MSL] );
GLOBAL_DEF ( int last_descriptor, 0 );
GLOBAL_DEF ( int arena, FIGHT_OPEN );

GLOBAL ( char lastwinner[MSL] );
GLOBAL_DEF ( bool is_mass_arena, FALSE );
GLOBAL_DEF ( bool is_mass_arena_fight, FALSE );
//GLOBAL_DEF ( int arenacount, 0 );
GLOBAL_DEF ( int mass_arena_players, 0 );
GLOBAL_DEF ( int mass_arena_players_joined, 0 );
GLOBAL_DEF ( int mass_arena_valid_ticks, 0 );
GLOBAL_DEF ( int mass_arena_ticks, 0 );

GLOBAL_DEF ( bool mass_arena_fticked, 0 );
GLOBAL_DEF ( AREA_DATA * area_first_sorted, NULL );
GLOBAL_DEF ( bool newlock, FALSE );
GLOBAL_DEF ( bool wizlock, FALSE );
GLOBAL_DEF ( bool merc_down, FALSE );

GLOBAL_DEF ( int chain, 0 );

GLOBAL ( char boot_buf[MAX_STRING_LENGTH] );
GLOBAL_DEF ( BAN_DATA * ban_list, NULL );
// GLOBAL_DEF ( NOTE_DATA * note_list, NULL );
GLOBAL_DEF ( CLN_DATA * cln_list, NULL );

GLOBAL_DEF ( char *help_greetinga,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingb,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingc,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingd,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetinge,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingf,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingg,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingh,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingi,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingj,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingk,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingl,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingm,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingn,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingo,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingp,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingq,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingr,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetings,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingt,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingu,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingv,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingx,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingy,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingz,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingaa,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingbb,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingcc,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingdd,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingee,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingff,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetinggg,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetinghh,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingii,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingjj,
             "Welcome to Shattered Dreams" );
GLOBAL_DEF ( char *help_greetingkk,
             "Welcome to Shattered Dreams" );

GLOBAL_DEF ( char *help_authors, "" );
GLOBAL_DEF ( char *help_login, "Who wishes to chase a dream?" );

/* externals for counting purposes */
GLOBAL_DEF ( OBJ_DATA * obj_free, NULL );
GLOBAL_DEF ( CHAR_DATA * char_free, NULL );
GLOBAL_DEF ( DESCRIPTOR_DATA * descriptor_free, NULL );
GLOBAL_DEF ( PC_DATA * pcdata_free, NULL );
GLOBAL_DEF ( AFFECT_DATA * affect_free, NULL );
GLOBAL_DEF ( HELP_DATA * help_free, NULL );

GLOBAL_DEF ( bool fBootDb, FALSE );
GLOBAL ( MOB_INDEX_DATA * mob_index_hash[MAX_KEY_HASH] );
GLOBAL ( OBJ_INDEX_DATA * obj_index_hash[MAX_KEY_HASH] );
GLOBAL_DEF ( FILE * fpArea, NULL );
GLOBAL ( char strArea[MAX_INPUT_LENGTH] );

GLOBAL_DEF ( int nAllocPerm, 0 );
GLOBAL_DEF ( int nAllocString, 0 );

GLOBAL_DEF ( int maxSocial, 0 );    /* Max number of socials */

/*GLOBAL_DEF ( NOTE_DATA * idea_list, NULL );
GLOBAL_DEF ( NOTE_DATA * penalty_list, NULL );
GLOBAL_DEF ( NOTE_DATA * news_list, NULL );
GLOBAL_DEF ( NOTE_DATA * changes_list, NULL );
GLOBAL_DEF ( NOTE_DATA * weddings_list, NULL ); */

GLOBAL ( char str_boot_time[MAX_INPUT_LENGTH] );

GLOBAL_DEF(bool AttemptJump, FALSE);
GLOBAL( jmp_buf jump_env );

/* Stuff by Rhaelar */
GLOBAL_DEF ( int reboot_countdown, -1 );
GLOBAL_DEF ( bool is_reboot_countdown, FALSE );
GLOBAL_DEF ( bool stuff_was_changed, FALSE );
GLOBAL_DEF ( bool debug_mode, FALSE );
GLOBAL_DEF ( int aftermaintenance, 0 );
GLOBAL_DEF ( int max_ever, 0 );
GLOBAL_DEF ( int cur_on, 0 );
GLOBAL_DEF ( int max_on, 0 );
GLOBAL_DEF ( int arena_countdown, 0 );

/* Today stats for 'mudinfo' and others - Rhaelar 12'2004 */
GLOBAL_DEF ( long logins_today, 0 );
GLOBAL_DEF ( int rerolls_today, 0 );
GLOBAL_DEF ( int creations_today, 0 );
GLOBAL_DEF ( long connections_today, 0 );
GLOBAL_DEF ( long commands_today, 0 );
GLOBAL_DEF ( long socials_today, 0 );
GLOBAL_DEF ( long mobkills_today, 0 );
GLOBAL_DEF ( long mobdeaths_today, 0 );
GLOBAL_DEF ( int pkills_today, 0 );
GLOBAL_DEF ( int arenas_today, 0 );
GLOBAL_DEF ( BOUNTY_DATA * bounty_list, NULL );
#endif
