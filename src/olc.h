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

/***************************************************************************
 *  File: olc.h                                                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/
/*
 * This is a header file for all the OLC files.  Feel free to copy it into
 * merc.h if you wish.  Many of these routines may be handy elsewhere in
 * the code.  -Jason Dinkel
 */

/*
 * The version info.  Please use this info when reporting bugs.
 * It is displayed in the game by typing 'version' while editing.
 * Do not remove these from the code - by request of Jason Dinkel
 */
#define VERSION	"ILAB Online Creation Beta 1.0, ROM 2.3 modified]\n\r" \
		"     Port a ROM 2.4 v1.00. Modifications by Lokine of Death Rising.\n\r"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)\n\r" \
                "     Modified for use with ROM 2.3\n\r"        \
                "     By Hans Birkeland (hansbi@ifi.uio.no)\n\r" \
                "     Modificado para uso en ROM 2.4b4a\n\r"	\
                "     Por Birdie (itoledo@ramses.centic.utem.cl)\n\r"
#define DATE	"     (Apr. 7, 1995 - ROM mod, Apr 16, 1995)\n\r" \
		"     (Port a ROM 2.4 - Nov 2, 1996)\n\r" \
		"     Version actual : 1.5a - Mar 9, 1997\n\r"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"

/*
 * New typedefs.
 */
typedef bool OLC_FUN args ( ( CHAR_DATA * ch, char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun

/*
 * Connected states for editor.
 */
#define ED_AREA 1
#define ED_ROOM 2
#define ED_OBJECT 3
#define ED_MOBILE 4
#define ED_MPCODE 5
#define ED_HELP 6
/*
 * Interpreter Prototypes
 */
void aedit args ( ( CHAR_DATA * ch, char *argument ) );
void redit args ( ( CHAR_DATA * ch, char *argument ) );
void medit args ( ( CHAR_DATA * ch, char *argument ) );
void oedit args ( ( CHAR_DATA * ch, char *argument ) );
void mpedit args ( ( CHAR_DATA * ch, char *argument ) );
void    hedit           args( ( CHAR_DATA *, char * ) );

/*
 * OLC Constants
 */
#define MAX_MOB	1               /* Default maximum number for resetting mobs */

/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type
{
    char *const name;
    OLC_FUN *olc_fun;
};

/*
 * Structure for an OLC editor startup command.
 */
struct editor_cmd_type
{
    char *const name;
    DO_FUN *do_fun;
};

/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type aedit_table[];
extern const struct olc_cmd_type redit_table[];
extern const struct olc_cmd_type oedit_table[];
extern const struct olc_cmd_type medit_table[];
extern const struct olc_cmd_type mpedit_table[];
extern const struct olc_cmd_type        hedit_table[];

/*
 * General Functions
 */
bool show_commands args ( ( CHAR_DATA * ch, char *argument ) );
bool show_help args ( ( CHAR_DATA * ch, char *argument ) );
bool edit_done args ( ( CHAR_DATA * ch ) );
bool show_version args ( ( CHAR_DATA * ch, char *argument ) );

/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN ( aedit_show );
DECLARE_OLC_FUN ( aedit_create );
DECLARE_OLC_FUN ( aedit_name );
DECLARE_OLC_FUN ( aedit_file );
DECLARE_OLC_FUN ( aedit_age );
/* DECLARE_OLC_FUN( aedit_recall	);       ROM OLC */
DECLARE_OLC_FUN ( aedit_reset );
DECLARE_OLC_FUN ( aedit_security );
DECLARE_OLC_FUN ( aedit_builder );
DECLARE_OLC_FUN ( aedit_vnum );
DECLARE_OLC_FUN ( aedit_lvnum );
DECLARE_OLC_FUN ( aedit_uvnum );
DECLARE_OLC_FUN ( aedit_credits );
DECLARE_OLC_FUN ( aedit_repop_msg );

/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN ( redit_show );
DECLARE_OLC_FUN ( redit_create );
DECLARE_OLC_FUN ( redit_name );
DECLARE_OLC_FUN ( redit_desc );
DECLARE_OLC_FUN ( redit_ed );
DECLARE_OLC_FUN ( redit_format );
DECLARE_OLC_FUN ( redit_north );
DECLARE_OLC_FUN ( redit_south );
DECLARE_OLC_FUN ( redit_east );
DECLARE_OLC_FUN ( redit_west );
DECLARE_OLC_FUN ( redit_up );
DECLARE_OLC_FUN ( redit_down );
DECLARE_OLC_FUN ( redit_mreset );
DECLARE_OLC_FUN ( redit_oreset );
DECLARE_OLC_FUN ( redit_mlist );
DECLARE_OLC_FUN ( redit_rlist );
DECLARE_OLC_FUN ( redit_olist );
DECLARE_OLC_FUN ( redit_mshow );
DECLARE_OLC_FUN ( redit_oshow );
DECLARE_OLC_FUN ( redit_heal );
DECLARE_OLC_FUN ( redit_mana );
DECLARE_OLC_FUN ( redit_tele );
DECLARE_OLC_FUN ( redit_clan );
DECLARE_OLC_FUN ( redit_owner );

/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN ( oedit_show );
DECLARE_OLC_FUN ( oedit_create );
DECLARE_OLC_FUN ( oedit_name );
DECLARE_OLC_FUN ( oedit_short );
DECLARE_OLC_FUN ( oedit_long );
DECLARE_OLC_FUN ( oedit_addaffect );
DECLARE_OLC_FUN ( oedit_addapply );
DECLARE_OLC_FUN ( oedit_delaffect );
DECLARE_OLC_FUN ( oedit_value0 );
DECLARE_OLC_FUN ( oedit_value1 );
DECLARE_OLC_FUN ( oedit_value2 );
DECLARE_OLC_FUN ( oedit_value3 );
DECLARE_OLC_FUN ( oedit_value4 );   /* ROM */
DECLARE_OLC_FUN ( oedit_weight );
DECLARE_OLC_FUN ( oedit_cost );
DECLARE_OLC_FUN ( oedit_ed );

DECLARE_OLC_FUN ( oedit_extra );    /* ROM */
DECLARE_OLC_FUN ( oedit_wear ); /* ROM */
DECLARE_OLC_FUN ( oedit_type ); /* ROM */
DECLARE_OLC_FUN ( oedit_affect );   /* ROM */
DECLARE_OLC_FUN ( oedit_material ); /* ROM */
DECLARE_OLC_FUN ( oedit_level );    /* ROM */
DECLARE_OLC_FUN ( oedit_condition );    /* ROM */
DECLARE_OLC_FUN ( oedit_autoweapon );
DECLARE_OLC_FUN ( oedit_autoarmor );

/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN ( medit_show );
DECLARE_OLC_FUN ( medit_create );
DECLARE_OLC_FUN ( medit_name );
DECLARE_OLC_FUN ( medit_short );
DECLARE_OLC_FUN ( medit_long );
DECLARE_OLC_FUN ( medit_shop );
DECLARE_OLC_FUN ( medit_desc );
DECLARE_OLC_FUN ( medit_level );
DECLARE_OLC_FUN ( medit_align );
DECLARE_OLC_FUN ( medit_spec );

DECLARE_OLC_FUN ( medit_sex );  /* ROM */
DECLARE_OLC_FUN ( medit_act );  /* ROM */
DECLARE_OLC_FUN ( medit_act2 ); /* ROM */
DECLARE_OLC_FUN ( medit_affect );   /* ROM */
DECLARE_OLC_FUN ( medit_shield );   /*loki */
DECLARE_OLC_FUN ( medit_ac );   /* ROM */
DECLARE_OLC_FUN ( medit_form ); /* ROM */
DECLARE_OLC_FUN ( medit_part ); /* ROM */
DECLARE_OLC_FUN ( medit_imm );  /* ROM */
DECLARE_OLC_FUN ( medit_res );  /* ROM */
DECLARE_OLC_FUN ( medit_vuln ); /* ROM */
DECLARE_OLC_FUN ( medit_material ); /* ROM */
DECLARE_OLC_FUN ( medit_off );  /* ROM */
DECLARE_OLC_FUN ( medit_size ); /* ROM */
DECLARE_OLC_FUN ( medit_hitdice );  /* ROM */
DECLARE_OLC_FUN ( medit_manadice ); /* ROM */
DECLARE_OLC_FUN ( medit_damdice );  /* ROM */
DECLARE_OLC_FUN ( medit_race ); /* ROM */
DECLARE_OLC_FUN ( medit_position ); /* ROM */
DECLARE_OLC_FUN ( medit_gold ); /* ROM */
DECLARE_OLC_FUN ( medit_hitroll );  /* ROM */
DECLARE_OLC_FUN ( medit_damtype );  /* ROM */
DECLARE_OLC_FUN ( medit_group );    /* ROM */
DECLARE_OLC_FUN ( medit_addmprog ); /* ROM */
DECLARE_OLC_FUN ( medit_delmprog ); /* ROM */
DECLARE_OLC_FUN ( medit_autoset );
DECLARE_OLC_FUN ( medit_autoeasy );
DECLARE_OLC_FUN ( medit_autohard );

/* Mobprog editor */

DECLARE_OLC_FUN ( mpedit_create );
DECLARE_OLC_FUN ( mpedit_code );
DECLARE_OLC_FUN ( mpedit_show );

/* Help editor */
DECLARE_OLC_FUN( hedit_keyword          );
DECLARE_OLC_FUN( hedit_text             );
DECLARE_OLC_FUN( hedit_new              );
DECLARE_OLC_FUN( hedit_level            );
DECLARE_OLC_FUN( hedit_delete           );
DECLARE_OLC_FUN( hedit_show             );
DECLARE_OLC_FUN( hedit_list             );

/*
 * Macros
 */

#define IS_SWITCHED( ch )       ( ch->desc->original )  /* ROM OLC */

#define IS_BUILDER(ch, Area)	( !IS_NPC ( ch ) && !IS_SWITCHED( ch ) &&  \
				( ch->pcdata->security >= Area->security  \
				|| strstr( Area->builders, ch->name )	  \
				|| strstr( Area->builders, "All" ) ) )

#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (MOB_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (OBJ_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area)	( Area = (AREA_DATA *)Ch->desc->pEdit )

void show_olc_cmds
args ( ( CHAR_DATA * ch, const struct olc_cmd_type * olc_table ) );
void show_flag_cmds
args ( ( CHAR_DATA * ch, const struct flag_type * flag_table ) );
void show_skill_cmds args ( ( CHAR_DATA * ch, int tar ) );
void show_spec_cmds args ( ( CHAR_DATA * ch ) );
bool check_range args ( ( int lower, int upper ) );
bool change_exit args ( ( CHAR_DATA * ch, char *argument, int door ) );
int wear_loc args ( ( int bits, int count ) );
int wear_bit args ( ( int loc ) );
void show_obj_values args ( ( CHAR_DATA * ch, OBJ_INDEX_DATA * obj ) );
bool set_obj_values
args ( ( CHAR_DATA * ch, OBJ_INDEX_DATA * pObj, int value_num,
         char *argument ) );
bool set_value
args ( ( CHAR_DATA * ch, OBJ_INDEX_DATA * pObj, char *argument, int value ) );
bool oedit_values args ( ( CHAR_DATA * ch, char *argument, int value ) );
