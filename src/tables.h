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

struct flag_type
{
    char *name;
    int bit;
    bool settable;
};

struct clan_type
{
    char *name;
    char *who_name;
    long hall;
    long entrance;
    long pit;
    bool independent;           /* true for loners */
    bool pkill;                 /* true for pkilling clans */
    char *exname;
};

struct position_type
{
    char *name;
    char *short_name;
};

struct sex_type
{
    char *name;
};

struct size_type
{
    char *name;
};

struct clan_titles
{
    char *title_of_rank[3];
};

struct name_amount_type
{
    char *name;
    int amount;
};


/* game tables */
extern const struct clan_type clan_table[MAX_CLAN];
extern const struct position_type position_table[];
extern const struct sex_type sex_table[];
extern const struct size_type size_table[];
extern const struct clan_titles clan_rank_table[];

/* flag tables */
extern const struct flag_type act_flags[];
extern const struct flag_type act2_flags[];
extern const struct flag_type plr_flags[];
extern const struct flag_type plr2_flags[];
extern const struct flag_type affect_flags[];
extern const struct flag_type shield_flags[];
extern const struct flag_type off_flags[];
extern const struct flag_type imm_flags[];
extern const struct flag_type form_flags[];
extern const struct flag_type part_flags[];
extern const struct flag_type comm_flags[];
extern const struct flag_type item_extra[];
extern const struct flag_type item_wear[];
extern const struct flag_type item_weapon[];
extern const struct flag_type extra_flags[];
extern const struct flag_type wear_flags[];
extern const struct flag_type weapon_flags[];
extern const struct flag_type container_flags[];
extern const struct flag_type portal_flags[];
extern const struct flag_type room_flags[];
extern const struct flag_type area_room[];
extern const struct flag_type exit_flags[];
extern const struct flag_type mprog_flags[];
extern const struct flag_type area_flags[];
extern const struct flag_type sex_flags[];
extern const struct flag_type door_resets[];
extern const struct flag_type sector_flags[];
extern const struct flag_type type_flags[];
extern const struct flag_type apply_flags[];
extern const struct flag_type wear_loc_strings[];
extern const struct flag_type wear_loc_flags[];
extern const struct flag_type ac_type[];
extern const struct flag_type size_flags[];
extern const struct flag_type res_flags[];
extern const struct flag_type vuln_flags[];
extern const struct flag_type position_flags[];
extern const struct flag_type weapon_class[];
extern const struct flag_type weapon_type2[];
extern const struct flag_type furniture_flags[];
extern const struct flag_type apply_types[];

