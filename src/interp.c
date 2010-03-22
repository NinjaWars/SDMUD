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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> /* unlink() */
#include "sd.h"
#include "db.h"

bool check_disabled ( const struct cmd_type * command );
DISABLED_DATA *disabled_first;

#define END_MARKER	"END"       /* for load_disabled() and save_disabled() */

/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2

/*
 * Command table.
 */
const struct cmd_type cmd_table[] = {
    /*
     * Common movement commands.
     */
    {"north", do_north, POS_STANDING, 0, 1, LOG_NORMAL, 0},
    {"east", do_east, POS_STANDING, 0, 1, LOG_NORMAL, 0},
    {"south", do_south, POS_STANDING, 0, 1, LOG_NORMAL, 0},
    {"west", do_west, POS_STANDING, 0, 1, LOG_NORMAL, 0},
    {"up", do_up, POS_STANDING, 0, 1, LOG_NORMAL, 0},
    {"down", do_down, POS_STANDING, 0, 1, LOG_NORMAL, 0},

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    {"auction", do_auction, POS_DEAD, 0, 1, LOG_ALWAYS, 1},
    {"ancient", do_ancient, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"at", do_at, POS_DEAD, L6, 1, LOG_NORMAL, 1},
    {"cast", do_cast, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
/*    { "ooc",            do_ooc,         POS_SLEEPING,    0,  1,LOG_NORMAL, 1 },*/
    {"buy", do_buy, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"channels", do_channels, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"exits", do_exits, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"get", do_get, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"goto", do_goto, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"group", do_group, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"guild", do_guild, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"hit", do_kill, POS_FIGHTING, 0, 1, LOG_NORMAL, 0},
    {"inventory", do_inventory, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"kill", do_kill, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"attack", do_kill, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"look", do_look, POS_RESTING, 0, 1, LOG_NORMAL, 1},
/*    {"lscore", do_lscore, POS_SLEEPING, 0, 1, LOG_NORMAL, 1}, */
    {"clan", do_clantalk, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"cwho", do_clanwho, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"home", do_home, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"map", do_map, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"music", do_music, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"mock", do_mock, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"order", do_order, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"practice", do_practice, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"rest", do_rest, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"sit", do_sit, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"soc", do_gocial, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"sockets", do_sockets, POS_DEAD, L7, 1, LOG_NORMAL, 1},
    {"stand", do_stand, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"jog", do_jog, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {".", do_jog, POS_STANDING, 0, 1, LOG_NORMAL, 1},
/*    {"shape", do_shape, POS_STANDING, 0, 1, LOG_NORMAL, 1}, */
    {"tell", do_tell, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"unlock", do_unlock, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"wield", do_wear, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"wizhelp", do_wizhelp, POS_DEAD, 0, 2, LOG_NORMAL, 1},
    {"update", do_update, POS_DEAD, L1, 2, LOG_NORMAL, 1},
    {"hints", do_hints, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"imap", do_imap, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    //{"rmessage", do_rmessage, POS_DEAD, L4, 1, LOG_NORMAL, 1},

    /*
     * Informational commands.
     */
//    {"check", do_unread, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"affects", do_affects, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"areas", do_areas, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"anew", do_anew, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"board", do_board, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"bug", do_bug, POS_DEAD, 0, 1, LOG_NORMAL, 0},
//    {"changes", do_changes, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"commands", do_commands, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"compare", do_compare, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"consider", do_consider, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    //{"count", do_count, POS_SLEEPING, 0, 1, LOG_NORMAL, 1}, 
    {"credits", do_credits, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"copyright", do_copyright, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"equipment", do_equipment, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"lore", do_lore, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"examine", do_examine, POS_RESTING, 0, 1, LOG_NORMAL, 1},
/*  { "groups",		do_groups,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
    {"?", do_help, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"help", do_help, POS_DEAD, 0, 1, LOG_NORMAL, 1},
//    {"idea", do_idea, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"info", do_groups, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"motd", do_motd, POS_DEAD, 0, 1, LOG_NORMAL, 1},
//    {"news", do_news, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"peek", do_peek, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"read", do_read, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"report", do_report, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"rules", do_rules, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"cinfo", do_charinfo, POS_DEAD, 0, 1, LOG_NORMAL, 1},
/*    {"oscore", do_score, POS_DEAD, 0, 1, LOG_NORMAL, 1}, */
//    { "nscore",         do_score_new,   POS_DEAD,        0,  1,  LOG_NORMAL, 1 },
    {"score", do_score, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"scan", do_scan, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"skills", do_skills, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"slist", do_slist, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"socials", do_socials, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"show", do_show, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"spells", do_spells, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"story", do_story, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"time", do_time, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"typo", do_typo, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"weather", do_weather, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"who", do_who, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"whois", do_whois, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"whowas", do_finger, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"wizlist", do_wizlist, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"ancients", do_ancients, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"worth", do_worth, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"version", do_version, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"rtimer", do_rtimer, POS_DEAD, 0, 1, LOG_NORMAL, 1},
//    {"changes", do_changes, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"mudinfo", do_mudinfo, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"mudstatus", do_mudinfo, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"facts", do_mudinfo, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"bounty", do_bounty, POS_DEAD, 0, 1, LOG_NORMAL, 0},
/*
 * Marriage Functions.
 */
/*    {"marry", do_marry, POS_DEAD, IM, 1, LOG_ALWAYS, 1}, 
    {"divorce", do_divorce, POS_DEAD, IM, 1, LOG_ALWAYS, 1}, 
    {"spousetalk", do_spousetalk, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"consent", do_consent, POS_RESTING, 0, 1, LOG_NORMAL, 1}, */

    /*
     * Configuration commands.
     */
    {"alia", do_alia, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"alias", do_alias, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autolist", do_autolist, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autoassist", do_autoassist, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autoexit", do_autoexit, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autogold", do_autogold, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autoloot", do_autoloot, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autosac", do_autosac, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autosplit", do_autosplit, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autostore", do_autostore, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autopeek", do_autopeek, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"brief", do_brief, POS_DEAD, 0, 1, LOG_NORMAL, 1},
/*  { "channels",	do_channels,	POS_DEAD,	 0,  1,  LOG_NORMAL, 1 }, */
    {"colour", do_colour, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"color", do_colour, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"combine", do_combine, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"compact", do_compact, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"description", do_description, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"long", do_long, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"nofollow", do_nofollow, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"noloot", do_noloot, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"nosummon", do_nosummon, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"notran", do_notran, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"noexp", do_noexp, POS_DEAD, 0, 1, LOG_NORMAL, 1},
//    {"outfit", do_outfit, POS_RESTING, IM, 1, LOG_NORMAL, 1},
    {"password", do_password, POS_DEAD, 0, 1, LOG_ALWAYS, 1},
    {"prompt", do_prompt, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"scroll", do_scroll, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"title", do_title, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"unalias", do_unalias, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"wimpy", do_wimpy, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"autoall", do_autoall, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    /*
     * Communication commands.
     */
    {"afk", do_afk, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"answer", do_answer, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"ansi", do_colour, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    /* { "auction",      do_auction,     POS_SLEEPING,    0,  1,  LOG_NORMAL, 1 }, 
     */ {"deaf", do_deaf, POS_DEAD, 0, 1, LOG_NORMAL, 1},
/*    {"cheers", do_cheers, POS_DEAD, 0, 1, LOG_NORMAL, 1}, */
    {"emote", do_emote, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"pmote", do_pmote, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"gmote", do_gmote, POS_RESTING, 0, 1, LOG_NORMAL, 1},
//    {".", do_ooc, POS_SLEEPING, 0, 1, LOG_NORMAL, 0},
    {"ic", do_gossip, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"=", do_cgossip, POS_SLEEPING, 0, 1, LOG_NORMAL, 0},
    {"cgossip", do_cgossip, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"-", do_qgossip, POS_SLEEPING, 0, 1, LOG_NORMAL, 0},
    {"qgossip", do_qgossip, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {",", do_emote, POS_RESTING, 0, 1, LOG_NORMAL, 0},
    {"grats", do_grats, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"gtell", do_gtell, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {";", do_gtell, POS_DEAD, 0, 1, LOG_NORMAL, 0},
/*  { "music",		do_music,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
//    {"note", do_note, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
/*    {"pose", do_pose, POS_RESTING, 0, 1, LOG_NORMAL, 1}, */
    {"iquest", do_iquest, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"ask", do_ask, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"quote", do_quote, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"quiet", do_quiet, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"reply", do_reply, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"replay", do_replay, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"say", do_say, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"'", do_say, POS_RESTING, 0, 1, LOG_NORMAL, 0},
    {"shout", do_shout, POS_RESTING, 0, 1, LOG_NORMAL, 1},
//    {"unread", do_unread, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
//    {"weddings", do_weddings, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
//    {"jokes", do_jokes, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"yell", do_yell, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"ooc", do_ooc, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"chat", do_chat, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"gossip", do_chat, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"gocial", do_gocial, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"rtalk", do_racetalk, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"racetalk", do_racetalk, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"announce", do_announce, POS_DEAD, L4, 1, LOG_NORMAL, 1},
    {"noannounce", do_noannounce, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"pray", do_pray, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"gsocial", do_gocial, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"globalsocial", do_gocial, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    { "spousetalk", do_spousetalk, POS_SLEEPING, 0, 1, LOG_NORMAL, 1 },
    { "propose",    do_propose, POS_STANDING,    0, 1, LOG_NORMAL, 1 },
    { "consent",    do_consent, POS_STANDING,    0, 1, LOG_NORMAL, 1 },
    { "marry",      do_marry,   POS_DEAD,       L5, 1, LOG_NORMAL, 1 },
    { "divorce",    do_divorce, POS_DEAD,       L5, 1, LOG_NORMAL, 1 },
    { "union",      do_union,   POS_STANDING,    0, 1, LOG_NORMAL, 1 },

    /*
     * Object manipulation commands.
     */
    {"brandish", do_brandish, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"close", do_close, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"drink", do_drink, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"drop", do_drop, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"eat", do_eat, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"envenom", do_envenom, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"fill", do_fill, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"give", do_give, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"heal", do_heal, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"repent", do_repent, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"curse", do_curse, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"hold", do_wear, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"list", do_list, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"lock", do_lock, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"open", do_open, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"pick", do_pick, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"pour", do_pour, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"put", do_put, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"quaff", do_quaff, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"recite", do_recite, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"remove", do_remove, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"rm", do_remove, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"sell", do_sell, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"second", do_second, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"take", do_get, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"sacrifice", do_sacrifice, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"trash", do_trash, POS_RESTING, 0, 1, LOG_NORMAL, 0},
    {"cleanup", do_clean, POS_RESTING, 0, 1, LOG_NORMAL, 0},
    {"tap", do_sacrifice, POS_RESTING, 0, 1, LOG_NORMAL, 0},
/*  { "unlock",		do_unlock,	POS_RESTING,	 0,  1,  LOG_NORMAL, 1 }, */
    {"value", do_value, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"verbose", do_verbose, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"wear", do_wear, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"zap", do_zap, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"butcher", do_butcher, POS_RESTING, 0, 1, LOG_NORMAL, 1},

    /*
     * Combat commands.
     */
    {"assassinate", do_assassinate, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"backstab", do_backstab, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"bash", do_bash, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"bs", do_backstab, POS_FIGHTING, 0, 1, LOG_NORMAL, 0},
    {"berserk", do_berserk, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"circle", do_circle, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"call", do_call_wild, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"feed", do_feed, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"dirt", do_dirt, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"disarm", do_disarm, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"flee", do_flee, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"gouge", do_gouge, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"kick", do_kick, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"murde", do_murde, POS_FIGHTING, 0, 1, LOG_NORMAL, 0},
    {"murder", do_murder, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"rescue", do_rescue, POS_FIGHTING, 0, 1, LOG_NORMAL, 0},
    {"surrender", do_surrender, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"trip", do_trip, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"strangle", do_strangle, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"rampage", do_rampage, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    //{ "garrote",        do_garrote,     POS_FIGHTING,    0,  1,  LOG_NORMAL, 1 }, 
    {"ambush", do_ambush, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"stake", do_stake, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"sharpen", do_sharpen, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"nervestrike", do_nervestrike, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"ns", do_nervestrike, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"strike", do_strike, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"eyerub", do_rub, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"thrust", do_thrust, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"whirlwind", do_whirlwind, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
/*    {"bodyslam", do_bodyslam, POS_STANDING, 0, 1, LOG_NORMAL, 1},  */
    {"knee", do_knee, POS_FIGHTING, 0, 1, LOG_NORMAL, 1}, 
    {"windpipe", do_windpipe, POS_FIGHTING, 0, 1, LOG_NORMAL, 1}, 
    {"impale", do_impale, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"push", do_push, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"drag", do_drag, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"gorge", do_gorge, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},

    
    /*
     * Mob command interpreter (placed here for faster scan...)
     */
    {"mob", do_mob, POS_DEAD, 0, LOG_NEVER, 0},

    /*
     * Miscellaneous commands.
     */
    {"enter", do_enter, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"follow", do_follow, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"gain", do_gain, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"go", do_enter, POS_STANDING, 0, 1, LOG_NORMAL, 0},
/*  { "group",		do_group,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
    {"groups", do_groups, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"hide", do_hide, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"explored", do_explored, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"member", do_member, POS_RESTING, 0, 1, LOG_NORMAL, 1},
//    {"play", do_play, POS_RESTING, 0, 1, LOG_NORMAL, 1},
/*  { "practice",	do_practice,	POS_SLEEPING,	 0,  1,  LOG_NORMAL, 1 }, */
    {"qui", do_qui, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"quit", do_quit, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"recall", do_recall, POS_FIGHTING, 0, 1, LOG_NORMAL, 1},
    {"/", do_recall, POS_FIGHTING, 0, 1, LOG_NORMAL, 0},
    {"rent", do_rent, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"save", do_save, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"sleep", do_sleep, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"sneak", do_sneak, POS_STANDING, 0, 1, LOG_NORMAL, 1},
/*    {"swalk", do_swalk, POS_STANDING, 0, 1, LOG_NORMAL, 1},  */
    {"split", do_split, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"steal", do_steal, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"train", do_train, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"track", do_track, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"visible", do_visible, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"wake", do_wake, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"where", do_where, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"donate", do_donate, POS_STANDING, 0, 1, LOG_NORMAL, 1}, 
/*    {"donateclan", do_clan_donate, POS_STANDING, 5, 1, LOG_NORMAL, 1},*/
    {"class", do_class, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
/*    {"forge", do_forge, POS_STANDING, 0, 1, LOG_NORMAL, 0}, */
/*    {"qforge", do_questforge, POS_STANDING, 0, 1, LOG_NORMAL, 1}, */
    {"ignor", do_ignor, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"ignore", do_ignore, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"remember", do_remembe, POS_SLEEPING, 0, 1, LOG_NORMAL, 0},
    {"unignore", do_remember, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"forget", do_forget, POS_SLEEPING, 0, 1, LOG_NORMAL, 1}, 
    {"voodoo", do_voodoo, POS_STANDING, 20, 1, LOG_NORMAL, 1},
    {"backup", do_backup, POS_DEAD, 0, 1, LOG_ALWAYS, 1},
    {"game", do_game, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    /*   { "slots",          do_slots,       POS_STANDING,    0,  1,  LOG_NORMAL, 1 }, */
/*    { "highdice",       do_high_dice,   POS_STANDING,    0,  1,  LOG_NORMAL, 1 },*/
    {"quest", do_quest, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    //{ "gquest",         do_gquest,      POS_RESTING,     0,  1,  LOG_NORMAL,  1 },
/*    {"reform", do_reform, POS_RESTING, 0, 1, LOG_ALWAYS, 1}, */
    {"aqpgive", do_aqpgive, POS_RESTING, 0, 1, LOG_ALWAYS, 1},
    {"iqpgive", do_iqpgive, POS_RESTING, 0, 1, LOG_ALWAYS, 1},
    {"decline", do_decline, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"bet", do_bet, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"accept", do_agree, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"challenge", do_challenge, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    //{"dismiss", do_dismiss, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"loner", do_loner, POS_STANDING, 0, 1, LOG_ALWAYS, 1},
    {"pknewbies", do_pknewbies, POS_STANDING, 0, 1, LOG_ALWAYS, 0},
    {"clear", do_clear, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
//    {"randomquote", do_qquote, POS_SLEEPING, 0, 1, LOG_NORMAL, 1},
    {"tournament", do_tournament, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"heel", do_heel, POS_RESTING, 0, 1, LOG_NORMAL, 1},
    {"ftag", do_ftag, POS_SLEEPING, 0, 1,  LOG_NORMAL, 1 },
    {"tag", do_tag, POS_RESTING, 0, 1, LOG_NORMAL, 1 },
    {"red", do_red, POS_RESTING, 0, 1, LOG_NORMAL, 1 },
    {"blue", do_blue, POS_RESTING, 0, 1, LOG_NORMAL, 1 },
    {"mobdeaths", do_mobdeaths, POS_SLEEPING, 0, 1, LOG_NORMAL, 1 },
    {"mobkills", do_mobkills, POS_SLEEPING, 0, 1, LOG_NORMAL, 1 },
    {"stance", do_stance, POS_FIGHTING, 0, 1, LOG_NORMAL, 1 },
    {"sstat", do_sstat, POS_DEAD, 0, 1, LOG_NORMAL, 1 },
    {"stances", do_stances, POS_DEAD, 0, 1, LOG_NORMAL, 1 },
    /*
     * Immortal commands.
     */
/*    {"avatar", do_avator, POS_DEAD, L2, 1, LOG_ALWAYS, 1}, */
    {"itake", do_itake, POS_DEAD, L5, 1, LOG_ALWAYS, 1},
    /*{"copyove", do_copyove, POS_DEAD, L2, 1, LOG_ALWAYS, 0},
    {"copyover", do_copyover, POS_DEAD, L2, 1, LOG_ALWAYS, 1},*/
    {"autoreboot", do_autoreboot, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"abort", do_abort, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"advance", do_advance, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"dump", do_dump, POS_DEAD, ML, 1, LOG_ALWAYS, 0},
    {"trust", do_trust, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"violate", do_violate, POS_DEAD, L2, 1, LOG_ALWAYS, 1},

    {"allow", do_allow, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"ban", do_ban, POS_DEAD, L3, 1, LOG_ALWAYS, 1},
    {"bane", do_bane, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"bank", do_bank, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"deny", do_deny, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"idisconnect", do_disconnect, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"dupe", do_dupe, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    {"finger", do_finger, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"flag", do_flag, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"freeze", do_freeze, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"passwd", do_passwd, POS_DEAD, L1, 1, LOG_ALWAYS, 1},
    {"permban", do_permban, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
/*    {"protect", do_protect, POS_DEAD, L2, 1, LOG_ALWAYS, 1}, */
    {"reboo", do_reboo, POS_DEAD, L1, 1, LOG_ALWAYS, 0},
//    {"reboot", do_reboot, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"reboot", do_reboot, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"set", do_set, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"shutdow", do_shutdow, POS_DEAD, L1, 1, LOG_ALWAYS, 0},
    {"shutdown", do_shutdown, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
/*  { "sockets",	do_sockets,	POS_DEAD,	L2,  1,  LOG_NORMAL, 1 }, */
    {"wedpost", do_wedpost, POS_DEAD, L1, 1, LOG_ALWAYS, 1},
    {"wizlock", do_wizlock, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"force", do_force, POS_DEAD, L5, 1, LOG_ALWAYS, 1},
    {"forcesilent", do_forcesilent, POS_DEAD, L3, 1, LOG_ALWAYS, 1},
    {"load", do_load, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"newlock", do_newlock, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"nochannels", do_nochannels, POS_DEAD, L7, 1, LOG_ALWAYS, 1},
    {"noemote", do_noemote, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"noclan", do_noclan, POS_DEAD, L3, 1, LOG_ALWAYS, 1},
    {"norestore", do_norestore, POS_DEAD, L3, 1, LOG_ALWAYS, 1},
    {"noshout", do_noshout, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"note", do_note, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"notell", do_notell, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"noarena", do_noarena, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"nogsocial", do_nogsocial, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"nosay", do_nosay, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"notitle", do_notitle, POS_DEAD, L3, 1, LOG_ALWAYS, 1},
    {"pack", do_pack, POS_DEAD, IM, 2, LOG_ALWAYS, 1},
    {"pecho", do_pecho, POS_DEAD, L5, 1, LOG_ALWAYS, 1},
    {"purge", do_purge, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"restore", do_restore, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"sla", do_sla, POS_DEAD, L5, 1, LOG_ALWAYS, 0},
    {"slay", do_slay, POS_DEAD, L5, 1, LOG_ALWAYS, 1},
    {"showskill", do_showskill, POS_DEAD, 1, 1, LOG_NORMAL, 1},
    {"showclass", do_showclass, POS_DEAD, 1, 1, LOG_NORMAL, 1},
    {"showrace", do_showrace, POS_DEAD, 1, 1, LOG_NORMAL, 1},
    {"teleport", do_transfer, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"disable", do_disable, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"pickup", do_pickup, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"uptime", do_uptime, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    {"repeat", do_repeat, POS_DEAD, L13, 1, LOG_ALWAYS, 1},
    {"transfer", do_transfer, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"corner", do_corner, POS_DEAD, L7, 1, LOG_ALWAYS, 1},
    {"jail", do_jail, POS_DEAD, L1, 1, LOG_ALWAYS, 1},
    {"twit", do_twit, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"permtwit", do_permtwit, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"pardon", do_pardon, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"pkviolent", do_violent, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"pksafe", do_safe, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"promote", do_promote, POS_DEAD, 0, 1, LOG_NORMAL, 1},
/*  { "at",		do_at,		POS_DEAD,	L6,  1,  LOG_NORMAL, 1 }, */
    {"poofin", do_bamfin, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"poofout", do_bamfout, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"gecho", do_echo, POS_DEAD, L5, 1, LOG_ALWAYS, 1},
/*  { "goto",		do_goto,	POS_DEAD,	L8,  1,  LOG_NORMAL, 1 }, */
    {"ghost", do_ghost, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"holylight", do_holylight, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"incognito", do_incognito, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"invis", do_invis, POS_DEAD, IM, 1, LOG_NORMAL, 0},
    {"log", do_log, POS_DEAD, L1, 1, LOG_ALWAYS, 1},
    {"memory", do_memory, POS_DEAD, L7, 1, LOG_NORMAL, 1},
    {"mwhere", do_mwhere, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"owhere", do_owhere, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"mlevel", do_mlevel, POS_DEAD, ML, 1, LOG_NORMAL, 1},
    {"olevel", do_olevel, POS_DEAD, ML, 1, LOG_NORMAL, 1},
    {"peace", do_peace, POS_DEAD, L13, 1, LOG_NORMAL, 1},
    {"allpeace", do_allpeace, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
//    {"penalty", do_penalty, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"echo", do_recho, POS_DEAD, L8, 1, LOG_ALWAYS, 1},
/*    {"return", do_return, POS_DEAD, ML, 1, LOG_NORMAL, 1}, */
    {"snoop", do_snoop, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"stat", do_stat, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"string", do_string, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
/*    {"switch", do_switch, POS_DEAD, ML, 1, LOG_ALWAYS, 1}, */
    {"wizinvis", do_invis, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"wecho", do_wecho, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"vnum", do_vnum, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"zecho", do_zecho, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"clead", do_clead, POS_DEAD, L1, 1, LOG_ALWAYS, 1},
//    {"randclan", do_randclan, POS_DEAD, L1, 1, LOG_NORMAL, 1},
    {"clone", do_clone, POS_DEAD, IM, 1, LOG_ALWAYS, 1},

    {"immtalk", do_immtalk, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"wiznet", do_wiznet, POS_DEAD, IM, 2, LOG_NORMAL, 1},
    {"immchat", do_immtalk, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"imotd", do_imotd, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {":", do_immtalk, POS_DEAD, IM, 1, LOG_NORMAL, 0},
    {"shortestpath", do_shortestpath, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"skillstat", do_skillstat, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"spellstat", do_spellstat, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"vlist", do_vlist, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"freevnums", do_freevnums, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"otype", do_otype, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"smote", do_smote, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"prefi", do_prefi, POS_DEAD, IM, 1, LOG_NORMAL, 0},
    {"prefix", do_prefix, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"mpoint", do_mpoint, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"mquest", do_mquest, POS_DEAD, IM, 1, LOG_NORMAL, 1},
//    {"sign", do_sign, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"immkiss", do_immkiss, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"squire", do_squire, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"knight", do_knight, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"recover", do_recover, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"gset", do_gset, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"wizslap", do_wizslap, POS_DEAD, L10, 1, LOG_ALWAYS, 1},
    {"wipe", do_wipe, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"mpdump", do_mpdump, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"mpstat", do_mpstat, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"omni", do_omni, POS_DEAD, IM, 1, LOG_NORMAL, 1},
    {"bonus", do_bonus, POS_DEAD, L3, 1, LOG_ALWAYS, 1},
    {"award", do_award, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    {"vape", do_vape, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"addlag", do_addlag, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    /*{ "sever",          do_sever,       POS_DEAD,       IM,  1,  LOG_NORMAL, 1}, */
//    {"spellup", do_qspell, POS_DEAD, L13, 1, LOG_ALWAYS, 1},
    {"questreset", do_questreset, POS_DEAD, L13, 1, LOG_ALWAYS, 1},
    {"pload", do_pload, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"punload", do_punload, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"addapply", do_addapply, POS_DEAD, L5, 1, LOG_NORMAL, 1},
    {"setxp", do_resetxp, POS_DEAD, L13, 1, LOG_ALWAYS, 1},
    {"rename", do_rename, POS_DEAD, L6, 1, LOG_ALWAYS, 1},
    {"doublexp", do_doublexp, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"doubledamage", do_doubledamage, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"doubleqp", do_doubleqp, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"doublestance", do_doublestance, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"doublepracs", do_doublepracs, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"astat", do_astat, POS_DEAD, L6, 1, LOG_NORMAL, 1},
    {"alock", do_alock, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"abusy", do_abusy, POS_DEAD, L5, 1, LOG_ALWAYS, 1},
    {"aclear", do_aclear, POS_DEAD, L13, 1, LOG_ALWAYS, 1},
    {"cscore", do_cscore, POS_DEAD, L13, 1, LOG_ALWAYS, 1},
    {"ifollow", do_ifollow, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"arealinks", do_arealinks, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    {"idle", do_idle, POS_DEAD, L2, 1, LOG_NORMAL, 1},
    {"nameban", do_nameban, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"pull", do_pull, POS_DEAD, L12, 1, LOG_ALWAYS, 1},
    {"iwelcome", do_iwelcome, POS_DEAD, L3, 1, LOG_ALWAYS, 1},
//    {"itournament", do_itournament, POS_DEAD, L7, 1, LOG_ALWAYS, 1},
    {"nowaitquest", do_nowaitquesting, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"alldoubles", do_alldoubles, POS_DEAD, L2, 1, LOG_ALWAYS, 1},
    {"toggle", do_toggle, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"scatter", do_scatter, POS_DEAD, IM, 1, LOG_ALWAYS, 1},
    //{"addchange", do_addchange, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"last", do_last, POS_DEAD, L6, 1, LOG_ALWAYS, 1},    
    //{"mudinfo", do_mudinfo, POS_DEAD, ML, 1, LOG_ALWAYS, 1},    
    {"damnoun", do_damnoun, POS_DEAD, L4, 1, LOG_ALWAYS, 1},
    {"mudset", do_mudset, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"helper", do_helper, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"inactive", do_inactive, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
    {"ps", do_ps, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
//    {"multiplay", do_multiplay, POS_DEAD, ML, 1, LOG_ALWAYS, 1},
   
    /*    
     * OLC   
     */
    {"edit", do_olc, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"asave", do_asave, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"alist", do_alist, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"resets", do_resets, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"redit", do_redit, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"medit", do_medit, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"aedit", do_aedit, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"oedit", do_oedit, POS_DEAD, IM, 0, LOG_NORMAL, 1},
    {"mpedit", do_mpedit, POS_DEAD, IM, 0, LOG_NORMAL, 1},
//    {"msedit", do_msedit, POS_DEAD, 0, LOG_NORMAL, 1},
    {"sedit", do_sedit, POS_DEAD, L12, 1, LOG_ALWAYS, 1},
    {"mplist", do_mplist, POS_DEAD, L12, 1, LOG_NORMAL, 1},
    {"hedit", do_hedit, POS_DEAD, ML, 1, LOG_ALWAYS, 1},    
    /*
     * Commands needed last on list
     */
    {"restring", do_restring, POS_STANDING, 0, 1, LOG_ALWAYS, 1},
    {"delet", do_delet, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"delete", do_delete, POS_STANDING, 0, 1, LOG_NORMAL, 1},
    {"rerol", do_rerol, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"reroll", do_reroll, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"immortals", do_wizlist, POS_DEAD, 0, 1, LOG_NORMAL, 1},
    {"immlist", do_wizlist, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"staff", do_wizlist, POS_DEAD, 0, 1, LOG_NORMAL, 0},
    {"crew", do_wizlist, POS_DEAD, 0, 1, LOG_NORMAL, 0},

    /*
     * End of list.
     */
    {"", 0, POS_DEAD, 0, 1, LOG_NORMAL, 0}
};

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret ( CHAR_DATA * ch, char *argument )
{
    if ( !ch ) return;
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;
    int string_count = nAllocString;
    int perm_count = nAllocPerm;    
    
    /*
     * To avoid tilde bugs.
     * -Rhaelar 05'2004
     */
    smash_tilde ( argument );

    sprintf ( last_command, "%s in room[%ld]: %s.", ch->name,
              ch->in_room->vnum, argument );
    if ( ch->desc != NULL )
    {
        sprintf ( last_command2, "%s", argument );
        last_descriptor = ch->desc->descriptor;
    }
    else
    {
        last_command2[0] = '\0';
    }

    /*
     * Strip leading spaces.
     */
    while ( isspace ( *argument ) )
        argument++;
    if ( argument[0] == '\0' )
        return;

    /*
     * No hiding.
     */
    affect_strip ( ch, gsn_hide );
    REMOVE_BIT ( ch->affected_by, AFF_HIDE );

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_FREEZE ) )
    {
        send_to_char ( "You're totally frozen!\n\r", ch );
        return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy ( logline, argument );
    if ( !isalpha ( argument[0] ) && !isdigit ( argument[0] ) )
    {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while ( isspace ( *argument ) )
            argument++;
    }
    else
    {
        argument = one_argument ( argument, command );
    }

/* Don't allow people to do anything else than toggle AFK when they are AFK - Rhaelar */
    if ( str_cmp( command, "afk" ) && ( ch->position != POS_FIGHTING ) && IS_SET ( ch->comm, COMM_AFK ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "Not while you're still AFK. Type 'afk' to toggle AFK on/off.\n\r", ch );
        return;
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust ( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == cmd_table[cmd].name[0] &&
             !str_prefix ( command, cmd_table[cmd].name ) &&
             cmd_table[cmd].level <= trust )
        {
            if ( cmd_table[cmd].tier == 1 )
            {
                found = TRUE;
                break;
            }
            else if ( ch->class >= MAX_CLASS / 2 )
            {
                found = TRUE;
                break;
            }
            else if ( ch->level >= LEVEL_ANCIENT )
            {
                found = TRUE;
                break;
            }
        }
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
        strcpy ( logline, "" );

    if ( ( !IS_NPC ( ch ) && ( IS_SET ( ch->act, PLR_LOG ) || 
	 IS_SET ( ch->act2, PLR2_PKVIOLENT ) ||
	 IS_SET ( ch->act2, PLR2_PKSAFE ) ) ) || fLogAll ||
         ( cmd_table[cmd].log == LOG_ALWAYS && ch->level != MAX_LEVEL ) )
    {
        sprintf ( log_buf, "Log %s: %s", ch->name, logline );
        wiznet ( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust ( ch ) );
        log_string ( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
        write_to_buffer ( ch->desc->snoop_by, "% ", 2 );
        write_to_buffer ( ch->desc->snoop_by, logline, 0 );
        write_to_buffer ( ch->desc->snoop_by, "\n\r", 2 );
    }

    if ( !found )
    {
        /*
         * Look for command in socials table.
         */
        if ( !check_social ( ch, command, argument ) )
            send_to_char ( "That command does not exist. Try again, without typos this time.\n\r",
                           ch );
	else
	socials_today++;
        return;
    }
    else                        /* a normal valid command.. check if it is
                                   disabled */ if ( check_disabled ( &cmd_table[cmd] ) &&
                                                    !is_name ( ch->name,
                                                               "Rhaelar" ) 
)
    {
        send_to_char ( "That command has been disabled. Try again later.\n\r", ch );
        return;
    }


    commands_today++;
    
    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
        switch ( ch->position )
        {
            case POS_DEAD:
                send_to_char ( "Lie still; you are DEAD.\n\r", ch );
                break;

            case POS_MORTAL:
            case POS_INCAP:
                send_to_char ( "You are hurt far too bad for that.\n\r", ch );
                break;

            case POS_STUNNED:
                send_to_char ( "You are too stunned to do that.\n\r", ch );
                break;

            case POS_SLEEPING:
                send_to_char ( "In your dreams, or what?\n\r", ch );
                break;

            case POS_RESTING:
                send_to_char ( "Nah... You feel too relaxed...\n\r", ch );
                break;

            case POS_SITTING:
                send_to_char ( "Better stand up first.\n\r", ch );
                break;

            case POS_FIGHTING:
                send_to_char ( "No way!  You are still fighting!\n\r", ch );
                break;

        }
        return;
    }

    /*
     * Dispatch the command.
     */
    ( *cmd_table[cmd].do_fun ) ( ch, argument );

    if ( string_count != nAllocString )
    {
        sprintf ( log_buf, "%s in strings :: %s : %s. %d to %d, a%s of %d",
                  ( string_count < nAllocString ? "Increase" : "Decrease" ),
                  ch->name, cmd_table[cmd].name, string_count, nAllocString,
                  ( string_count <
                    nAllocString ? "n increase" : " decrease" ),
                  abs ( ( nAllocString - string_count ) ) );
        wiznet ( log_buf, NULL, NULL, WIZ_MEM, 0, 0 );
    }
    if ( perm_count != nAllocPerm )
    {
        sprintf ( log_buf, "%s in perms :: %s : %s. %d to %d, a%s of %d",
                  ( perm_count < nAllocPerm ? "Increase" : "Decrease" ),
                  ch->name, cmd_table[cmd].name, perm_count, nAllocPerm,
                  ( perm_count < nAllocPerm ? "n increase" : " decrease" ),
                  abs ( ( nAllocPerm - perm_count ) ) );
        wiznet ( log_buf, NULL, NULL, WIZ_MEM, 0, 0 );
    }

    tail_chain (  );
    return;
}

bool check_social ( CHAR_DATA * ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == social_table[cmd].name[0] &&
             !str_prefix ( command, social_table[cmd].name ) )
        {
            found = TRUE;
            break;
        }
    }

    if ( !found )
        return FALSE;

    if ( !IS_NPC ( ch ) && IS_SET ( ch->comm, COMM_NOEMOTE ) )
    {
        send_to_char ( "You are anti-social!\n\r", ch );
        return TRUE;
    }

    switch ( ch->position )
    {
        case POS_DEAD:
            send_to_char ( "Lie still; you are DEAD.\n\r", ch );
            return TRUE;

        case POS_INCAP:
        case POS_MORTAL:
            send_to_char ( "You are hurt far too bad for that.\n\r", ch );
            return TRUE;

        case POS_STUNNED:
            send_to_char ( "You are too stunned to do that.\n\r", ch );
            return TRUE;

        case POS_SLEEPING:
            /*
             * I just know this is the path to a 12" 'if' statement.  :(
             * But two players asked for it already!  -- Furey
             */
            if ( !str_cmp ( social_table[cmd].name, "snore" ) )
                break;
            send_to_char ( "In your dreams, or what?\n\r", ch );
            return TRUE;

    }

    one_argument ( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
        act ( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM );
        act ( social_table[cmd].char_no_arg, ch, NULL, victim, TO_CHAR );
    }
    else if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
        act ( social_table[cmd].others_auto, ch, NULL, victim, TO_ROOM );
        act ( social_table[cmd].char_auto, ch, NULL, victim, TO_CHAR );
    }
    else
    {
        act ( social_table[cmd].others_found, ch, NULL, victim, TO_NOTVICT );
        act ( social_table[cmd].char_found, ch, NULL, victim, TO_CHAR );
        act ( social_table[cmd].vict_found, ch, NULL, victim, TO_VICT );

        if ( !IS_NPC ( ch ) && IS_NPC ( victim ) &&
             !IS_AFFECTED ( victim, AFF_CHARM ) && IS_AWAKE ( victim ) &&
             victim->desc == NULL )
        {
            switch ( number_bits ( 4 ) )
            {
                case 0:

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                    act ( social_table[cmd].others_found, victim, NULL, ch,
                          TO_NOTVICT );
                    act ( social_table[cmd].char_found, victim, NULL, ch,
                          TO_CHAR );
                    act ( social_table[cmd].vict_found, victim, NULL, ch,
                          TO_VICT );
                    break;

                case 9:
                case 10:
                case 11:
                case 12:
                    act ( "$n slaps $N.", victim, NULL, ch, TO_NOTVICT );
                    act ( "You slap $N.", victim, NULL, ch, TO_CHAR );
                    act ( "$n slaps you.", victim, NULL, ch, TO_VICT );
                    break;
            }
        }
    }

    return TRUE;
}

/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{

    if ( *arg == '\0' )
        return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit ( *arg ) )
            return FALSE;
    }

    return TRUE;
}

/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument ( char *argument, char *arg )
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '.' )
        {
            *pdot = '\0';
            number = atoi ( argument );
            *pdot = '.';
            strcpy ( arg, pdot + 1 );
            return number;
        }
    }

    strcpy ( arg, argument );
    return 1;
}

/* 
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument ( char *argument, char *arg )
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi ( argument );
            *pdot = '*';
            strcpy ( arg, pdot + 1 );
            return number;
        }
    }

    strcpy ( arg, argument );
    return 1;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument ( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace ( *argument ) )
        argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
        cEnd = *argument++;

    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        *arg_first = LOWER ( *argument );
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while ( isspace ( *argument ) )
        argument++;

    return argument;
}

CH_CMD ( do_commands )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
    int count = 0;
    col = 0;
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level < LEVEL_ANCIENT &&
             cmd_table[cmd].level <= get_trust ( ch ) && cmd_table[cmd].show )
        {
            if ( cmd_table[cmd].tier == 1 )
            {
                sprintf ( buf, "%-12s", cmd_table[cmd].name );
                send_to_char ( buf, ch );
                if ( ++col % 7 == 0 )
                    send_to_char ( "\n\r", ch );
		++count;
            }
            else if ( ch->class >= MAX_CLASS / 2 )
            {
                sprintf ( buf, "%-12s", cmd_table[cmd].name );
                send_to_char ( buf, ch );
                if ( ++col % 7 == 0 )
                    send_to_char ( "\n\r", ch );
		++count;
            }
            else if ( ch->level >= LEVEL_ANCIENT )
            {
                sprintf ( buf, "%-12s", cmd_table[cmd].name );
                send_to_char ( buf, ch );
                if ( ++col % 7 == 0 )
                    send_to_char ( "\n\r", ch );
	        ++count;
            }
        }
    }

    if ( col % 6 != 0 )
    	send_to_char ( "\n\r", ch );
	printf_to_char( ch, "\n\r\n\r{RCommands found{x: {Y%d{x\n\r", count );
    return;
}
/*
CH_CMD ( do_wizhelp )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
    int count;

    printf_to_char ( ch,
                     "{Y------------------------------{cImmortal Commands{Y------------------------------{x\n\r" );

    col = 0;
    if ( ( ch->class < MAX_CLASS / 2 ) &&
         ( get_trust ( ch ) < LEVEL_ANCIENT ) )
    {
        return;
    }
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( ( cmd_table[cmd].level >= LEVEL_ANCIENT &&
               cmd_table[cmd].level <= get_trust ( ch ) &&
               cmd_table[cmd].show ) || ( ( cmd_table[cmd].tier == 2 ) &&
                                          ( cmd_table[cmd].level <=
                                            get_trust ( ch ) ) &&
                                          cmd_table[cmd].show ) )

        {
            sprintf ( buf, "{r%-12s{x", cmd_table[cmd].name );
            send_to_char ( buf, ch );
            if ( ++col % 7 == 0 )
                send_to_char ( "\n\r", ch );
	    ++count;
        }
    }

    if ( col % 7 != 0 )
        send_to_char ( "\n\r", ch );
    printf_to_char ( ch,
                     "{cRCommands found{x: {Y%s{x\n\r", count );
    printf_to_char ( ch,
                     "{Y-------------------------------------------------------------------------------{x" );
    return;
}
*/

CH_CMD ( do_wizhelp )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;
    int clevel;
    int count = 0;
    col = 0;

    for( clevel = LEVEL_HERO + 1; clevel < MAX_LEVEL + 1; clevel++ ) 
    {
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
	{ 
            if ( cmd_table[cmd].level >= LEVEL_HERO
            &&   cmd_table[cmd].level <= get_trust( ch ) 
            &&   cmd_table[cmd].show
	    &&   cmd_table[cmd].level == clevel)
	    {
	        sprintf( buf, "{c[{x%-3d{c] %-12s{x", cmd_table[cmd].level, cmd_table[cmd].name );
	        send_to_char( buf, ch );
	        if ( ++col % 6 == 0 )
		    send_to_char( "\n\r", ch );
		++count;
	    }
	}
    }
 
    if ( col % 6 != 0 )
    send_to_char ( "\n\r", ch );
	printf_to_char( ch, "\n\r{RCommands found{x: {Y%d{x\n\r", count );
    return;
}

CH_CMD ( do_disable )
{
	int i;
	DISABLED_DATA *p,*q;
	char buf[100];
	
	if (IS_NPC(ch))
	{
		send_to_char ("RETURN first.\n\r",ch);
		return;
	}

	if ( ch->pcdata->security < 9 )
	{
		send_to_char ("You're not allowed.\n\r",ch);
		return;
	}
	
	if (!argument[0]) /* Nothing specified. Show disabled commands. */
	{
		if (!disabled_first) /* Any disabled at all ? */
		{
			send_to_char ("There are no commands disabled.\n\r",ch);
			return;
		}

		send_to_char ("Disabled commands:\n\r"
		              "Command      Level   Disabled by\n\r",ch);
		                
		for (p = disabled_first; p; p = p->next)
		{
			sprintf (buf, "%-12s %5d   %-12s\n\r",p->command->name, p->level, p->disabled_by);
			send_to_char (buf,ch);
		}
		return;
	}
	
	/* command given */

	/* First check if it is one of the disabled commands */
	for (p = disabled_first; p ; p = p->next)
		if (!str_cmp(argument, p->command->name))
			break;
			
	if (p) /* this command is disabled */
	{
	/* Optional: The level of the imm to enable the command must equal or exceed level
	   of the one that disabled it */
	
		if (get_trust(ch) < p->level)
		{
			send_to_char ("This command was disabled by a higher power.\n\r",ch);
			return;
		}
		
		/* Remove */
		
		if (disabled_first == p) /* node to be removed == head ? */
			disabled_first = p->next;
		else /* Find the node before this one */
		{
			for (q = disabled_first; q->next != p; q = q->next); /* empty for */
			q->next = p->next;
		}
		
		free_string (p->disabled_by); /* free name of disabler */
		free_mem (p,sizeof(DISABLED_DATA)); /* free node */
		save_disabled(); /* save to disk */
		send_to_char ("Command enabled.\n\r",ch);
	}
	else /* not a disabled command, check if that command exists */
	{
		/* IQ test */
		if (!str_cmp(argument,"disable"))
		{
			send_to_char ("You cannot disable the disable command.\n\r",ch);
			return;
		}

		/* Search for the command */
		for (i = 0; cmd_table[i].name[0] != '\0'; i++)
			if (!str_cmp(cmd_table[i].name, argument))
				break;

		/* Found? */				
		if (cmd_table[i].name[0] == '\0')
		{
			send_to_char ("No such command.\n\r",ch);
			return;
		}

		/* Can the imm use this command at all ? */				
		if (cmd_table[i].level > get_trust(ch))
		{
			send_to_char ("You dot have access to that command; you cannot disable it.\n\r",ch);
			return;
		}
		
		/* Disable the command */
		
		p = alloc_mem (sizeof(DISABLED_DATA));

		p->command = &cmd_table[i];
		p->disabled_by = str_dup (ch->name); /* save name of disabler */
		p->level = get_trust(ch); /* save trust */
		p->next = disabled_first;
		disabled_first = p; /* add before the current first element */
		
		send_to_char ("Command disabled.\n\r",ch);
		save_disabled(); /* save to disk */
	}
}

bool check_disabled (const struct cmd_type *command)
{
	DISABLED_DATA *p;
	
	for (p = disabled_first; p ; p = p->next)
		if (p->command->do_fun == command->do_fun)
			return TRUE;

	return FALSE;
}

/* Load disabled commands */
void load_disabled()
{
	FILE *fp=NULL;
	DISABLED_DATA *p;
	char *name;
	int i;
	
	disabled_first = NULL;
	
	fp = fopen (DISABLED_FILE, "r");
	
	if (!fp) /* No disabled file.. no disabled commands : */
		return;
		
	name = fread_word (fp);
	
	while (str_cmp(name, END_MARKER)) /* as long as name is NOT END_MARKER :) */
	{
		/* Find the command in the table */
		for (i = 0; cmd_table[i].name[0] ; i++)
			if (!str_cmp(cmd_table[i].name, name))
				break;
				
		if (!cmd_table[i].name[0]) /* command does not exist? */
		{
			bug ("Skipping uknown command in " DISABLED_FILE " file.",0);
			fread_number(fp); /* level */
			fread_word(fp); /* disabled_by */
		}
		else /* add new disabled command */
		{
			p = alloc_mem(sizeof(DISABLED_DATA));
			p->command = &cmd_table[i];
			p->level = fread_number(fp);
			p->disabled_by = str_dup(fread_word(fp)); 
			p->next = disabled_first;
			
			disabled_first = p;

		}
		
		name = fread_word(fp);
	}

	new_fclose (fp);		
}

void save_disabled()
{
	FILE *fp=NULL;
	DISABLED_DATA *p;
	
	if (!disabled_first) /* delete file if no commands are disabled */
	{
		unlink (DISABLED_FILE);
		return;
	}
	
	fp = fopen (DISABLED_FILE, "w");
	
	if (!fp)
	{
		bug ("Could not open " DISABLED_FILE " for writing",0);
		return;
	}
	
	for (p = disabled_first; p ; p = p->next)
		fprintf (fp, "%s %d %s\n", p->command->name, p->level, p->disabled_by);
		
	fprintf (fp, "%s\n",END_MARKER);
		
	new_fclose (fp);
}

