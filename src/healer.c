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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sd.h"
#include "magic.h"

CH_CMD ( do_heal )
{
    CHAR_DATA *mob;
    char arg[MAX_INPUT_LENGTH];
    int cost, sn;
    SPELL_FUN *spell;
    char *words;

    /* check for healer */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC ( mob ) && IS_SET ( mob->act, ACT_IS_HEALER ) )
            break;
    }

    if ( mob == NULL )
    {
        send_to_char ( "You can't do that here.\n\r", ch );
        return;
    }

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* display price list */
        act ( "$N says '{aI offer the following spells:{x'", ch, NULL, mob,
              TO_CHAR );
        send_to_char
            ( "  {Wlight  {x:{c cure light wounds{y       10 {Ygold{x\n\r",
              ch );
        send_to_char
            ( "  {Wserious{x:{c cure serious wounds{y     15 {Ygold{x\n\r",
              ch );
        send_to_char
            ( "  {Wcritic{x : {ccure critical wounds{y    25 {Ygold{x\n\r",
              ch );
        send_to_char
            ( "  {Wheal{x   : {chealing spell{y  	   50 {Ygold{x\n\r", ch );
        send_to_char
            ( "  {Wblind{x  : {ccure blindness{y          20 {Ygold{x\n\r",
              ch );
        send_to_char
            ( "  {Wdisease{x:{c cure disease{y            15{Y gold{x\n\r",
              ch );
        send_to_char
            ( "  {Wpoison{x : {ccure poison{y	           25 {Ygold{x\n\r",
              ch );
        send_to_char
            ( "  {Wuncurse{x:{c remove curse  {y          50 {Ygold{x\n\r",
              ch );
        send_to_char
            ( "  {Wrefresh{x:{c restore movement     {y    5 {Ygold{x\n\r",
              ch );
        send_to_char
            ( "  {Wmana{x   : {crestore mana	  {y         10 {Ygold{x\n\r",
              ch );
        //       send_to_char("  acid   : acidshield             4 plat\n\r",ch);
//      send_to_char("  shock  : shockshield            3 plat\n\r",ch); 
//        send_to_char("  fire   : fireshield             2 plat\n\r",ch);
//        send_to_char("  ice    : iceshield              1 plat\n\r",ch);
//        send_to_char("  pshield: poisonshield          50 gold\n\r",ch);

        send_to_char
            ( "  {WCancel {x:{c Cancellation{y            90 {Ygold{x\n\r",
              ch );
        send_to_char ( " \n\r{xType heal <{rtype{x> to be healed.\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg, "light" ) )
    {
        spell = spell_cure_light;
        sn = skill_lookup ( "cure light" );
        words = "judicandus dies";
        cost = 1000;
    }

    else if ( !str_prefix ( arg, "serious" ) )
    {
        spell = spell_cure_serious;
        sn = skill_lookup ( "cure serious" );
        words = "judicandus gzfuajg";
        cost = 1600;
    }

    else if ( !str_prefix ( arg, "critical" ) )
    {
        spell = spell_cure_critical;
        sn = skill_lookup ( "cure critical" );
        words = "judicandus qfuhuqar";
        cost = 2500;
    }

    else if ( !str_prefix ( arg, "heal" ) )
    {
        spell = spell_heal;
        sn = skill_lookup ( "heal" );
        words = "pzar";
        cost = 5000;
    }

    else if ( !str_prefix ( arg, "blindness" ) )
    {
        spell = spell_cure_blindness;
        sn = skill_lookup ( "cure blindness" );
        words = "judicandus noselacri";
        cost = 2000;
    }

    else if ( !str_prefix ( arg, "disease" ) )
    {
        spell = spell_cure_disease;
        sn = skill_lookup ( "cure disease" );
        words = "judicandus eugzagz";
        cost = 1500;
    }

    else if ( !str_prefix ( arg, "cancel" ) )
    {
        spell = spell_cancellation;
        sn = skill_lookup ( "cancellation" );
        words = "judicandus eugzagz";
        cost = 9000;
    }

    else if ( !str_prefix ( arg, "poison" ) )
    {
        spell = spell_cure_poison;
        sn = skill_lookup ( "cure poison" );
        words = "judicandus sausabru";
        cost = 2500;
    }

    else if ( !str_prefix ( arg, "uncurse" ) || !str_prefix ( arg, "curse" ) )
    {
        spell = spell_remove_curse;
        sn = skill_lookup ( "remove curse" );
        words = "candussido judifgz";
        cost = 5000;
    }

    else if ( !str_prefix ( arg, "mana" ) || !str_prefix ( arg, "energize" ) )
    {
        spell = NULL;
        sn = -1;
        words = "energizer";
        cost = 1000;
    }

    else if ( !str_prefix ( arg, "refresh" ) || !str_prefix ( arg, "moves" ) )
    {
        spell = spell_refresh;
        sn = skill_lookup ( "refresh" );
        words = "candusima";
        cost = 500;
    }
/*
    else if (!str_prefix(arg, "acid"))
    {
        spell = spell_acidshield;
        sn    = skill_lookup("acidshield");
        words = "acidshield";
        cost  = 40000;
    }
    else if (!str_prefix(arg, "shock"))
    {
	spell = spell_shockshield;
        sn    = skill_lookup("shockshield");
        words = "shockshield";
        cost  = 30000;
    }
    else if (!str_prefix(arg, "fire"))
    {
	spell = spell_fireshield;
	sn    = skill_lookup("fireshield");
 	words = "fireshield";
	cost  = 20000;
    }
    else if (!str_prefix(arg, "ice"))
    {
       	spell = spell_iceshield;
	sn    = skill_lookup("iceshield");
        words = "iceshield";
        cost  = 10000;
    }
       else if (!str_prefix(arg, "pshield"))
    {
        spell = spell_poisonshield;
        sn    = skill_lookup("poisonshield");
        words = "poisonshield";
        cost  = 5000;
    }*/
    else
    {
        act ( "$N says '{aType 'heal' for a list of spells.{x'", ch, NULL,
              mob, TO_CHAR );
        return;
    }

    if ( cost >
         ( ( ch->platinum * 10000 ) + ( ch->gold * 100 ) + ch->silver ) )
    {
        act ( "$N says '{aYou do not have enough gold for my services.{x'",
              ch, NULL, mob, TO_CHAR );
        return;
    }

    WAIT_STATE ( ch, PULSE_VIOLENCE );

    deduct_cost ( ch, cost, VALUE_SILVER );
    act ( "$n utters the words '{a$T{x'.", mob, NULL, words, TO_ROOM );

    if ( spell == NULL )        /* restore mana trap...kinda hackish */
    {
        ch->mana += dice ( 2, 8 ) + mob->level / 3;
        ch->mana = UMIN ( ch->mana, ch->max_mana );
        send_to_char ( "A warm {Yglow{x passes through you.\n\r", ch );
        return;
    }

    if ( sn == -1 )
        return;

    spell ( sn, mob->level, mob, ch, TARGET_CHAR );
}
